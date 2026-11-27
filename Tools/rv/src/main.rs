// SPDX-License-Identifier: BSD-2-Clause
// Copyright (c) 2026 The ravynOS Authors
mod audit;
mod bootstrap;
mod graph;
mod manifest;
mod ninja;
mod vendors;

use std::collections::BTreeMap;
use std::process::exit;

#[derive(Clone, Copy, PartialEq)]
enum Cmd {
    Build,
    Bootstrap,
    Graph,
    Shell,
    Audit,
    AuditSize,
    Sdk,
}

fn usage() -> ! {
    eprintln!(
        "usage: rv [--root <tree>] <command>\n\
         usage: rv build [-t <target>] [<component>|<layer>...]\n\
         usage: rv bootstrap [--verify] [--refetch] [-j <jobs>]\n\
         usage: rv graph [--reverse] [--forbid]\n\
         usage: rv shell [<component>]\n\
         usage: rv sdk [-t <target>]\n\
         usage: rv vendors check\n\
         usage: rv audit [license|size] [-t <target>]"
    );
    exit(2);
}

fn main() {
    let mut args: Vec<String> = std::env::args().skip(1).collect();
    let mut root: Option<std::path::PathBuf> = None;
    while args.first().map(String::as_str) == Some("--root") {
        args.remove(0);
        let Some(dir) = (!args.is_empty()).then(|| args.remove(0)) else {
            usage();
        };
        root = Some(std::path::PathBuf::from(dir));
    }
    let Some(cmd_arg) = (!args.is_empty()).then(|| args.remove(0)) else {
        usage();
    };
    let tree = match root {
        Some(dir) => dir,
        None => std::env::current_dir().expect("rv must run from the tree root"),
    };

    if cmd_arg == "vendors" {
        match args.first().map(String::as_str) {
            None | Some("check") => report(vendors::check(&tree)),
            _ => usage(),
        }
        return;
    }

    let mut target = String::from("x86_64-ravynos");
    let mut names: Vec<String> = Vec::new();
    let mut reverse = false;

    let cmd = match cmd_arg.as_str() {
        "build" => {
            let mut rest = args.iter().peekable();
            while let Some(arg) = rest.peek() {
                if arg.as_str() == "-t" {
                    rest.next();
                    target = rest.next().cloned().unwrap_or_else(|| usage());
                } else {
                    break;
                }
            }
            names = rest.cloned().collect();
            Cmd::Build
        }
        "graph" => {
            for arg in &args {
                match arg.as_str() {
                    "--reverse" => reverse = true,
                    "--forbid" => {}
                    _ => usage(),
                }
            }
            Cmd::Graph
        }
        "bootstrap" => Cmd::Bootstrap,
        "sdk" => {
            if let Some(t) = args.iter().position(|a| a == "-t") {
                target = args.get(t + 1).cloned().unwrap_or_else(|| usage());
            }
            Cmd::Sdk
        }
        "shell" => {
            names = args.clone();
            Cmd::Shell
        }
        "audit" => {
            if let Some(t) = args.iter().position(|a| a == "-t") {
                target = args.get(t + 1).cloned().unwrap_or_else(|| usage());
            }
            match args.first().map(String::as_str) {
                None | Some("license") => Cmd::Audit,
                Some("size") => Cmd::AuditSize,
                _ => usage(),
            }
        }
        _ => usage(),
    };

    report(run(cmd, &tree, &args, &target, &names, reverse));
}

fn run(
    cmd: Cmd,
    tree: &std::path::Path,
    args: &[String],
    target: &str,
    names: &[String],
    reverse: bool,
) -> Result<(), String> {
    if !tree.is_dir() {
        return Err(format!(
            "error: E-ROOT: {} is not a directory (rv --root <tree>)",
            tree.display()
        ));
    }
    let manifests = manifest::load_all(tree)?;
    let deps = graph::resolve(&manifests)?;
    match cmd {
        Cmd::Build => build(tree, &manifests, &deps, target, names),
        Cmd::Bootstrap => bootstrap::run(tree, args),
        Cmd::Graph => {
            print_graph(&manifests, &deps, reverse);
            Ok(())
        }
        Cmd::Shell => shell(tree, &manifests, &deps, target, names.first()),
        Cmd::Audit => audit::license(tree, &manifests),
        Cmd::AuditSize => audit_size(tree, target),
        Cmd::Sdk => sdk(tree, &manifests, target),
    }
}

fn report(result: Result<(), String>) {
    if let Err(message) = result {
        eprintln!("{message}");
        exit(1);
    }
}

fn select(
    manifests: &BTreeMap<String, manifest::Manifest>,
    deps: &BTreeMap<String, Vec<String>>,
    names: &[String],
) -> Result<Vec<String>, String> {
    let names: Vec<String> = if names.iter().any(|n| n.as_str() == "all") {
        manifests.keys().cloned().collect()
    } else {
        names.to_vec()
    };
    if names.is_empty() {
        return Ok(manifests.keys().cloned().collect());
    }
    let mut selected = std::collections::BTreeSet::new();
    for name in &names {
        if manifests.contains_key(name.as_str()) {
            selected.insert(name.clone());
        } else if graph::is_layer(name) {
            for (comp, m) in manifests {
                if m.layer == *name {
                    selected.insert(comp.clone());
                }
            }
        } else {
            let known: Vec<&String> = manifests.keys().collect();
            let mut layers: Vec<&str> = Vec::new();
            graph::each_layer(|l| layers.push(l));
            return Err(format!(
                "error: no component or layer named '{name}'\n       components: {}\n       layers: {}",
                known.iter().map(|s| s.as_str()).collect::<Vec<_>>().join(", "),
                layers.join(", ")
            ));
        }
    }
    let mut order = Vec::new();
    graph::closure(deps, &selected, &mut order)?;
    Ok(order)
}

fn build(
    tree: &std::path::Path,
    manifests: &BTreeMap<String, manifest::Manifest>,
    deps: &BTreeMap<String, Vec<String>>,
    target: &str,
    names: &[String],
) -> Result<(), String> {
    audit::license(tree, manifests)?;
    for m in manifests.values() {
        if !m.targets.is_empty() && !m.targets.iter().any(|t| t == target) {
            return Err(format!(
                "error: {} does not support target {target}\n       targets: {}",
                m.name,
                m.targets.join(", ")
            ));
        }
    }
    let order = select(manifests, deps, names)?;
    let build_dir = tree.join("build").join(target);
    std::fs::create_dir_all(&build_dir).map_err(|e| format!("error: create build dir: {e}"))?;
    let ninja_file = build_dir.join("build.ninja");
    std::fs::write(&ninja_file, ninja::emit(manifests, &order, target))
        .map_err(|e| format!("error: write build.ninja: {e}"))?;
    for tool in ["ninja", "clang", "llvm-ar", "ld.lld"] {
        if std::process::Command::new(tool)
            .arg("--version")
            .output()
            .is_err()
        {
            return Err(format!(
                "error: {tool} not found on PATH (the build uses clang, lld and llvm-ar only)"
            ));
        }
    }
    let status = std::process::Command::new("ninja")
        .arg("-C")
        .arg(&build_dir)
        .status()
        .map_err(|e| format!("error: cannot run ninja: {e}"))?;
    if !status.success() {
        return Err(format!(
            "error: ninja failed in {} with {}",
            build_dir.display(),
            status
        ));
    }
    Ok(())
}

fn print_graph(
    manifests: &BTreeMap<String, manifest::Manifest>,
    deps: &BTreeMap<String, Vec<String>>,
    reverse: bool,
) {
    if reverse {
        let mut rdeps: BTreeMap<&str, Vec<&str>> = BTreeMap::new();
        for (comp, ds) in deps {
            for d in ds {
                rdeps.entry(d.as_str()).or_default().push(comp.as_str());
            }
        }
        for (comp, ds) in &rdeps {
            println!("{comp} <- {}", ds.join(" "));
        }
    } else {
        for (comp, ds) in deps {
            let layer = &manifests[comp].layer;
            if ds.is_empty() {
                println!("{comp} ({layer})");
            } else {
                println!("{comp} ({layer}) -> {}", ds.join(" "));
            }
        }
    }
}

fn shell(
    tree: &std::path::Path,
    manifests: &BTreeMap<String, manifest::Manifest>,
    deps: &BTreeMap<String, Vec<String>>,
    target: &str,
    name: Option<&String>,
) -> Result<(), String> {
    let comp = name.cloned().unwrap_or_else(|| "hello".to_string());
    if !manifests.contains_key(&comp) {
        return Err(format!("error: no component named '{comp}'"));
    }
    let _ = deps;
    let sysroot = tree.join("build").join("sysroots").join(target);
    println!(
        "export CC=clang\nexport CFLAGS=--target={target} --sysroot={}\nexport RV_BUILD_DIR={}\n[{} {target}] $ ninja -C $RV_BUILD_DIR",
        sysroot.display(),
        tree.join("build").join(target).join(&comp).display(),
        comp
    );
    Ok(())
}

/// Assembles `build/sysroot/<target>` from what the build produced: every
/// component's public headers land in usr/include and its libraries in usr/lib,
/// so a component can compile against installed interfaces only (GAP-04).
fn sdk(
    tree: &std::path::Path,
    manifests: &BTreeMap<String, manifest::Manifest>,
    target: &str,
) -> Result<(), String> {
    let sysroot = tree.join("build").join("sysroot").join(target);
    let include = sysroot.join("usr").join("include");
    let lib = sysroot.join("usr").join("lib");
    for dir in [&include, &lib] {
        std::fs::create_dir_all(dir)
            .map_err(|e| format!("error: create {}: {e}", dir.display()))?;
    }
    let mut headers = 0;
    let mut libs = 0;
    let build_dir = tree.join("build").join(target);
    for m in manifests.values() {
        let headers_dir = tree.join(&m.dir).join("include");
        if headers_dir.is_dir() {
            for entry in std::fs::read_dir(&headers_dir)
                .map_err(|e| format!("error: read {}: {e}", headers_dir.display()))?
                .flatten()
            {
                let path = entry.path();
                if path.extension().map(|e| e == "h").unwrap_or(false) {
                    let name = path.file_name().unwrap_or_default();
                    std::fs::copy(&path, include.join(name))
                        .map_err(|e| format!("error: install {}: {e}", path.display()))?;
                    headers += 1;
                }
            }
        }
        let artifact = build_dir.join(&m.name);
        let candidates: Vec<String> = match m.kind.as_str() {
            "static-lib" => vec![format!("{}.a", m.name)],
            "dylib" => vec![format!("lib{}.dylib", m.name)],
            _ => Vec::new(),
        };
        for file in candidates {
            let src = artifact.join(&file);
            if src.is_file() {
                let dst = lib.join(src.file_name().unwrap_or_default());
                std::fs::copy(&src, &dst)
                    .map_err(|e| format!("error: install {}: {e}", src.display()))?;
                libs += 1;
            }
        }
    }
    println!(
        "sdk: {headers} headers and {libs} libraries in {}",
        sysroot.display()
    );
    Ok(())
}

/// Reported sizes of this target's build outputs, so a change that doubles a
/// binary is visible (GAP-06).
fn audit_size(tree: &std::path::Path, target: &str) -> Result<(), String> {
    let build_dir = tree.join("build").join(target);
    if !build_dir.is_dir() {
        return Err(format!(
            "error: nothing built for {target} ({} does not exist)",
            build_dir.display()
        ));
    }
    let mut rows: Vec<(String, u64)> = Vec::new();
    let mut total = 0;
    for entry in std::fs::read_dir(&build_dir)
        .map_err(|e| format!("error: read {}: {e}", build_dir.display()))?
        .flatten()
    {
        let path = entry.path();
        if !path.is_dir() || path.file_name().map(|n| n == "nobuild").unwrap_or(false) {
            continue;
        }
        for file in std::fs::read_dir(&path)
            .map_err(|e| format!("error: read {}: {e}", path.display()))?
            .flatten()
        {
            let file = file.path();
            if file.is_dir() {
                continue;
            }
            let size = std::fs::metadata(&file).map(|m| m.len()).unwrap_or(0);
            total += size;
            rows.push((file.display().to_string(), size));
        }
    }
    rows.sort();
    for (path, size) in &rows {
        println!("{size:>10}  {path}");
    }
    println!("{total:>10}  total ({target})");
    Ok(())
}
