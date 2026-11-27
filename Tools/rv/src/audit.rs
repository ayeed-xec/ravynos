// SPDX-License-Identifier: BSD-3-Clause
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

pub fn license(tree: &Path, manifests: &BTreeMap<String, Manifest>) {
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
        let full = tree.join(&m.dir);
        walk(&full, &full, &mut failures);
    }
    if !failures.is_empty() {
        for f in &failures {
            eprintln!("error: license audit: {f}");
        }
        eprintln!(
            "       (Docs/design/05-build-system.md license audit gate; accepted: {})",
            ACCEPT.join(", ")
        );
        std::process::exit(1);
    }
}

fn walk(dir: &Path, root: &Path, failures: &mut Vec<String>) {
    let Ok(entries) = std::fs::read_dir(dir) else {
        return;
    };
    for entry in entries.flatten() {
        let path = entry.path();
        let name = path.file_name().unwrap().to_string_lossy().to_string();
        if name.starts_with('.') || name == "build" {
            continue;
        }
        if path.is_dir() {
            walk(&path, root, failures);
        } else if is_source(&name) {
            let rel = path
                .strip_prefix(root)
                .unwrap_or(&path)
                .display()
                .to_string();
            let Ok(text) = std::fs::read_to_string(&path) else {
                failures.push(format!("{rel}: unreadable as UTF-8"));
                continue;
            };
            check(&rel, &text, failures);
        }
    }
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
