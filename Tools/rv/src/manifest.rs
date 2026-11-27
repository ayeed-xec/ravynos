// SPDX-License-Identifier: BSD-2-Clause
// Copyright (c) 2026 The ravynOS Authors
use serde::Deserialize;
use std::collections::BTreeMap;
use std::path::{Component, Path, PathBuf};

pub const KINDS: &[&str] = &[
    "static-lib",
    "dylib",
    "framework",
    "executable",
    "app",
    "xpc-service",
    "dext",
    "plugin",
    "bundle",
    "kext",
    "uefi-app",
    "test-kernel",
    "host-tool",
    "data",
];

pub const STATES: &[&str] = &["working", "incomplete", "experimental"];

pub const TARGETS: &[&str] = &[
    "x86_64-ravynos",
    "arm64-ravynos",
    "riscv64-ravynos",
    "x86_64-uefi",
    "arm64-uefi",
    "riscv64-uefi",
    "x86_64-testkernel",
    "arm64-testkernel",
    "riscv64-testkernel",
];

const SIGNS: &[&str] = &["adhoc", "none"];

#[allow(dead_code)]
#[derive(Debug, Deserialize)]
#[serde(deny_unknown_fields)]
pub struct Manifest {
    pub name: String,
    pub layer: String,
    pub license: String,
    pub state: String,
    pub kind: String,
    pub sources: Vec<String>,
    #[serde(default)]
    pub source_root: String,
    #[serde(default)]
    pub install_name: String,
    #[serde(default)]
    pub install_path: String,
    #[serde(default)]
    pub current_version: String,
    #[serde(default)]
    pub compatibility_version: String,
    #[serde(default)]
    pub reexports: Vec<String>,
    #[serde(default)]
    pub umbrella: String,
    #[serde(default)]
    pub hook: String,
    #[serde(default)]
    pub cargo: String,
    #[serde(default)]
    pub targets: Vec<String>,
    #[serde(default)]
    pub exports: Exports,
    #[serde(default)]
    pub deps: Deps,
    #[serde(default)]
    pub flags: BTreeMap<String, Flags>,
    #[serde(default)]
    pub vendored: Vendored,
    #[serde(default)]
    pub info_plist: String,
    #[serde(default)]
    pub bundle_id: String,
    #[serde(default)]
    pub resources: Vec<String>,
    #[serde(default)]
    pub entitlements: String,
    #[serde(default)]
    pub sign: String,
    #[serde(default)]
    pub embed: Vec<String>,
    #[serde(default)]
    pub framework_version: String,
    #[serde(skip)]
    pub dir: PathBuf,
}

#[derive(Debug, Deserialize, Default, Clone)]
#[serde(deny_unknown_fields)]
pub struct Deps {
    #[serde(default)]
    pub internal: Vec<String>,
    #[serde(default)]
    pub upward: Vec<String>,
    #[serde(default)]
    pub system: Vec<String>,
}

#[derive(Debug, Deserialize, Default)]
#[serde(deny_unknown_fields)]
pub struct Exports {
    #[serde(default)]
    pub headers: Vec<String>,
    #[serde(default)]
    pub libs: Vec<String>,
}

// Schema-first: rv validates these tables today; rv build consumes the
// values in W1 (target compile/link flags, vendored provenance).
#[allow(dead_code)]
#[derive(Debug, Deserialize, Default)]
#[serde(deny_unknown_fields)]
pub struct Flags {
    #[serde(default)]
    pub cflags: Vec<String>,
    #[serde(default)]
    pub ldflags: Vec<String>,
}

#[allow(dead_code)]
#[derive(Debug, Deserialize, Default)]
#[serde(deny_unknown_fields)]
pub struct Vendored {
    #[serde(default)]
    pub upstream: String,
    #[serde(default)]
    pub tag: String,
    #[serde(default)]
    pub license: String,
    #[serde(default)]
    pub patches: String,
}

pub fn load_all(tree: &Path) -> BTreeMap<String, Manifest> {
    let mut found = BTreeMap::new();
    collect(tree, tree, &mut found);
    found
}

pub fn parse(text: &str) -> Result<Manifest, String> {
    let m: Manifest = toml::from_str(text).map_err(|e| schema_error(&e.to_string()))?;
    validate(&m)?;
    Ok(m)
}

fn schema_error(detail: &str) -> String {
    if detail.contains("unknown field") {
        format!("E-UNKNOWN-KEY: {detail}")
    } else {
        format!("E-SCHEMA: {detail}")
    }
}

fn is_safe_component(name: &str) -> bool {
    name != "."
        && name != ".."
        && !name.starts_with('.')
        && !name.contains('/')
        && !name.contains('\\')
        && !name.contains('\0')
}

pub fn validate(m: &Manifest) -> Result<(), String> {
    if m.name.is_empty() {
        return Err(String::from("E-SCHEMA: name must not be empty"));
    }
    if !is_safe_component(&m.name) {
        return Err(format!(
            "E-SCHEMA: name '{}' must be a single path component (no '/', '\\', '..' or leading '.')",
            m.name
        ));
    }
    if !crate::graph::is_layer(&m.layer) {
        return Err(format!(
            "E-SCHEMA: unknown layer '{}' (SYSTEM-DESIGN S01 layer table)",
            m.layer
        ));
    }
    if !KINDS.contains(&m.kind.as_str()) {
        return Err(format!(
            "E-SCHEMA: unknown kind '{}' (SYSTEM-DESIGN S01 kind list)",
            m.kind
        ));
    }
    if !STATES.contains(&m.state.as_str()) {
        return Err(format!(
            "E-SCHEMA: state '{}' must be one of: {}",
            m.state,
            STATES.join(", ")
        ));
    }
    for t in &m.targets {
        if !TARGETS.contains(&t.as_str()) {
            return Err(format!("E-SCHEMA: unknown target '{t}' in targets"));
        }
    }
    for t in m.flags.keys() {
        if !TARGETS.contains(&t.as_str()) {
            return Err(format!("E-SCHEMA: unknown target '{t}' in flags"));
        }
    }
    if !m.deps.system.is_empty() && m.kind != "host-tool" {
        return Err(format!(
            "E-SCHEMA: component '{}' uses deps.system but kind is '{}'; system libraries are host-tool only",
            m.name, m.kind
        ));
    }
    if !m.hook.is_empty() && !m.sources.is_empty() {
        return Err(format!(
            "E-SCHEMA: component '{}' sets hook '{}' but also lists sources; a hooked component declares none",
            m.name, m.hook
        ));
    }
    if !SIGNS.contains(&m.sign.as_str()) && !m.sign.is_empty() {
        return Err(format!(
            "E-SCHEMA: sign '{}' must be 'adhoc' or 'none'",
            m.sign
        ));
    }
    check_rel(&m.source_root, "source_root")?;
    check_rel(&m.vendored.patches, "vendored.patches")?;
    Ok(())
}

fn check_rel(p: &str, what: &str) -> Result<(), String> {
    if p.is_empty() {
        return Ok(());
    }
    let path = Path::new(p);
    if path.is_absolute() {
        return Err(format!(
            "E-SCHEMA: {what} must be a tree-relative path, got '{p}'"
        ));
    }
    if path.components().any(|c| c == Component::ParentDir) {
        return Err(format!("E-SCHEMA: {what} must not contain '..'"));
    }
    Ok(())
}

fn collect(root: &Path, dir: &Path, found: &mut BTreeMap<String, Manifest>) {
    let Ok(entries) = std::fs::read_dir(dir) else {
        return;
    };
    for entry in entries.flatten() {
        let path = entry.path();
        // Discovery never follows a symbolic link: a link could point outside the
        // tree and pull a foreign manifest into the graph (BUG-27).
        let Ok(meta) = std::fs::symlink_metadata(&path) else {
            continue;
        };
        if meta.file_type().is_symlink() || !meta.is_dir() {
            continue;
        }
        let name = path.file_name().unwrap().to_string_lossy();
        if name == "."
            || name == ".."
            || name.starts_with('.')
            || name == "build"
            || name == "target"
        {
            continue;
        }
        if dir == root.join("Tests") && name == "rv" {
            // Harness fixtures are separate rv roots driven with --root; their
            // manifests never join the main graph.
            continue;
        }
        let manifest_path = path.join("manifest.toml");
        if manifest_path.is_file() {
            let text = std::fs::read_to_string(&manifest_path).unwrap_or_else(|e| {
                eprintln!("error: cannot read {}: {e}", manifest_path.display());
                std::process::exit(1);
            });
            match parse(&text) {
                Ok(mut m) => {
                    m.dir = path
                        .strip_prefix(root)
                        .expect("manifest under tree root")
                        .to_path_buf();
                    if let Some(prev) = found.get(&m.name) {
                        eprintln!(
                            "error: E-DUP-NAME: manifests {} and {} are both named '{}'",
                            prev.dir.display(),
                            m.dir.display(),
                            m.name
                        );
                        std::process::exit(1);
                    }
                    found.insert(m.name.clone(), m);
                }
                Err(e) => {
                    eprintln!("error: bad manifest {}: {e}", manifest_path.display());
                    std::process::exit(1);
                }
            }
        }
        collect(root, &path, found);
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    const GOOD: &str = r#"
name = "libdispatch"
layer = "foundation"
license = "Apache-2.0"
state = "working"
kind = "dylib"
sources = ["src/**.c", "src/**.s"]
source_root = ""
install_name = "/usr/lib/system/libdispatch.dylib"
install_path = "/usr/lib/system"
current_version = "1.0.0"
compatibility_version = "1.0.0"
reexports = []
umbrella = "System"
targets = ["x86_64-ravynos", "arm64-ravynos"]
hook = ""
cargo = ""

[exports]
headers = ["dispatch/**.h", "os/**.h"]
libs = ["libdispatch.dylib"]

[deps]
internal = ["libsystem_kernel"]
upward = []
system = []

[flags.x86_64-ravynos]
cflags = ["-O2"]
ldflags = []

[vendored]
upstream = "https://github.com/apple-oss-distributions/libdispatch"
tag = "libdispatch-1.0"
license = "Apache-2.0"
patches = "Foundation/libdispatch/patches"
"#;

    #[test]
    fn full_contract_manifest_parses() {
        let m = parse(GOOD).expect("contract manifest must parse");
        assert_eq!(m.name, "libdispatch");
        assert_eq!(m.kind, "dylib");
        assert_eq!(m.state, "working");
        assert_eq!(m.install_name, "/usr/lib/system/libdispatch.dylib");
        assert_eq!(m.current_version, "1.0.0");
        assert_eq!(m.exports.libs, vec!["libdispatch.dylib"]);
        assert_eq!(m.deps.internal, vec!["libsystem_kernel"]);
        assert_eq!(m.flags["x86_64-ravynos"].cflags, vec!["-O2".to_string()]);
        assert_eq!(m.vendored.tag, "libdispatch-1.0");
    }

    #[test]
    fn bundle_keys_parse() {
        let text = r#"
name = "Kit"
layer = "frameworks"
license = "BSD-2-Clause"
state = "incomplete"
kind = "framework"
sources = []
info_plist = "Info.plist"
bundle_id = "org.ravynos.Kit"
resources = ["Resources/**"]
entitlements = "Kit.entitlements"
sign = "adhoc"
embed = ["KitHelper"]
framework_version = "C"
"#;
        let m = parse(text).expect("bundle manifest must parse");
        assert_eq!(m.bundle_id, "org.ravynos.Kit");
        assert_eq!(m.framework_version, "C");
        assert_eq!(m.embed, vec!["KitHelper"]);
    }

    #[test]
    fn minimal_manifest_parses() {
        let text = r#"
name = "tiny"
layer = "foundation"
license = "MIT"
state = "experimental"
kind = "data"
sources = []
"#;
        parse(text).expect("minimal manifest must parse");
    }

    #[test]
    fn unknown_top_level_key_is_rejected() {
        let text = GOOD.replace(
            "umbrella = \"System\"",
            "umbrella = \"System\"\nfrobnicate = 1",
        );
        let err = parse(&text).unwrap_err();
        assert!(err.starts_with("E-UNKNOWN-KEY"), "got: {err}");
        assert!(err.contains("frobnicate"), "got: {err}");
    }

    #[test]
    fn unknown_nested_key_is_rejected() {
        let text = GOOD.replace("[deps]\n", "[deps]\nexternal = []\n");
        let err = parse(&text).unwrap_err();
        assert!(err.starts_with("E-UNKNOWN-KEY"), "got: {err}");
    }

    #[test]
    fn unknown_export_and_flag_keys_are_rejected() {
        let text = GOOD.replace("[exports]\n", "[exports]\nmodules = []\n");
        assert!(parse(&text).unwrap_err().starts_with("E-UNKNOWN-KEY"));
        let text = GOOD.replace("[vendored]\n", "[vendored]\nbranch = \"main\"\n");
        assert!(parse(&text).unwrap_err().starts_with("E-UNKNOWN-KEY"));
        let text = GOOD.replace(
            "[flags.x86_64-ravynos]\n",
            "[flags.x86_64-ravynos]\nasflags = []\n",
        );
        assert!(parse(&text).unwrap_err().starts_with("E-UNKNOWN-KEY"));
    }

    #[test]
    fn name_with_a_separator_is_rejected() {
        let text = GOOD.replace("name = \"libdispatch\"", "name = \"../../x\"");
        let err = parse(&text).unwrap_err();
        assert!(err.contains("single path component"), "got: {err}");
    }

    #[test]
    fn name_that_is_a_parent_is_rejected() {
        let text = GOOD.replace("name = \"libdispatch\"", "name = \"..\"");
        assert!(parse(&text).unwrap_err().contains("single path component"));
    }

    #[test]
    fn name_with_a_leading_dot_is_rejected() {
        let text = GOOD.replace("name = \"libdispatch\"", "name = \".hidden\"");
        assert!(parse(&text).unwrap_err().contains("single path component"));
    }

    #[test]
    fn unknown_kind_is_rejected() {
        let text = GOOD.replace("kind = \"dylib\"", "kind = \"widget\"");
        let err = parse(&text).unwrap_err();
        assert!(err.contains("unknown kind 'widget'"), "got: {err}");
    }

    #[test]
    fn unknown_layer_is_rejected() {
        let text = GOOD.replace("layer = \"foundation\"", "layer = \"middleware\"");
        let err = parse(&text).unwrap_err();
        assert!(err.contains("unknown layer 'middleware'"), "got: {err}");
    }

    #[test]
    fn unknown_state_is_rejected() {
        let text = GOOD.replace("state = \"working\"", "state = \"wip\"");
        let err = parse(&text).unwrap_err();
        assert!(err.contains("state 'wip'"), "got: {err}");
    }

    #[test]
    fn unknown_target_in_targets_is_rejected() {
        let text = GOOD.replace(
            "targets = [\"x86_64-ravynos\", \"arm64-ravynos\"]",
            "targets = [\"x86_64-linux-gnu\"]",
        );
        let err = parse(&text).unwrap_err();
        assert!(
            err.contains("unknown target 'x86_64-linux-gnu'"),
            "got: {err}"
        );
    }

    #[test]
    fn unknown_target_in_flags_is_rejected() {
        let text = GOOD.replace("[flags.x86_64-ravynos]", "[flags.mips-ravynos]");
        let err = parse(&text).unwrap_err();
        assert!(err.contains("unknown target 'mips-ravynos'"), "got: {err}");
    }

    #[test]
    fn system_deps_are_host_tool_only() {
        let text = GOOD.replace("system = []", "system = [\"libz\"]");
        let err = parse(&text).unwrap_err();
        assert!(err.contains("deps.system"), "got: {err}");
        let ok = GOOD
            .replace("kind = \"dylib\"", "kind = \"host-tool\"")
            .replace("system = []", "system = [\"libz\"]");
        parse(&ok).expect("host-tool may use deps.system");
    }

    #[test]
    fn hook_forbids_sources() {
        let text = GOOD.replace("hook = \"\"", "hook = \"Tools/gen-syscall-tables\"");
        let err = parse(&text).unwrap_err();
        assert!(err.contains("hook"), "got: {err}");
    }

    #[test]
    fn sign_is_adhoc_or_none() {
        let text = GOOD.replace("umbrella = \"System\"", "sign = \"hardened\"");
        let err = parse(&text).unwrap_err();
        assert!(err.contains("sign 'hardened'"), "got: {err}");
    }

    #[test]
    fn source_root_must_stay_inside_the_tree() {
        let text = GOOD.replace("source_root = \"\"", "source_root = \"/etc\"");
        let err = parse(&text).unwrap_err();
        assert!(err.contains("source_root"), "got: {err}");
        let text = GOOD.replace("source_root = \"\"", "source_root = \"../escape\"");
        let err = parse(&text).unwrap_err();
        assert!(err.contains("source_root"), "got: {err}");
        let text = GOOD.replace(
            "source_root = \"\"",
            "source_root = \"Kernel/xnu/libsyscall\"",
        );
        parse(&text).expect("external tree-relative source_root is accepted");
    }

    #[test]
    fn empty_name_is_rejected() {
        let text = GOOD.replace("name = \"libdispatch\"", "name = \"\"");
        let err = parse(&text).unwrap_err();
        assert!(err.contains("name must not be empty"), "got: {err}");
    }

    #[test]
    fn missing_required_key_is_a_schema_error() {
        let text = GOOD.replace("state = \"working\"", "");
        let err = parse(&text).unwrap_err();
        assert!(err.starts_with("E-SCHEMA"), "got: {err}");
    }

    #[test]
    fn every_kind_is_accepted_for_a_minimal_manifest() {
        for kind in KINDS {
            let text = format!(
                "name = \"k\"\nlayer = \"foundation\"\nlicense = \"MIT\"\nstate = \"experimental\"\nkind = \"{kind}\"\nsources = []\n"
            );
            parse(&text).unwrap_or_else(|e| panic!("kind {kind} must be valid: {e}"));
        }
    }

    #[test]
    fn every_target_is_accepted() {
        for t in TARGETS {
            let text = format!(
                "name = \"t\"\nlayer = \"foundation\"\nlicense = \"MIT\"\nstate = \"experimental\"\nkind = \"data\"\nsources = []\ntargets = [\"{t}\"]\n"
            );
            parse(&text).unwrap_or_else(|e| panic!("target {t} must be valid: {e}"));
        }
    }
}
