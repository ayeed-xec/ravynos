// SPDX-License-Identifier: BSD-2-Clause
// Copyright (c) 2026 The ravynOS Authors
use crate::manifest::Manifest;
use std::collections::BTreeMap;
use std::path::Path;

const ACCEPT: &[&str] = &[
    "APSL-2.0",
    "BSD",
    "BSD-2-Clause",
    "BSD-3-Clause",
    "MIT",
    "Apache-2.0",
    "Zlib",
    "ISC",
    "CC0-1.0",
    "Unlicense",
    "LicenseRef-PublicDomain",
];

const FAIL: &[&str] = &["GPL", "LGPL", "AGPL"];

pub fn license(tree: &Path, manifests: &BTreeMap<String, Manifest>) -> Result<(), String> {
    let mut failures = Vec::new();
    for m in manifests.values() {
        let expr: String = m
            .license
            .to_uppercase()
            .chars()
            .take_while(|c| c.is_ascii_alphanumeric() || *c == '-' || *c == '.')
            .collect();
        if FAIL.iter().any(|bad| expr.starts_with(bad)) {
            failures.push(format!(
                "{}: manifest license '{}' is prohibited",
                m.name, m.license
            ));
        } else if !ACCEPT.iter().any(|a| expr.starts_with(&a.to_uppercase())) {
            failures.push(format!(
                "{}: manifest license '{}' is not on the accept list",
                m.name, m.license
            ));
        }
    }
    let mut vendored =
        crate::vendors::paths(tree).map_err(|e| format!("error: license audit: {e}"))?;
    vendored.extend(BUILD_OUTPUTS.iter().map(|p| p.to_string()));
    walk(tree, tree, &vendored, &mut failures);
    if failures.is_empty() {
        return Ok(());
    }
    let mut lines: Vec<String> = failures
        .iter()
        .map(|f| format!("error: license audit: {f}"))
        .collect();
    lines.push(format!(
        "       (Docs/design/05-build-system.md license audit gate; accepted: {})",
        ACCEPT.join(", ")
    ));
    Err(lines.join("\n"))
}

/// Walks the whole tree, not only component directories: every file is either in a
/// tree pinned by `VENDORS` (its license comes from that entry) or carries its own
/// SPDX header (GAP-09, GAP-10).
fn walk(dir: &Path, root: &Path, vendored: &[String], failures: &mut Vec<String>) {
    let Ok(entries) = std::fs::read_dir(dir) else {
        return;
    };
    for entry in entries.flatten() {
        let path = entry.path();
        let Some(name) = path.file_name().map(|n| n.to_string_lossy().to_string()) else {
            continue;
        };
        if name.starts_with('.') || name == "build" || name == "target" {
            continue;
        }
        let rel = path
            .strip_prefix(root)
            .unwrap_or(&path)
            .display()
            .to_string();
        if is_vendored(&rel, vendored) {
            continue;
        }
        if path.is_dir() {
            walk(&path, root, vendored, failures);
        } else if is_source(&name) && !is_generated(&rel) {
            let Ok(text) = std::fs::read_to_string(&path) else {
                failures.push(format!("{rel}: unreadable as UTF-8"));
                continue;
            };
            check(&rel, &text, failures);
        }
    }
}

fn is_vendored(rel: &str, vendored: &[String]) -> bool {
    vendored
        .iter()
        .any(|v| rel == v || rel.starts_with(&format!("{v}/")))
}

/// Trees that are inputs to a tool's own tests: Apple-derived fixture headers the
/// mig harness compiles. They are never linked into a product, so their licence comes
/// from upstream, not from an SPDX line we would be inventing (GAP-11 tracks moving
/// them under `upstream/`).
const FIXTURES: &[&str] = &["Tools/mig/tests"];

/// Working copies and install prefixes the tool builds create; they are derived from
/// the vendored tree and never shipped.
const BUILD_OUTPUTS: &[&str] = &["Tools/mig/work", "Tools/mig/bin", "Tools/mig/libexec"];

fn is_generated(rel: &str) -> bool {
    rel.split('/').any(|c| c == "generated") || FIXTURES.iter().any(|f| rel.starts_with(f))
}

fn is_source(name: &str) -> bool {
    matches!(
        name.rsplit('.').next(),
        Some("c") | Some("h") | Some("cpp") | Some("hpp") | Some("rs") | Some("m") | Some("mm")
    ) && name != "Cargo.toml"
}

fn check(rel: &str, text: &str, failures: &mut Vec<String>) {
    let upper = text.to_uppercase();
    let needle = "SPDX-License-Identifier:".to_uppercase();
    for bad in FAIL {
        if upper.contains(&format!("{needle}{bad}")) {
            failures.push(format!("{rel}: {bad} license is prohibited"));
            return;
        }
    }
    if let Some(pos) = upper.find(&needle) {
        let rest = &upper[pos + needle.len()..];
        let expr: String = rest
            .chars()
            .skip_while(|c| !c.is_ascii_alphanumeric())
            .take_while(|c| c.is_ascii_alphanumeric() || *c == '-' || *c == '.')
            .collect();
        if !ACCEPT.iter().any(|a| expr.starts_with(&a.to_uppercase())) {
            failures.push(format!("{rel}: SPDX '{expr}' is not on the accept list"));
        }
        return;
    }
    failures.push(format!("{rel}: no SPDX-License-Identifier header"));
}
