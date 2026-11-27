// SPDX-License-Identifier: BSD-2-Clause
// Copyright (c) 2026 The ravynOS Authors

//! `rv bootstrap`: build and verify the stage-1 toolchain.
//!
//! This is a thin wrapper over Tools/toolchain/stage1.sh, which fetches
//! swiftlang/llvm-project at the pinned commit, builds clang, lld, llvm-ar,
//! llvm-nm and llvm-symbolizer with a content-hash cache key, and can verify
//! an existing cache entry (SYSTEM-DESIGN.md S02 stage-1/D8).

use std::path::Path;
use std::process::exit;

fn usage() -> ! {
    eprintln!("usage: rv bootstrap [--verify] [--refetch] [-j <jobs>]");
    exit(2);
}

pub fn run(tree: &Path, args: &[String]) -> Result<(), String> {
    let script = tree.join("Tools/toolchain/stage1.sh");
    if !script.is_file() {
        return Err(format!(
            "error: {} not found (rv bootstrap runs from the tree root)",
            script.display()
        ));
    }

    let mut cmd = std::process::Command::new("bash");
    cmd.arg(&script);
    let mut it = args.iter().peekable();
    while let Some(arg) = it.next() {
        match arg.as_str() {
            "--verify" | "--refetch" => {
                cmd.arg(arg);
            }
            "-j" => {
                let jobs = match it.next() {
                    Some(j) => j,
                    None => usage(),
                };
                if jobs.is_empty() || !jobs.bytes().all(|b| b.is_ascii_digit()) {
                    usage();
                }
                cmd.arg("-j").arg(jobs);
            }
            _ => usage(),
        }
    }

    let status = cmd
        .status()
        .map_err(|e| format!("error: cannot run {}: {e}", script.display()))?;
    if !status.success() {
        return Err(format!("error: {} failed with {status}", script.display()));
    }
    Ok(())
}
