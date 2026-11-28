// SPDX-License-Identifier: BSD-2-Clause
// Copyright (c) 2026 The ravynOS Authors

//! The `xnu` build hook (SYSTEM-DESIGN.md S05), following the S01 hook
//! contract, version 1: `xnuhook --request <request.json>` reads the request
//! and writes `<out>/plan.json`. The hook only plans; it never runs a tool.
//!
//! The plan covers the steps written so far, which are the byacc parsers
//! (ADR-17). Configure, the system-call generators, mig, compile, link and
//! install are not planned yet (README.md); the manifest says
//! `state = "incomplete"`.

mod parsers;

use serde::{Deserialize, Serialize};
use std::collections::BTreeMap;
use std::path::{Path, PathBuf};
use std::process::ExitCode;

const CONTRACT: u32 = 1;

#[allow(dead_code)] // target, config and board select steps not planned yet
#[derive(Debug, Deserialize)]
#[serde(deny_unknown_fields)]
struct Request {
    version: u32,
    component: String,
    target: String,
    #[serde(default)]
    config: String,
    #[serde(default)]
    board: String,
    src: PathBuf,
    out: PathBuf,
    tools: BTreeMap<String, PathBuf>,
}

/// One `generate` edge: run `argv`, which reads `inputs` and writes `outputs`.
#[derive(Debug, Serialize, PartialEq, Eq)]
pub struct Generate {
    pub outputs: Vec<PathBuf>,
    pub inputs: Vec<PathBuf>,
    pub argv: Vec<String>,
}

#[derive(Debug, Serialize)]
struct Plan {
    version: u32,
    inputs: Vec<PathBuf>,
    generate: Vec<Generate>,
    compile: Vec<serde_json::Value>,
    link: Vec<serde_json::Value>,
    install: Vec<serde_json::Value>,
}

fn plan(req: &Request) -> Result<Plan, String> {
    if req.version != CONTRACT {
        return Err(format!(
            "E-HOOK-VERSION: request version {} is not contract version {CONTRACT}",
            req.version
        ));
    }
    if req.component != "xnu" {
        return Err(format!(
            "E-HOOK-COMPONENT: xnuhook plans component xnu, not '{}'",
            req.component
        ));
    }
    let byacc = req
        .tools
        .get("byacc")
        .ok_or("E-HOOK-TOOL: the request names no byacc")?;
    let generate = parsers::plan(&req.src, &req.out, byacc)?;
    let inputs = generate.iter().flat_map(|g| g.inputs.clone()).collect();
    Ok(Plan {
        version: CONTRACT,
        inputs,
        generate,
        compile: Vec::new(),
        link: Vec::new(),
        install: Vec::new(),
    })
}

fn run(request: &Path) -> Result<PathBuf, String> {
    let text = std::fs::read_to_string(request)
        .map_err(|e| format!("cannot read {}: {e}", request.display()))?;
    let req: Request = serde_json::from_str(&text)
        .map_err(|e| format!("E-HOOK-REQUEST: {}: {e}", request.display()))?;
    let plan = plan(&req)?;
    std::fs::create_dir_all(&req.out)
        .map_err(|e| format!("cannot create {}: {e}", req.out.display()))?;
    let path = req.out.join("plan.json");
    let mut json = serde_json::to_string_pretty(&plan).map_err(|e| e.to_string())?;
    json.push('\n');
    std::fs::write(&path, json).map_err(|e| format!("cannot write {}: {e}", path.display()))?;
    Ok(path)
}

fn main() -> ExitCode {
    let args: Vec<String> = std::env::args().skip(1).collect();
    let request = match args.as_slice() {
        [flag, request] if flag == "--request" => request,
        _ => {
            eprintln!("usage: xnuhook --request <request.json>");
            return ExitCode::from(2);
        }
    };
    match run(Path::new(request)) {
        Ok(path) => {
            println!("xnuhook: wrote {}", path.display());
            ExitCode::SUCCESS
        }
        Err(e) => {
            eprintln!("error: {e}");
            ExitCode::FAILURE
        }
    }
}
