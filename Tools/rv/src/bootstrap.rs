// SPDX-License-Identifier: BSD-2-Clause
// Copyright (c) 2026 The ravynOS Authors

//! `rv bootstrap`: build and verify the stage-1 toolchain (S02).
//!
//! Stage 1 is two components with separate cache keys. `buildtools`
//! (Tools/toolchain/buildtools.sh: byacc, unifdef, rvpatch, mkfat, elf2pe,
//! mig) takes seconds; `llvm` (Tools/toolchain/stage1.sh: clang, lld and the
//! LLVM tools from upstream llvm-project) takes hours. Neither key reads the
//! other's inputs, so one can change without rebuilding the other.

use std::path::Path;
use std::process::Command;

const USAGE: &str = "usage: rv bootstrap [--verify] [--refetch] [-j <jobs>] [buildtools|llvm...]";

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
enum Component {
    BuildTools,
    Llvm,
}

#[derive(Debug, PartialEq, Eq)]
struct Plan {
    verify: bool,
    refetch: bool,
    jobs: Option<String>,
    components: Vec<Component>,
}

fn parse(args: &[String]) -> Result<Plan, String> {
    let mut plan = Plan {
        verify: false,
        refetch: false,
        jobs: None,
        components: Vec::new(),
    };
    let mut it = args.iter();
    while let Some(arg) = it.next() {
        let component = match arg.as_str() {
            "--verify" => {
                plan.verify = true;
                continue;
            }
            "--refetch" => {
                plan.refetch = true;
                continue;
            }
            "-j" => match it.next() {
                Some(j) if !j.is_empty() && j.bytes().all(|b| b.is_ascii_digit()) => {
                    plan.jobs = Some(j.clone());
                    continue;
                }
                _ => return Err(USAGE.to_string()),
            },
            "buildtools" => Component::BuildTools,
            "llvm" => Component::Llvm,
            _ => return Err(USAGE.to_string()),
        };
        if !plan.components.contains(&component) {
            plan.components.push(component);
        }
    }
    if plan.components.is_empty() {
        // The cheap component first, so a broken byacc or mig shows up before
        // the LLVM build starts.
        plan.components = vec![Component::BuildTools, Component::Llvm];
    }
    Ok(plan)
}

/// The scripts one component runs, in order, each as `bash <script> <args>`.
fn steps(plan: &Plan, component: Component) -> Vec<(&'static str, Vec<String>)> {
    let mut flags = Vec::new();
    if plan.verify {
        flags.push("--verify".to_string());
    }
    if plan.refetch {
        flags.push("--refetch".to_string());
    }
    match component {
        Component::BuildTools => vec![("Tools/toolchain/buildtools.sh", flags)],
        Component::Llvm => {
            if let Some(j) = &plan.jobs {
                flags.push("-j".to_string());
                flags.push(j.clone());
            }
            let mut steps = Vec::new();
            // stage1.sh --verify rebuilds from build/vendor/llvm-project but
            // does not fetch it, so a verify on a fresh checkout with a
            // restored cache needs the source first.
            if plan.verify {
                let fetch_flags = if plan.refetch {
                    vec!["--refetch".to_string()]
                } else {
                    Vec::new()
                };
                steps.push(("Tools/toolchain/llvm/fetch.sh", fetch_flags));
            }
            steps.push(("Tools/toolchain/stage1.sh", flags));
            steps
        }
    }
}

/// Whether `args` form a valid `rv bootstrap` command line, so the caller can
/// print the usage and exit 2 before any work starts.
pub fn args_valid(args: &[String]) -> bool {
    parse(args).is_ok()
}

pub fn run(tree: &Path, args: &[String]) -> Result<(), String> {
    let plan = parse(args).map_err(|usage| format!("error: {usage}"))?;
    for &component in &plan.components {
        for (script, script_args) in steps(&plan, component) {
            let path = tree.join(script);
            if !path.is_file() {
                return Err(format!("error: {} not found", path.display()));
            }
            let status = Command::new("bash")
                .arg(&path)
                .args(&script_args)
                .current_dir(tree)
                .status()
                .map_err(|e| format!("error: cannot run {}: {e}", path.display()))?;
            if !status.success() {
                return Err(format!("error: {script} failed with {status}"));
            }
        }
    }
    Ok(())
}

#[cfg(test)]
mod tests {
    use super::*;

    fn args(list: &[&str]) -> Vec<String> {
        list.iter().map(|s| s.to_string()).collect()
    }

    #[test]
    fn default_builds_buildtools_then_llvm() {
        let plan = parse(&[]).expect("no arguments is valid");
        assert_eq!(
            plan.components,
            vec![Component::BuildTools, Component::Llvm]
        );
        assert!(!plan.verify);
    }

    #[test]
    fn buildtools_alone_never_runs_the_llvm_scripts() {
        let plan = parse(&args(&["--verify", "buildtools"])).expect("valid");
        assert_eq!(plan.components, vec![Component::BuildTools]);
        let scripts: Vec<&str> = steps(&plan, Component::BuildTools)
            .into_iter()
            .map(|(s, _)| s)
            .collect();
        assert_eq!(scripts, vec!["Tools/toolchain/buildtools.sh"]);
    }

    #[test]
    fn llvm_verify_fetches_before_rebuilding() {
        let plan = parse(&args(&["llvm", "--verify", "-j", "8"])).expect("valid");
        let steps = steps(&plan, Component::Llvm);
        assert_eq!(steps[0].0, "Tools/toolchain/llvm/fetch.sh");
        assert_eq!(steps[1].0, "Tools/toolchain/stage1.sh");
        assert_eq!(steps[1].1, args(&["--verify", "-j", "8"]));
    }

    #[test]
    fn refetch_reaches_the_llvm_fetch_before_a_verify() {
        let plan = parse(&args(&["--verify", "--refetch", "llvm"])).expect("valid");
        let steps = steps(&plan, Component::Llvm);
        assert_eq!(steps[0].1, args(&["--refetch"]));
        assert_eq!(steps[1].1, args(&["--verify", "--refetch"]));
    }

    #[test]
    fn jobs_go_to_llvm_only() {
        let plan = parse(&args(&["-j", "4"])).expect("valid");
        assert!(steps(&plan, Component::BuildTools)[0].1.is_empty());
        assert_eq!(steps(&plan, Component::Llvm)[0].1, args(&["-j", "4"]));
    }

    #[test]
    fn rejects_bad_arguments() {
        for bad in [
            &["-j"][..],
            &["-j", "x"],
            &["-j", ""],
            &["clang"],
            &["--fast"],
        ] {
            assert!(parse(&args(bad)).is_err(), "accepted {bad:?}");
        }
    }
}
