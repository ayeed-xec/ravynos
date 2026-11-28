// SPDX-License-Identifier: BSD-2-Clause
// Copyright (c) 2026 The ravynOS Authors
use crate::manifest::Manifest;
use std::collections::BTreeMap;
use std::path::Path;

/// Bundle kinds: `kind`, its extension and the linker rule that produces it
/// (S01, S03). A kext links with Apple `ld64 -kext`; `ld64.lld` has no such mode.
const BUNDLES: &[(&str, &str, &str)] = &[
    ("app", ".app", "app"),
    ("xpc-service", ".xpc", "bundle"),
    ("dext", ".dext", "bundle"),
    ("plugin", ".plugin", "bundle"),
    ("bundle", ".bundle", "bundle"),
    ("kext", ".kext", "kext"),
];

/// The OS version Darwin-target components are built for (ADR-01: macOS
/// 27.2.1): the `macosx<v>` triple of ADR-03 and the minos and SDK version in
/// `LC_BUILD_VERSION` (S01 target table).
const DEPLOYMENT: &str = "27.2";

pub fn emit(manifests: &BTreeMap<String, Manifest>, order: &[String], target: &str) -> String {
    let sysroot = format!("../sysroot/{target}");
    let mut out = String::new();
    out.push_str(&format!("target = {target}\n"));
    if is_macho_target(target) {
        out.push_str(&format!("arch = {}\n", macho_arch(target)));
    }
    out.push_str("rule lib\n  command = llvm-ar rcs $out $in\n");
    let (cc_flags, rules) = compile_and_link_rules(target, &sysroot);
    for (rule, dialect) in [
        ("cc", ""),
        ("cxx", "-x c++ "),
        ("objc", "-x objective-c "),
        ("objcxx", "-x objective-c++ "),
    ] {
        out.push_str(&format!(
            "rule {rule}\n  command = clang {cc_flags}{dialect}-I{sysroot}/usr/include -MMD -MF $out.d -c $in -o $out $includes\n  depfile = $out.d\n  deps = gcc\n"
        ));
    }
    out.push_str(&format!(
        "rule as\n  command = clang {cc_flags}-c $in -o $out $includes\n"
    ));
    out.push_str(&rules);
    out.push('\n');
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
            let Some(rule) = compile_rule(src) else {
                continue;
            };
            let obj = src.replace('/', "_");
            out.push_str(&format!(
                "build {comp_dir}/{obj}.o: {rule} ../../{}/{src}\n  includes = {includes}\n",
                m.dir.display()
            ));
            objs.push(format!("{comp_dir}/{obj}.o"));
        }
        if objs.is_empty() {
            continue;
        }
        let Some(mut artifact) = artifact(m, target) else {
            continue;
        };
        artifact.flags = link_flags(m, manifests);
        let libs: Vec<String> = m
            .deps
            .internal
            .iter()
            .map(|d| link_input(&manifests[d]))
            .collect();
        out.push_str(&artifact.build(&objs, &libs));
    }
    out
}

/// One component's link: which rule runs, where the output lands and what else
/// has to exist beside it (framework symlinks, `Info.plist`).
struct Artifact {
    rule: &'static str,
    path: String,
    install_name: Option<String>,
    /// Linker options this component adds: versions, re-exports, umbrella.
    flags: Vec<String>,
    extras: String,
    implicit: Vec<String>,
}

impl Artifact {
    fn plain(rule: &'static str, path: String) -> Self {
        Artifact {
            rule,
            path,
            install_name: None,
            flags: Vec::new(),
            extras: String::new(),
            implicit: Vec::new(),
        }
    }

    fn build(&self, objs: &[String], libs: &[String]) -> String {
        let mut line = if self.rule == "lib" {
            format!("build {}: lib {}\n", self.path, objs.join(" "))
        } else {
            format!(
                "build {}: {} {} {}\n",
                self.path,
                self.rule,
                objs.join(" "),
                libs.join(" ")
            )
        };
        if let Some(install_name) = &self.install_name {
            line.push_str(&format!("  install_name = {install_name}\n"));
        }
        if !self.flags.is_empty() {
            line.push_str(&format!("  link_flags = {}\n", self.flags.join(" ")));
        }
        if !self.implicit.is_empty() {
            let note = format!("  | {}\n", self.implicit.join(" "));
            line.push_str(&note);
        }
        line.push_str(&self.extras);
        line
    }
}

/// The S03 linker contract: Apple's version numbers, re-exported dependents,
/// and the umbrella a libSystem piece names.
fn link_flags(m: &Manifest, manifests: &BTreeMap<String, Manifest>) -> Vec<String> {
    let mut flags = Vec::new();
    if !m.current_version.is_empty() {
        flags.push(format!("-current_version {}", m.current_version));
    }
    if !m.compatibility_version.is_empty() {
        flags.push(format!(
            "-compatibility_version {}",
            m.compatibility_version
        ));
    }
    if !m.umbrella.is_empty() {
        flags.push(format!("-umbrella {}", m.umbrella));
    }
    for dep in &m.reexports {
        flags.push(format!("-reexport_library {}", link_input(&manifests[dep])));
    }
    for dep in &m.deps.upward {
        flags.push(format!("-upward_library {}", link_input(&manifests[dep])));
    }
    flags
}

/// Where a component's output lands (S01 output kinds, S03 bundle layout), or
/// None for a component that produces no file of its own.
fn artifact(m: &Manifest, target: &str) -> Option<Artifact> {
    let kind = m.kind.as_str();
    let exported = || m.exports.libs.first().cloned();
    match kind {
        "static-lib" | "data" => {
            exported().map(|lib| Artifact::plain("lib", format!("{}/{lib}", m.name)))
        }
        "dylib" => {
            let lib = exported().unwrap_or_else(|| format!("lib{}.dylib", m.name));
            let mut a = Artifact::plain("dylib", format!("{}/{lib}", m.name));
            a.install_name = Some(install_name(m, &lib));
            Some(a)
        }
        "framework" => Some(framework(m, target)),
        _ => BUNDLES
            .iter()
            .find(|(k, _, _)| *k == kind)
            .and_then(|(_, ext, rule)| bundle(m, ext, rule, target)),
    }
    .or_else(|| {
        // executable, test-kernel, host-tool and anything without a bundle layout
        // link to a plain file beside the component's build directory.
        let plain = matches!(
            kind,
            "executable" | "test-kernel" | "host-tool" | "uefi-app"
        );
        plain.then(|| Artifact::plain("link", format!("{}/{}", m.name, m.name)))
    })
}

fn framework(m: &Manifest, target: &str) -> Artifact {
    let version = if m.framework_version.is_empty() {
        "A"
    } else {
        m.framework_version.as_str()
    };
    if !is_macho_target(target) {
        return Artifact::plain("link", format!("{}/{}", m.name, m.name));
    }
    let base = format!("{}/{}.framework", m.name, m.name);
    let path = format!("{base}/Versions/{version}/{}", m.name);
    let mut a = Artifact::plain("dylib", path);
    a.install_name = Some(install_name(
        m,
        &format!("{}.framework/Versions/{version}/{}", m.name, m.name),
    ));
    a.extras = format!(
        "build {base}/Versions/Current: symlink {base}/Versions/{version}/{}\n  link = {version}\n\
         build {base}/{}: symlink {base}/Versions/Current\n  link = Versions/Current/{}\n",
        m.name, m.name, m.name
    );
    a
}

/// An app, xpc-service, dext, plugin, bundle or kext: the binary goes to
/// `Contents/MacOS/` and the plist template is copied beside it.
fn bundle(m: &Manifest, ext: &str, rule: &'static str, target: &str) -> Option<Artifact> {
    if !is_macho_target(target) {
        return Some(Artifact::plain("link", format!("{}/{}", m.name, m.name)));
    }
    let base = format!("{}/{}{ext}", m.name, m.name);
    let mut a = Artifact::plain(rule, format!("{base}/Contents/MacOS/{}", m.name));
    if rule == "kext" {
        a.install_name = None;
    }
    if !m.info_plist.is_empty() {
        let plist = format!("{base}/Contents/Info.plist");
        a.extras = format!(
            "build {plist}: plist ../../{}/{}\n",
            m.dir.display(),
            m.info_plist
        );
        a.implicit.push(plist);
    }
    Some(a)
}

/// The `-install_name` a dylib or framework carries (S03): the manifest's value
/// when it has one, otherwise an `@rpath` path the dyld search finds.
fn install_name(m: &Manifest, fallback: &str) -> String {
    if m.install_name.is_empty() {
        format!("@rpath/{fallback}")
    } else {
        m.install_name.clone()
    }
}

/// A dependent links a producer's exported `.tbd` stub by name (import-only
/// linking, S03); without a stub it links the artifact the build produced.
fn link_input(m: &Manifest) -> String {
    match m.exports.libs.iter().find(|l| l.ends_with(".tbd")) {
        Some(stub) => format!(
            "-l{}",
            stub.trim_end_matches(".tbd").trim_start_matches("lib")
        ),
        None => lib_output(m),
    }
}

fn lib_output(m: &Manifest) -> String {
    match m.exports.libs.first() {
        Some(lib) => format!("{}/{lib}", m.name),
        None => format!("{}/{}.a", m.name, m.name),
    }
}

fn include_flags(manifests: &BTreeMap<String, Manifest>, m: &Manifest) -> String {
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

fn compile_and_link_rules(target: &str, sysroot: &str) -> (String, String) {
    if target.ends_with("-linux-gnu") {
        return (
            String::new(),
            format!(
                "rule link\n  command = clang -fuse-ld=lld -L {sysroot}/usr/lib $in -o $out\n\
                 rule plist\n  command = cp $in $out\n"
            ),
        );
    }
    if is_macho_target(target) {
        // Chained fixups are what Apple's linker emits for this deployment
        // target, and binding through them needs no dyld_stub_binder from a
        // libSystem that does not exist yet.
        let platform =
            format!("-arch $arch -platform_version macos {DEPLOYMENT} {DEPLOYMENT} -fixup_chains");
        return (
            format!(
                "--target={}-apple-macosx{DEPLOYMENT} -ffreestanding -fno-builtin -nostdinc -isysroot {sysroot} ",
                macho_arch(target)
            ),
            format!(
                "rule link\n  command = ld64.lld {platform} -e __start $link_flags -L {sysroot}/usr/lib $in -o $out\n\
                 rule dylib\n  command = ld64.lld {platform} -dylib -install_name $install_name $link_flags -L {sysroot}/usr/lib $in -o $out\n\
                 rule app\n  command = ld64.lld {platform} $link_flags -L {sysroot}/usr/lib $in -o $out\n\
                 rule bundle\n  command = ld64.lld {platform} -bundle $link_flags -L {sysroot}/usr/lib $in -o $out\n\
                 rule kext\n  command = ld64 -arch $arch -kext $link_flags $in -o $out\n\
                 rule plist\n  command = cp $in $out\n\
                 rule symlink\n  command = ln -sfn $link $out\n"
            ),
        );
    }
    (
        format!("--target={target} -ffreestanding -fno-builtin -nostdinc -isysroot {sysroot} "),
        format!(
            "rule link\n  command = ld.lld {} -static -L {sysroot}/usr/lib $in -o $out\n\
             rule plist\n  command = cp $in $out\n",
            ld_emulation(target)
        ),
    )
}

/// The ninja rule that compiles a source file, or None for a file we do not
/// compile (headers, data). Extensions decide the language the way clang does.
fn compile_rule(src: &str) -> Option<&'static str> {
    match src.rsplit('.').next()? {
        "c" => Some("cc"),
        "m" => Some("objc"),
        "mm" => Some("objcxx"),
        "cc" | "cpp" | "cxx" => Some("cxx"),
        "S" | "s" => Some("as"),
        _ => None,
    }
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

#[cfg(test)]
mod tests {
    use super::*;
    use std::fs;
    use std::path::PathBuf;

    /// Parse a manifest and give it a real source tree, because `emit` lists the
    /// component directory to expand `sources`.
    fn manifest(text: &str) -> Manifest {
        let mut m = crate::manifest::parse(text).expect("manifest parses");
        let dir = std::env::temp_dir()
            .join("rv-ninja-fixtures")
            .join(&m.layer)
            .join(&m.name);
        fs::create_dir_all(dir.join("src")).expect("fixture src dir");
        fs::write(dir.join("src/main.c"), "int main(void) { return 0; }\n")
            .expect("fixture source");
        m.dir = dir;
        m
    }

    fn build_ninja(components: &[Manifest], target: &str) -> String {
        let mut manifests = BTreeMap::new();
        for m in components {
            manifests.insert(m.name.clone(), m.clone());
        }
        let order: Vec<String> = manifests.keys().cloned().collect();
        emit(&manifests, &order, target)
    }

    const DYLIB: &str = "name = \"libhello\"\nlayer = \"foundation\"\nlicense = \"BSD-2-Clause\"\n\
                         state = \"working\"\nkind = \"dylib\"\nsources = [\"src/*.c\"]\n\
                         [exports]\nlibs = [\"libhello.dylib\"]\nheaders = [\"include/*.h\"]\n";

    /// Replace the fixture's `kind` and add keys above the first table header,
    /// so the added keys stay top-level instead of landing inside `[exports]`.
    fn with(text: &str, extra: &str) -> String {
        let mut out = String::new();
        let mut inserted = false;
        for line in text.lines() {
            if !inserted && line.starts_with('[') {
                out.push_str(extra);
                inserted = true;
            }
            if !line.trim_start().starts_with("kind =") {
                out.push_str(line);
                out.push('\n');
            }
        }
        if !inserted {
            out.push_str(extra);
        }
        out
    }

    #[test]
    fn dylib_gets_a_bundle_install_name_by_default() {
        let ninja = build_ninja(&[manifest(DYLIB)], "x86_64-ravynos");
        assert!(
            ninja.contains("install_name = @rpath/libhello.dylib"),
            "{ninja}"
        );
        assert!(
            ninja.contains("-dylib -install_name $install_name"),
            "{ninja}"
        );
    }

    #[test]
    fn framework_lands_at_its_apple_path_with_symlinks() {
        let m = manifest(&with(
            &DYLIB.replace("libhello", "Example"),
            "kind = \"framework\"\nframework_version = \"A\"\n",
        ));
        let ninja = build_ninja(&[m], "x86_64-ravynos");
        assert!(
            ninja.contains("build Example/Example.framework/Versions/A/Example: dylib"),
            "{ninja}"
        );
        assert!(
            ninja.contains("build Example/Example.framework/Versions/Current: symlink"),
            "{ninja}"
        );
        assert!(
            ninja.contains("install_name = @rpath/Example.framework/Versions/A/Example"),
            "{ninja}"
        );
    }

    #[test]
    fn framework_version_defaults_to_a() {
        let text = with(
            &DYLIB.replace("libhello", "Example"),
            "kind = \"framework\"\n",
        );
        let ninja = build_ninja(&[manifest(&text)], "arm64-ravynos");
        assert!(ninja.contains("Versions/A/Example: dylib"), "{ninja}");
    }

    #[test]
    fn app_and_kext_get_their_bundle_layouts() {
        let app = manifest(&with(
            &DYLIB.replace("libhello", "prefs"),
            "kind = \"app\"\nbundle_id = \"org.ravynos.prefs\"\n",
        ));
        let kext = manifest(&with(
            &DYLIB.replace("libhello", "hfs"),
            "kind = \"kext\"\nbundle_id = \"org.ravynos.hfs\"\n",
        ));
        let ninja = build_ninja(&[app, kext], "x86_64-ravynos");
        assert!(
            ninja.contains("build prefs/prefs.app/Contents/MacOS/prefs: app"),
            "{ninja}"
        );
        assert!(
            ninja.contains("build hfs/hfs.kext/Contents/MacOS/hfs: kext"),
            "{ninja}"
        );
        assert!(ninja.contains("ld64 -arch $arch -kext"), "{ninja}");
    }

    #[test]
    fn info_plist_is_copied_into_the_bundle() {
        let app = manifest(&with(
            &DYLIB.replace("libhello", "prefs"),
            "kind = \"app\"\ninfo_plist = \"Info.plist\"\n",
        ));
        let ninja = build_ninja(&[app], "x86_64-ravynos");
        assert!(
            ninja.contains("build prefs/prefs.app/Contents/Info.plist: plist ../../"),
            "{ninja}"
        );
        assert!(ninja.contains("/Info.plist\n"), "{ninja}");
        assert!(
            ninja.contains("| prefs/prefs.app/Contents/Info.plist"),
            "{ninja}"
        );
    }

    #[test]
    fn every_compile_and_link_sees_the_sysroot() {
        let ninja = build_ninja(&[manifest(DYLIB)], "arm64-ravynos");
        assert!(
            ninja.contains("-isysroot ../sysroot/arm64-ravynos"),
            "{ninja}"
        );
        assert!(
            ninja.contains("-I../sysroot/arm64-ravynos/usr/include"),
            "{ninja}"
        );
        assert!(
            ninja.contains("-L ../sysroot/arm64-ravynos/usr/lib"),
            "{ninja}"
        );
    }

    #[test]
    fn a_stub_is_linked_by_name_and_an_artifact_by_path() {
        let stub = manifest(DYLIB);
        let mut with_stub = stub.clone();
        with_stub.exports.libs = vec![String::from("libhello.tbd")];
        assert_eq!(link_input(&with_stub), "-lhello");
        assert_eq!(link_input(&stub), "libhello/libhello.dylib");
    }

    #[test]
    fn dependents_link_the_exported_stub() {
        let producer = manifest(DYLIB);
        let user = manifest(
            "name = \"hello\"\nlayer = \"apps\"\nlicense = \"BSD-2-Clause\"\nstate = \"working\"\n\
             kind = \"executable\"\nsources = [\"src/*.c\"]\n[deps]\ninternal = [\"libhello\"]\n",
        );
        let mut manifests = BTreeMap::new();
        manifests.insert(producer.name.clone(), producer);
        manifests.insert(user.name.clone(), user);
        let mut stub = crate::manifest::parse(DYLIB).unwrap();
        stub.dir = PathBuf::from("Foundation/libhello");
        stub.exports.libs = vec![String::from("libhello.tbd")];
        manifests.insert(String::from("libhello"), stub);
        let ninja = emit(
            &manifests,
            &[String::from("libhello"), String::from("hello")],
            "x86_64-ravynos",
        );
        assert!(
            ninja.contains("build hello/hello: link hello/src_main.c.o -lhello"),
            "{ninja}"
        );
    }

    #[test]
    fn darwin_targets_use_the_adr03_triple_and_os_version() {
        let lib = manifest(
            "name = \"tri\"\nlayer = \"foundation\"\nlicense = \"BSD-2-Clause\"\n\
             state = \"working\"\nkind = \"dylib\"\nsources = [\"src/**.c\"]\n",
        );
        for (target, arch) in [("x86_64-ravynos", "x86_64"), ("arm64-ravynos", "arm64")] {
            let ninja = build_ninja(std::slice::from_ref(&lib), target);
            assert!(
                ninja.contains(&format!("--target={arch}-apple-macosx27.2 ")),
                "{target}: {ninja}"
            );
            assert!(
                ninja.contains("-platform_version macos 27.2 27.2 -fixup_chains"),
                "{target}: {ninja}"
            );
            assert!(!ninja.contains("macos11"), "{target} still names macOS 11");
        }
    }

    #[test]
    fn kext_needs_apple_ld64_and_says_so() {
        let kext = manifest(&with(
            &DYLIB.replace("libhello", "hfs"),
            "kind = \"kext\"\n",
        ));
        let ninja = build_ninja(&[kext], "x86_64-ravynos");
        assert!(!ninja.contains("ld64.lld -arch $arch -kext"), "{ninja}");
    }

    #[test]
    fn elf_targets_still_link_plain_files() {
        let m = manifest(&DYLIB.replace("libhello", "prefs"));
        let mut tool = m.clone();
        tool.kind = String::from("host-tool");
        tool.name = String::from("prefs");
        let ninja = build_ninja(&[tool], "x86_64-linux-gnu");
        assert!(ninja.contains("build prefs/prefs: link"), "{ninja}");
    }

    #[test]
    fn source_files_pick_the_clang_dialect() {
        let dir = std::env::temp_dir().join(format!("rv-ninja-{}", std::process::id()));
        let _ = fs::create_dir_all(dir.join("src"));
        for (file, body) in [
            ("src/main.c", "int main(void){return 0;}\n"),
            ("src/view.m", "int view;\n"),
            ("src/glue.mm", "int glue;\n"),
            ("src/entry.S", "nop\n"),
        ] {
            fs::write(dir.join(file), body).unwrap();
        }
        let mut m = crate::manifest::parse(
            "name = \"hello\"\nlayer = \"apps\"\nlicense = \"BSD-2-Clause\"\nstate = \"working\"\n\
             kind = \"executable\"\nsources = [\"src/*\"]\n",
        )
        .unwrap();
        m.dir = dir.clone();
        let ninja = build_ninja(&[m], "x86_64-ravynos");
        for rule in [
            "src_main.c.o: cc",
            "src_view.m.o: objc",
            "src_glue.mm.o: objcxx",
            "src_entry.S.o: as",
        ] {
            assert!(ninja.contains(rule), "missing {rule} in {ninja}");
        }
        fs::remove_dir_all(&dir).ok();
    }

    #[test]
    fn apple_versions_reexports_and_umbrella_reach_the_linker() {
        let core = manifest(&DYLIB.replace("libhello", "libcore"));
        let m = manifest(&DYLIB.replace(
            "[exports]",
            "current_version = \"1.2.3\"\ncompatibility_version = \"1.0.0\"\n\
             umbrella = \"System\"\nreexports = [\"libcore\"]\n\
             [deps]\ninternal = [\"libcore\"]\n[exports]",
        ));
        let ninja = build_ninja(&[core, m], "x86_64-ravynos");
        for flag in [
            "-current_version 1.2.3",
            "-compatibility_version 1.0.0",
            "-umbrella System",
            "-reexport_library libcore/libcore.dylib",
        ] {
            assert!(ninja.contains(flag), "missing {flag} in {ninja}");
        }
        assert!(ninja.contains("link_flags = "), "{ninja}");
    }

    #[test]
    fn upward_dependencies_link_as_upward_libraries() {
        let child = manifest(&DYLIB.replace("libhello", "libsystem_pthread"));
        let mut parent = manifest(&DYLIB.replace("libhello", "libsystem_c"));
        parent.deps.upward = vec![String::from("libsystem_pthread")];
        let ninja = build_ninja(&[child, parent], "arm64-ravynos");
        assert!(
            ninja.contains("-upward_library libsystem_pthread/libsystem_pthread.dylib"),
            "{ninja}"
        );
    }
}
