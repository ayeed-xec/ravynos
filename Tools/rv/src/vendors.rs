// SPDX-License-Identifier: BSD-2-Clause
// Copyright (c) 2026 The ravynOS Authors
use serde::Deserialize;
use std::path::Path;
use std::process::Command;

#[derive(Deserialize)]
pub struct Vendors {
    #[serde(default)]
    vendor: Vec<Vendor>,
}

#[derive(Deserialize)]
pub struct Vendor {
    name: String,
    path: String,
    #[serde(default)]
    tag: String,
    #[serde(default)]
    commit: String,
    #[serde(default)]
    tree: String,
    #[serde(default)]
    patches: String,
    #[serde(default)]
    subdirs: Vec<String>,
    #[serde(default)]
    prune: Vec<String>,
    #[serde(default)]
    fetch: bool,
}

pub fn check(tree_root: &Path) -> Result<(), String> {
    let text = std::fs::read_to_string(tree_root.join("VENDORS"))
        .map_err(|e| format!("error: cannot read VENDORS: {e}"))?;
    let parsed = parse(&text).map_err(|e| format!("error: VENDORS: {e}"))?;

    let mut failures = Vec::new();
    let mut checked = 0;
    for v in &parsed.vendor {
        if v.tag.is_empty() {
            failures.push(format!("{}: no tag", v.name));
        }
        if !looks_like_sha(&v.commit) {
            failures.push(format!(
                "{}: commit '{}' is not a 40-hex id",
                v.name, v.commit
            ));
        }
        if v.fetch {
            continue;
        }
        failures.extend(prune_failures(tree_root, v));
        if !looks_like_sha(&v.tree) {
            failures.push(format!("{}: tree '{}' is not a 40-hex id", v.name, v.tree));
            continue;
        }
        if !v.patches.is_empty() && !tree_root.join(&v.patches).is_dir() {
            failures.push(format!(
                "{}: patches directory '{}' does not exist",
                v.name, v.patches
            ));
        }
        let spec = match v.subdirs.as_slice() {
            [] => format!("HEAD:{}", v.path),
            [sub] => format!("HEAD:{}/{}", v.path, sub),
            _ => {
                failures.push(format!(
                    "{}: subdirs lists more than one path, but tree is a single id",
                    v.name
                ));
                continue;
            }
        };
        match git_tree(tree_root, &spec) {
            Some(got) if got == v.tree => checked += 1,
            Some(got) => failures.push(format!(
                "{}: {spec} is {got}, pinned {} for {}",
                v.name, v.tree, v.tag
            )),
            None => failures.push(format!("{}: cannot read {spec} from git", v.name)),
        }
    }

    if failures.is_empty() {
        println!("vendors gate OK: {checked} vendored trees match their pins");
        return Ok(());
    }
    let mut lines: Vec<String> = failures
        .iter()
        .map(|f| format!("error: vendors check: {f}"))
        .collect();
    lines.push(
        "       (SYSTEM-DESIGN.md S01 / ADR-10: vendored trees stay pristine; \
         ADR-17: generated files stay pruned)"
            .to_string(),
    );
    Err(lines.join("\n"))
}

/// Prefixes of the trees pinned in VENDORS, for callers that must skip them.
pub fn paths(tree_root: &Path) -> Result<Vec<String>, String> {
    let text = std::fs::read_to_string(tree_root.join("VENDORS"))
        .map_err(|e| format!("cannot read VENDORS: {e}"))?;
    let parsed = parse(&text)?;
    Ok(parsed.vendor.into_iter().map(|v| v.path).collect())
}

fn parse(text: &str) -> Result<Vendors, String> {
    let vendors: Vendors = toml::from_str(text).map_err(|e| e.to_string())?;
    if vendors.vendor.is_empty() {
        return Err("no [[vendor]] entries".to_string());
    }
    Ok(vendors)
}

fn looks_like_sha(id: &str) -> bool {
    id.len() == 40 && id.chars().all(|c| c.is_ascii_hexdigit())
}

fn prune_failures(tree_root: &Path, v: &Vendor) -> Vec<String> {
    let mut failures = Vec::new();
    for rel in &v.prune {
        if rel.is_empty() || rel.starts_with('/') || rel.split('/').any(|c| c == "..") {
            failures.push(format!(
                "{}: prune path '{rel}' must stay inside '{}'",
                v.name, v.path
            ));
        } else if tree_root.join(&v.path).join(rel).exists() {
            failures.push(format!(
                "{}: prune path '{rel}' reappeared; generated GPL-derived output must be deleted (ADR-17)",
                v.name
            ));
        }
    }
    failures
}

fn git_tree(tree_root: &Path, spec: &str) -> Option<String> {
    let out = Command::new("git")
        .arg("-C")
        .arg(tree_root)
        .args(["rev-parse", spec])
        .output()
        .ok()?;
    if !out.status.success() {
        return None;
    }
    let id = String::from_utf8_lossy(&out.stdout).trim().to_string();
    looks_like_sha(&id).then_some(id)
}

#[cfg(test)]
mod tests {
    use super::*;

    const MINIMAL: &str = r#"
[[vendor]]
name   = "xnu"
path   = "Kernel/xnu"
tag    = "xnu-1"
commit = "ac9718fb1af618d5ce8678d0dc6e8a58f252216f"
tree   = "cba06fba7c9d041633c55cfa3e3130fc367a5a1b"
"#;

    #[test]
    fn minimal_entry_parses() {
        let v = parse(MINIMAL).unwrap();
        assert_eq!(v.vendor.len(), 1);
        assert_eq!(v.vendor[0].name, "xnu");
        assert!(!v.vendor[0].fetch);
    }

    #[test]
    fn entry_without_path_is_a_schema_error() {
        assert!(parse("[[vendor]]\nname = \"x\"\n").is_err());
    }

    #[test]
    fn empty_file_is_an_error() {
        assert!(parse("").is_err());
    }

    #[test]
    fn commit_and_tree_must_be_full_ids() {
        assert!(looks_like_sha("ac9718fb1af618d5ce8678d0dc6e8a58f252216f"));
        assert!(!looks_like_sha("ac9718f"));
        assert!(!looks_like_sha("zz9718fb1af618d5ce8678d0dc6e8a58f252216f"));
    }

    fn migcom(prune: Vec<String>) -> Vendor {
        Vendor {
            name: "migcom".to_string(),
            path: "Tools/mig/upstream".to_string(),
            tag: "bootstrap_cmds-138".to_string(),
            commit: "c71d2d72f48995baaea76148f61002e5299841de".to_string(),
            tree: "7ab1533c050d5104f0ad64956a03bd640464b160".to_string(),
            patches: "Tools/mig/patches".to_string(),
            subdirs: vec!["migcom.tproj".to_string()],
            prune,
            fetch: false,
        }
    }

    #[test]
    fn prune_list_parses() {
        let text = r#"
[[vendor]]
name    = "migcom"
path    = "Tools/mig/upstream"
tag     = "bootstrap_cmds-138"
commit  = "c71d2d72f48995baaea76148f61002e5299841de"
tree    = "7ab1533c050d5104f0ad64956a03bd640464b160"
subdirs = ["migcom.tproj"]
prune   = ["migcom.tproj/parser.c", "migcom.tproj/lexxer.c"]
"#;
        let v = parse(text).unwrap();
        assert_eq!(
            v.vendor[0].prune,
            ["migcom.tproj/parser.c", "migcom.tproj/lexxer.c"]
        );
        assert!(parse(MINIMAL).unwrap().vendor[0].prune.is_empty());
    }

    #[test]
    fn pruned_path_must_stay_absent() {
        let root = std::env::temp_dir().join(format!("rv-vendors-prune-{}", std::process::id()));
        let _ = std::fs::remove_dir_all(&root);
        let dir = root.join("Tools/mig/upstream/migcom.tproj");
        std::fs::create_dir_all(&dir).unwrap();
        let rel = "migcom.tproj/parser.c".to_string();

        assert!(prune_failures(&root, &migcom(vec![rel.clone()])).is_empty());

        std::fs::write(dir.join("parser.c"), "generated").unwrap();
        let failures = prune_failures(&root, &migcom(vec![rel]));
        assert_eq!(failures.len(), 1);
        assert!(failures[0].contains("reappeared"), "{failures:?}");

        let _ = std::fs::remove_dir_all(&root);
    }

    #[test]
    fn prune_path_cannot_escape_the_vendored_tree() {
        let failures = prune_failures(Path::new("/nonexistent"), &migcom(vec!["../x".to_string()]));
        assert_eq!(failures.len(), 1);
        assert!(failures[0].contains("must stay inside"), "{failures:?}");
    }
}
