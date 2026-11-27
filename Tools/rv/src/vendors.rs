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
    fetch: bool,
}

pub fn check(tree_root: &Path) {
    let text = std::fs::read_to_string(tree_root.join("VENDORS")).unwrap_or_else(|e| {
        eprintln!("error: cannot read VENDORS: {e}");
        std::process::exit(1);
    });
    let parsed = parse(&text).unwrap_or_else(|e| {
        eprintln!("error: VENDORS: {e}");
        std::process::exit(1);
    });

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

    if !failures.is_empty() {
        for f in &failures {
            eprintln!("error: vendors check: {f}");
        }
        eprintln!("       (SYSTEM-DESIGN.md S01 / ADR-10: vendored trees stay pristine)");
        std::process::exit(1);
    }
    println!("vendors gate OK: {checked} vendored trees match their pins");
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
}
