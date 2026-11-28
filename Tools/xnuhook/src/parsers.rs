// SPDX-License-Identifier: BSD-2-Clause
// Copyright (c) 2026 The ravynOS Authors

//! The XNU parsers upstream ships as Bison output (ADR-17). The shipped copies
//! are pruned from `Kernel/xnu` (VENDORS `prune`); these edges regenerate them
//! with byacc from the grammars, and only that output may be compiled.

use crate::Generate;
use std::path::Path;

/// The Bison outputs that must stay out of the source tree.
pub const PRUNED: &[&str] = &[
    "libkern/c++/OSUnserialize.cpp",
    "iokit/System/OSUnserializeXMLSharedImplementation.h",
    "libkern/libkern/c++/OSUnserializeXMLSharedImplementation.h",
];

/// (grammar, symbol prefix, output). Each grammar's header comment names its
/// prefix (`bison -p OSUnserialize`, `bison -p OSUnserializeXML`): both
/// parsers link into one kernel, so an unprefixed `yyparse` would clash. The
/// XML grammar declares `%pure_parser` itself, so no `-P` is passed.
const GRAMMARS: &[(&str, &str, &str)] = &[
    (
        "libkern/c++/OSUnserialize.y",
        "OSUnserialize",
        "OSUnserialize.cpp",
    ),
    (
        "libkern/c++/OSUnserializeXML.y",
        "OSUnserializeXML",
        "OSUnserializeXMLSharedImplementation.h",
    ),
];

pub fn plan(src: &Path, out: &Path, byacc: &Path) -> Result<Vec<Generate>, String> {
    for stale in PRUNED {
        if src.join(stale).exists() {
            return Err(format!(
                "E-PRUNED: {} is Bison output and must stay pruned (ADR-17)",
                src.join(stale).display()
            ));
        }
    }
    let mut edges = Vec::new();
    for (grammar, prefix, output) in GRAMMARS {
        let input = src.join(grammar);
        if !input.is_file() {
            return Err(format!("E-HOOK-SOURCE: {} does not exist", input.display()));
        }
        let output = out.join("parsers").join(output);
        edges.push(Generate {
            argv: vec![
                byacc.display().to_string(),
                "-p".to_string(),
                prefix.to_string(),
                "-o".to_string(),
                output.display().to_string(),
                input.display().to_string(),
            ],
            outputs: vec![output],
            inputs: vec![input],
        });
    }
    Ok(edges)
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::fs;
    use std::path::PathBuf;

    fn fake_xnu(name: &str) -> PathBuf {
        let dir = std::env::temp_dir().join("xnuhook-unit").join(name);
        let _ = fs::remove_dir_all(&dir);
        for (grammar, _, _) in GRAMMARS {
            let path = dir.join(grammar);
            fs::create_dir_all(path.parent().unwrap()).unwrap();
            fs::write(path, "%%\n").unwrap();
        }
        dir
    }

    #[test]
    fn each_grammar_gets_its_own_prefix() {
        let src = fake_xnu("prefix");
        let edges = plan(&src, Path::new("/o"), Path::new("/t/byacc")).unwrap();
        assert_eq!(edges.len(), 2);
        assert_eq!(edges[0].argv[1..3], ["-p", "OSUnserialize"]);
        assert_eq!(edges[1].argv[1..3], ["-p", "OSUnserializeXML"]);
        assert_eq!(
            edges[1].outputs,
            [PathBuf::from(
                "/o/parsers/OSUnserializeXMLSharedImplementation.h"
            )]
        );
        assert!(edges.iter().all(|e| !e.argv.contains(&"-P".to_string())));
    }

    #[test]
    fn a_pruned_file_that_came_back_is_refused() {
        for stale in PRUNED {
            let src = fake_xnu("pruned");
            let path = src.join(stale);
            fs::create_dir_all(path.parent().unwrap()).unwrap();
            fs::write(&path, "bison output").unwrap();
            let err = plan(&src, Path::new("/o"), Path::new("byacc")).unwrap_err();
            assert!(err.starts_with("E-PRUNED"), "{stale}: {err}");
        }
    }

    #[test]
    fn pruned_list_matches_vendors() {
        // VENDORS is the source of truth for what is pruned from Kernel/xnu;
        // the hook repeats the list because it sees only the source copy.
        let vendors = fs::read_to_string(
            std::path::Path::new(env!("CARGO_MANIFEST_DIR")).join("../../VENDORS"),
        )
        .unwrap();
        let entry = vendors
            .split("[[vendor]]")
            .find(|e| e.contains("name     = \"xnu\""))
            .expect("VENDORS has an xnu entry");
        let line = entry
            .lines()
            .find(|l| l.starts_with("prune"))
            .expect("xnu has a prune list");
        let listed: Vec<&str> = line.split('"').skip(1).step_by(2).collect();
        assert_eq!(listed, PRUNED);
    }

    #[test]
    fn a_missing_grammar_is_an_error() {
        let src = fake_xnu("missing");
        fs::remove_file(src.join(GRAMMARS[1].0)).unwrap();
        let err = plan(&src, Path::new("/o"), Path::new("byacc")).unwrap_err();
        assert!(err.starts_with("E-HOOK-SOURCE"), "{err}");
    }
}
