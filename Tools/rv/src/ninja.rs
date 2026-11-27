// SPDX-License-Identifier: BSD-2-Clause
// Copyright (c) 2026 The ravynOS Authors
use std::collections::BTreeMap;
use std::path::Path;

pub fn emit(
    manifests: &BTreeMap<String, crate::manifest::Manifest>,
    order: &[String],
    target: &str,
) -> String {
    let host = target.ends_with("-linux-gnu");
    let mut out = String::new();
    out.push_str(&format!("target = {target}\n"));
    out.push_str("rule lib\n  command = llvm-ar rcs $out $in\n");
    if host {
        out.push_str(
            "rule cc\n  command = clang -c $in -o $out $includes\n\
             rule link\n  command = clang -fuse-ld=lld $in -o $out\n\n",
        );
    } else if is_macho_target(target) {
        let arch = macho_arch(target);
        out.push_str(&format!(
            "rule cc\n  command = clang --target={arch}-apple-macos11 -ffreestanding -fno-builtin -nostdinc -c $in -o $out $includes\n\
             rule link\n  command = ld64.lld -arch {arch} -platform_version macos 11.0 11.0 -e __start $in -o $out\n\
             rule dylib\n  command = ld64.lld -arch {arch} -platform_version macos 11.0 11.0 -dylib $in -o $out\n\n"
        ));
    } else {
        out.push_str(&format!(
            "rule cc\n  command = clang --target=$target -ffreestanding -fno-builtin -nostdinc -c $in -o $out $includes\n\
             rule link\n  command = ld.lld {} -static $in -o $out\n\n",
            ld_emulation(target)
        ));
    }
    for name in order {
        let m = &manifests[name];
        let comp_dir = &m.name;
        let includes = include_flags(manifests, m);
        let files = list_files(&m.dir);
        let mut srcs = Vec::new();
        for pattern in &m.sources {
            for file in files.iter().filter(|f| glob(pattern, f)) {
                if !srcs.contains(file) {
                    srcs.push(file.clone());
                }
            }
        }
        let mut objs = Vec::new();
        for src in &srcs {
            if !src.ends_with(".c") {
                continue;
            }
            let obj = src.replace('/', "_");
            out.push_str(&format!(
                "build {comp_dir}/{obj}.o: cc ../../{}/{src}\n  includes = {includes}\n",
                m.dir.display()
            ));
            objs.push(format!("{comp_dir}/{obj}.o"));
        }
        if objs.is_empty() {
            continue;
        }
        let kind = m.kind.as_str();
        if kind == "static-lib" || kind == "data" {
            if let Some(lib) = m.exports.libs.first() {
                out.push_str(&format!("build {comp_dir}/{lib}: lib {}\n", objs.join(" ")));
                continue;
            }
        }
        if kind == "dylib" {
            let lib = m
                .exports
                .libs
                .first()
                .cloned()
                .unwrap_or_else(|| format!("lib{}.dylib", m.name));
            out.push_str(&format!(
                "build {comp_dir}/{lib}: dylib {}\n",
                objs.join(" ")
            ));
            continue;
        }
        let libs: Vec<String> = m
            .deps
            .internal
            .iter()
            .map(|d| lib_output(&manifests[d]))
            .collect();
        out.push_str(&format!(
            "build {comp_dir}/{}: link {} {}\n",
            m.name,
            objs.join(" "),
            libs.join(" ")
        ));
    }
    out
}

fn lib_output(m: &crate::manifest::Manifest) -> String {
    match m.exports.libs.first() {
        Some(lib) => format!("{}/{lib}", m.name),
        None => format!("{}/{}.a", m.name, m.name),
    }
}

fn include_flags(
    manifests: &BTreeMap<String, crate::manifest::Manifest>,
    m: &crate::manifest::Manifest,
) -> String {
    let mut flags = Vec::new();
    for pattern in &m.exports.headers {
        let dir = include_dir(pattern);
        flags.push(inc_flag(&m.dir.display().to_string(), &dir));
    }
    for dep in &m.deps.internal {
        let dep_m = &manifests[dep];
        for pattern in &dep_m.exports.headers {
            let dir = include_dir(pattern);
            flags.push(inc_flag(&dep_m.dir.display().to_string(), &dir));
        }
    }
    flags.join(" ")
}

fn inc_flag(comp_dir: &str, dir: &str) -> String {
    if dir == "." {
        format!("-I ../../{comp_dir}")
    } else {
        format!("-I ../../{comp_dir}/{dir}")
    }
}

fn include_dir(pattern: &str) -> String {
    let mut segs = Vec::new();
    for seg in pattern.split('/') {
        if seg.contains('*') {
            break;
        }
        segs.push(seg);
    }
    if segs.is_empty() {
        String::from(".")
    } else {
        segs.join("/")
    }
}

fn list_files(dir: &Path) -> Vec<String> {
    let mut files = Vec::new();
    walk_files(dir, dir, &mut files);
    files.sort();
    files
}

fn walk_files(root: &Path, dir: &Path, files: &mut Vec<String>) {
    let Ok(entries) = std::fs::read_dir(dir) else {
        return;
    };
    for entry in entries.flatten() {
        let path = entry.path();
        if path.is_dir() {
            walk_files(root, &path, files);
        } else if let Ok(rel) = path.strip_prefix(root) {
            files.push(rel.display().to_string());
        }
    }
}

fn glob(pattern: &str, path: &str) -> bool {
    let p: Vec<&str> = pattern.split('/').filter(|s| !s.is_empty()).collect();
    let f: Vec<&str> = path.split('/').filter(|s| !s.is_empty()).collect();
    segs_match(&p, &f)
}

fn segs_match(pat: &[&str], path: &[&str]) -> bool {
    match pat.split_first() {
        None => path.is_empty(),
        Some((seg, rest)) if *seg == "**" => (0..=path.len()).any(|n| segs_match(rest, &path[n..])),
        Some((seg, rest)) if seg.starts_with("**") => {
            let tail = format!("*{}", &seg[2..]);
            (1..=path.len()).any(|n| wildcard(&tail, path[n - 1]) && segs_match(rest, &path[n..]))
        }
        Some((seg, rest)) => {
            !path.is_empty() && wildcard(seg, path[0]) && segs_match(rest, &path[1..])
        }
    }
}

fn wildcard(pat: &str, s: &str) -> bool {
    let (p, s) = (pat.as_bytes(), s.as_bytes());
    let (mut pi, mut si) = (0usize, 0usize);
    let (mut star, mut mark) = (None, 0usize);
    while si < s.len() {
        if pi < p.len() && (p[pi] == b'?' || p[pi] == s[si]) {
            pi += 1;
            si += 1;
        } else if pi < p.len() && p[pi] == b'*' {
            star = Some(pi);
            pi += 1;
            mark = si;
        } else if let Some(st) = star {
            pi = st + 1;
            mark += 1;
            si = mark;
        } else {
            return false;
        }
    }
    while pi < p.len() && p[pi] == b'*' {
        pi += 1;
    }
    pi == p.len()
}

fn ld_emulation(target: &str) -> &'static str {
    if target.starts_with("x86_64") {
        "-m elf_x86_64"
    } else if target.starts_with("arm64") || target.starts_with("aarch64") {
        "-m aarch64elf"
    } else if target.starts_with("riscv64") {
        "-m elf64lriscv"
    } else {
        ""
    }
}

/// A Mach-O target gets clang as the linker driver so it picks lld with the
/// right emulation; ld.lld's bare `-m` flags are ELF-only.
fn is_macho_target(target: &str) -> bool {
    target.starts_with("x86_64") || target.starts_with("arm64") || target.starts_with("aarch64")
}

/// ld64.lld's -arch spelling for the target triple.
fn macho_arch(target: &str) -> &'static str {
    if target.starts_with("arm64") || target.starts_with("aarch64") {
        "arm64"
    } else {
        "x86_64"
    }
}
