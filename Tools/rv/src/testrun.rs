// SPDX-License-Identifier: BSD-2-Clause
// Copyright (c) 2026 The ravynOS Authors
use serde::Deserialize;
use std::path::{Path, PathBuf};
use std::process::Command;

/// One `test.toml` (S01, S09). A file without `kind` is a schema fixture rather
/// than a test, so discovery keeps it but `rv test` does not run it.
#[derive(Debug, Deserialize)]
pub struct Spec {
    pub name: String,
    #[serde(default)]
    kind: String,
    #[serde(default)]
    run: String,
    #[serde(default)]
    expect: String,
}

impl Spec {
    fn runs() -> &'static [&'static str] {
        &["script", "host"]
    }
}

struct Found {
    dir: PathBuf,
    spec: Spec,
}

pub fn run(tree: &Path, names: &[String]) -> Result<(), String> {
    let found = discover(&tree.join("Tests"))?;
    if found.is_empty() {
        return Err(String::from("error: E-NO-TEST: no test.toml under Tests/"));
    }
    let selected: Vec<&Found> = if names.is_empty() {
        found.iter().collect()
    } else {
        let mut picked = Vec::new();
        for name in names {
            let hit = found.iter().find(|f| f.spec.name == *name);
            match hit {
                Some(f) => picked.push(f),
                None => return Err(format!("error: E-NO-TEST: no test named '{name}'")),
            }
        }
        picked
    };
    let mut passed = 0;
    let mut failed = 0;
    let mut skipped = 0;
    for f in selected {
        match f.spec.kind.as_str() {
            "" => skipped += 1,
            kind if Spec::runs().contains(&kind) => match execute(&f.dir, &f.spec) {
                Ok(()) => {
                    passed += 1;
                    println!("ok   {}", f.spec.name);
                }
                Err(message) => {
                    failed += 1;
                    println!("FAIL {}: {message}", f.spec.name);
                }
            },
            "qemu" => {
                skipped += 1;
                println!("skip {}: E-NOT-BUILT S09 (needs an image)", f.spec.name);
            }
            other => {
                return Err(format!(
                    "error: E-TEST-KIND: {}: unknown kind '{other}'",
                    f.spec.name
                ))
            }
        }
    }
    println!("test: {passed} passed, {failed} failed, {skipped} skipped");
    if failed > 0 {
        return Err(format!("error: E-TEST-FAIL: {failed} test(s) failed"));
    }
    Ok(())
}

fn execute(dir: &Path, spec: &Spec) -> Result<(), String> {
    if spec.run.is_empty() {
        return Err(String::from("error: E-SCHEMA: missing `run`"));
    }
    let out = Command::new("sh")
        .arg("-c")
        .arg(&spec.run)
        .current_dir(dir)
        .output()
        .map_err(|e| format!("error: E-TEST-RUN: {e}"))?;
    let mut text = String::from_utf8_lossy(&out.stdout).into_owned();
    text.push_str(&String::from_utf8_lossy(&out.stderr));
    if !spec.expect.is_empty() && !text.contains(&spec.expect) {
        return Err(format!(
            "error: E-TEST-EXPECT: output lacks {:?}",
            spec.expect
        ));
    }
    if !out.status.success() {
        return Err(format!("error: E-TEST-STATUS: exited {}", out.status));
    }
    Ok(())
}

fn discover(dir: &Path) -> Result<Vec<Found>, String> {
    let mut found = Vec::new();
    walk(dir, &mut found)?;
    found.sort_by(|a, b| a.spec.name.cmp(&b.spec.name));
    Ok(found)
}

fn walk(dir: &Path, found: &mut Vec<Found>) -> Result<(), String> {
    let Ok(entries) = std::fs::read_dir(dir) else {
        return Ok(());
    };
    for entry in entries.flatten() {
        let path = entry.path();
        if path.is_dir() {
            walk(&path, found)?;
        } else if path.file_name().map(|n| n == "test.toml").unwrap_or(false) {
            let text = std::fs::read_to_string(&path)
                .map_err(|e| format!("error: read {}: {e}", path.display()))?;
            let spec: Spec = toml::from_str(&text)
                .map_err(|e| format!("error: E-SCHEMA: {}: {e}", path.display()))?;
            found.push(Found {
                dir: path.parent().unwrap_or(dir).to_path_buf(),
                spec,
            });
        }
    }
    Ok(())
}

#[cfg(test)]
mod tests {
    use super::*;

    fn tree(label: &str, body: &str) -> PathBuf {
        let dir = std::env::temp_dir().join(format!("rv-test-{}-{label}", std::process::id()));
        std::fs::create_dir_all(dir.join("Tests/demo")).unwrap();
        std::fs::write(dir.join("Tests/demo/test.toml"), body).unwrap();
        dir
    }

    #[test]
    fn a_passing_script_test_reports_ok() {
        let dir = tree(
            "pass",
            "name = \"demo\"\nkind = \"script\"\nrun = \"echo hello\"\nexpect = \"hello\"\n",
        );
        assert!(run(&dir, &[]).is_ok());
        std::fs::remove_dir_all(&dir).ok();
    }

    #[test]
    fn missing_expected_output_fails_the_test() {
        let dir = tree(
            "expect",
            "name = \"demo\"\nkind = \"script\"\nrun = \"echo hello\"\nexpect = \"goodbye\"\n",
        );
        let err = run(&dir, &[]).unwrap_err();
        assert!(err.contains("E-TEST-FAIL"), "{err}");
        std::fs::remove_dir_all(&dir).ok();
    }

    #[test]
    fn a_nonzero_exit_fails_the_test() {
        let dir = tree(
            "status",
            "name = \"demo\"\nkind = \"script\"\nrun = \"exit 4\"\n",
        );
        assert!(run(&dir, &[]).is_err());
        std::fs::remove_dir_all(&dir).ok();
    }

    #[test]
    fn an_unknown_name_is_an_error() {
        let dir = tree(
            "name",
            "name = \"demo\"\nkind = \"script\"\nrun = \"true\"\n",
        );
        let err = run(&dir, &[String::from("other")]).unwrap_err();
        assert!(err.contains("E-NO-TEST"), "{err}");
        assert!(run(&dir, &[String::from("demo")]).is_ok());
        std::fs::remove_dir_all(&dir).ok();
    }

    #[test]
    fn a_schema_fixture_without_kind_is_skipped() {
        let dir = tree("fixture", "name = \"demo\"\nmatch = \"liba -> libb\"\n");
        assert!(run(&dir, &[]).is_ok());
        std::fs::remove_dir_all(&dir).ok();
    }

    #[test]
    fn a_qemu_test_is_skipped_not_failed() {
        let dir = tree(
            "qemu",
            "name = \"demo\"\nkind = \"qemu\"\nrun = \"qemu-system-x86_64\"\n",
        );
        assert!(run(&dir, &[]).is_ok());
        std::fs::remove_dir_all(&dir).ok();
    }
}
