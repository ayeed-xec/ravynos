// SPDX-License-Identifier: BSD-2-Clause
// Copyright (c) 2026 The ravynOS Authors
use serde::Deserialize;
use std::collections::BTreeMap;
use std::path::Path;

#[derive(Deserialize)]
pub struct Manifest {
    pub name: String,
    pub layer: String,
    pub license: String,
    pub sources: Vec<String>,
    #[serde(default)]
    pub exports: Exports,
    #[serde(default)]
    pub deps: Deps,
    #[serde(default)]
    pub targets: Vec<String>,
    #[serde(skip)]
    pub dir: std::path::PathBuf,
}

#[derive(Deserialize, Default, Clone)]
pub struct Deps {
    #[serde(default)]
    pub internal: Vec<String>,
}

#[derive(Deserialize, Default)]
pub struct Exports {
    #[serde(default)]
    pub headers: Vec<String>,
    #[serde(default)]
    pub libs: Vec<String>,
}

pub fn load_all(tree: &Path) -> BTreeMap<String, Manifest> {
    let mut found = BTreeMap::new();
    collect(tree, tree, &mut found);
    found
}

fn collect(root: &Path, dir: &Path, found: &mut BTreeMap<String, Manifest>) {
    let Ok(entries) = std::fs::read_dir(dir) else {
        return;
    };
    for entry in entries.flatten() {
        let path = entry.path();
        if !path.is_dir() {
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
        let manifest_path = path.join("manifest.toml");
        if manifest_path.is_file() {
            let text = std::fs::read_to_string(&manifest_path).unwrap_or_else(|e| {
                eprintln!("error: cannot read {}: {e}", manifest_path.display());
                std::process::exit(1);
            });
            match toml::from_str::<Manifest>(&text) {
                Ok(mut m) => {
                    m.dir = path
                        .strip_prefix(root)
                        .expect("manifest under tree root")
                        .to_path_buf();
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
