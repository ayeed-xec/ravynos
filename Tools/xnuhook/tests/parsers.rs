// SPDX-License-Identifier: BSD-2-Clause
// Copyright (c) 2026 The ravynOS Authors

//! Runs xnuhook on the vendored `Kernel/xnu`, then executes the plan's
//! generate edges with a real byacc, the way rv will. byacc comes from
//! `BYACC` or from PATH (the stage-1 build tools); without one the test fails
//! rather than skipping.

use std::path::{Path, PathBuf};
use std::process::Command;

fn root() -> PathBuf {
    Path::new(env!("CARGO_MANIFEST_DIR")).join("../..")
}

fn byacc() -> PathBuf {
    if let Some(path) = std::env::var_os("BYACC") {
        return PathBuf::from(path);
    }
    std::env::var_os("PATH")
        .and_then(|paths| {
            std::env::split_paths(&paths)
                .map(|dir| dir.join("byacc"))
                .find(|p| p.is_file())
        })
        .expect("byacc not found: build the stage-1 tools (rv bootstrap buildtools) and put their bin on PATH")
}

fn scratch(name: &str) -> PathBuf {
    let dir = std::env::temp_dir().join("xnuhook-it").join(name);
    let _ = std::fs::remove_dir_all(&dir);
    std::fs::create_dir_all(&dir).unwrap();
    dir
}

fn request(dir: &Path, src: &Path, extra: &str) -> PathBuf {
    let req = dir.join("request.json");
    let text = format!(
        r#"{{"version": 1, "component": "xnu", "target": "x86_64-ravynos",
            "config": "RELEASE", "board": "none", "src": "{}", "out": "{}",
            "tools": {{"byacc": "{}"}}{extra}}}"#,
        src.display(),
        dir.join("out").display(),
        byacc().display()
    );
    std::fs::write(&req, text).unwrap();
    req
}

fn hook(req: &Path) -> std::process::Output {
    Command::new(env!("CARGO_BIN_EXE_xnuhook"))
        .arg("--request")
        .arg(req)
        .output()
        .unwrap()
}

#[test]
fn plan_regenerates_both_parsers_with_byacc() {
    let dir = scratch("regen");
    let req = request(&dir, &root().join("Kernel/xnu"), "");
    let out = hook(&req);
    assert!(
        out.status.success(),
        "{}",
        String::from_utf8_lossy(&out.stderr)
    );

    let plan: serde_json::Value =
        serde_json::from_slice(&std::fs::read(dir.join("out/plan.json")).unwrap()).unwrap();
    assert_eq!(plan["version"], 1);
    let edges = plan["generate"].as_array().unwrap();
    assert_eq!(edges.len(), 2);
    for edge in edges {
        let argv: Vec<&str> = edge["argv"]
            .as_array()
            .unwrap()
            .iter()
            .map(|a| a.as_str().unwrap())
            .collect();
        // ninja creates an edge's output directories; so does this executor.
        for output in edge["outputs"].as_array().unwrap() {
            let parent = Path::new(output.as_str().unwrap()).parent().unwrap();
            std::fs::create_dir_all(parent).unwrap();
        }
        let output = Command::new(argv[0]).args(&argv[1..]).output().unwrap();
        assert!(
            output.status.success(),
            "{argv:?}: {}",
            String::from_utf8_lossy(&output.stderr)
        );
    }

    let parsers = dir.join("out/parsers");
    let classic = std::fs::read_to_string(parsers.join("OSUnserialize.cpp")).unwrap();
    let xml =
        std::fs::read_to_string(parsers.join("OSUnserializeXMLSharedImplementation.h")).unwrap();
    for text in [&classic, &xml] {
        assert!(text.contains("#define YYBYACC"), "not byacc output");
    }
    // Both parsers link into one kernel: their entry points must differ.
    assert!(classic.contains("#define yyparse    OSUnserializeparse"));
    assert!(xml.contains("#define yyparse    OSUnserializeXMLparse"));
    // Only the XML grammar is reentrant, and it takes its state as the
    // yyparse parameter the grammar defines.
    assert!(classic.contains("#define YYPURE 0"));
    assert!(xml.contains("#define YYPURE 1"));
    assert!(xml.contains("#define YYPARSE_PARAM   state"));
}

#[test]
fn same_request_same_plan() {
    let dir = scratch("determinism");
    let req = request(&dir, &root().join("Kernel/xnu"), "");
    assert!(hook(&req).status.success());
    let first = std::fs::read(dir.join("out/plan.json")).unwrap();
    assert!(hook(&req).status.success());
    assert_eq!(first, std::fs::read(dir.join("out/plan.json")).unwrap());
}

#[test]
fn unknown_request_key_is_rejected() {
    let dir = scratch("unknown-key");
    let req = request(&dir, &root().join("Kernel/xnu"), r#", "jobs": 8"#);
    let out = hook(&req);
    assert!(!out.status.success());
    assert!(String::from_utf8_lossy(&out.stderr).contains("E-HOOK-REQUEST"));
}

#[test]
fn bad_usage_exits_2() {
    let out = Command::new(env!("CARGO_BIN_EXE_xnuhook"))
        .arg("--plan")
        .output()
        .unwrap();
    assert_eq!(out.status.code(), Some(2));
}
