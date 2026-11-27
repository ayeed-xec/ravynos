// SPDX-License-Identifier: BSD-2-Clause
// Copyright (c) 2026 The ravynOS Authors
use std::collections::BTreeMap;

const FENCES: &[(&str, &[&str])] = &[
    ("bootloader", &[]),
    ("kernel", &[]),
    ("foundation", &["kernel"]),
    ("frameworks", &["foundation"]),
    ("server", &["frameworks", "foundation"]),
    ("apps", &["frameworks", "foundation", "server"]),
    ("compat", &["kernel"]),
    ("build-toolchain", &[]),
    (
        "test",
        &[
            "bootloader",
            "kernel",
            "foundation",
            "frameworks",
            "server",
            "apps",
            "compat",
            "build-toolchain",
        ],
    ),
];

pub fn is_layer(name: &str) -> bool {
    FENCES.iter().any(|(l, _)| *l == name)
}

pub fn each_layer(f: impl FnMut(&'static str)) {
    FENCES.iter().map(|(l, _)| *l).for_each(f);
}

fn may_depend(from: &str, to: &str) -> bool {
    FENCES
        .iter()
        .find(|(l, _)| *l == from)
        .map(|(_, allowed)| to == from || allowed.contains(&to))
        .unwrap_or(false)
}

pub fn resolve(
    manifests: &BTreeMap<String, crate::manifest::Manifest>,
) -> Result<BTreeMap<String, Vec<String>>, String> {
    let mut graph = BTreeMap::new();
    for m in manifests.values() {
        for dep in &m.deps.internal {
            let Some(dep_m) = manifests.get(dep) else {
                return Err(format!(
                    "error: E-DEP-UNKNOWN: {} declares dependency '{}' but no manifest defines it",
                    m.name, dep
                ));
            };
            if !may_depend(&m.layer, &dep_m.layer) {
                return Err(format!(
                    "error: E-LAYER: {} -> {} (private) violates layer rules\n       {} may not depend on {} layer\n       (02-architecture.md, layer table)",
                    m.name, dep, m.layer, dep_m.layer
                ));
            }
        }
        for dep in &m.deps.upward {
            if !manifests.contains_key(dep) {
                return Err(format!(
                    "error: E-DEP-UNKNOWN: {} declares upward dependency '{}' but no manifest defines it",
                    m.name, dep
                ));
            }
        }
        graph.insert(m.name.clone(), m.deps.internal.clone());
    }
    let mut state = BTreeMap::new();
    for comp in graph.keys() {
        visit(comp, &graph, &mut state)?;
    }
    Ok(graph)
}

fn visit(
    comp: &str,
    graph: &BTreeMap<String, Vec<String>>,
    state: &mut BTreeMap<String, u8>,
) -> Result<(), String> {
    match state.get(comp) {
        Some(2) => return Ok(()),
        Some(1) => return Err(format!("error: E-CYCLE: dependency cycle through '{comp}'")),
        _ => {}
    }
    state.insert(comp.to_string(), 1);
    for dep in &graph[comp] {
        visit(dep, graph, state)?;
    }
    state.insert(comp.to_string(), 2);
    Ok(())
}

pub fn closure(
    graph: &BTreeMap<String, Vec<String>>,
    selected: &std::collections::BTreeSet<String>,
    order: &mut Vec<String>,
) -> Result<(), String> {
    let mut state = BTreeMap::new();
    for comp in selected {
        emit(comp, graph, &mut state, order)?;
    }
    Ok(())
}

fn emit(
    comp: &str,
    graph: &BTreeMap<String, Vec<String>>,
    state: &mut BTreeMap<String, u8>,
    order: &mut Vec<String>,
) -> Result<(), String> {
    match state.get(comp).copied().unwrap_or(0) {
        2 => return Ok(()),
        1 => return Err(format!("error: E-CYCLE: dependency cycle through '{comp}'")),
        _ => {}
    }
    state.insert(comp.to_string(), 1);
    for dep in &graph[comp] {
        emit(dep, graph, state, order)?;
    }
    state.insert(comp.to_string(), 2);
    order.push(comp.to_string());
    Ok(())
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn allowed_edges_follow_the_layer_table() {
        assert!(may_depend("apps", "foundation"));
        assert!(may_depend("apps", "frameworks"));
        assert!(may_depend("apps", "server"));
        assert!(may_depend("frameworks", "foundation"));
        assert!(may_depend("foundation", "kernel"));
        assert!(may_depend("server", "foundation"));
        assert!(may_depend("server", "frameworks"));
        assert!(may_depend("compat", "kernel"));
    }

    #[test]
    fn upward_edges_are_rejected() {
        assert!(!may_depend("foundation", "frameworks"));
        assert!(!may_depend("frameworks", "apps"));
        assert!(!may_depend("apps", "kernel"));
        assert!(!may_depend("apps", "compat"));
        assert!(!may_depend("server", "kernel"));
        assert!(!may_depend("kernel", "foundation"));
        assert!(!may_depend("compat", "foundation"));
    }

    #[test]
    fn unknown_layers_depend_on_nothing() {
        assert!(!may_depend("no-such-layer", "kernel"));
        assert!(!may_depend("kernel", "no-such-layer"));
    }

    #[test]
    fn same_layer_edges_are_allowed() {
        for (layer, _) in FENCES {
            assert!(may_depend(layer, layer), "{layer} must depend on itself");
        }
    }

    #[test]
    fn all_nine_layers_are_declared() {
        assert_eq!(FENCES.len(), 9);
        for (layer, _) in FENCES {
            assert!(is_layer(layer));
        }
    }

    #[test]
    fn test_layer_may_depend_on_any_layer() {
        for (layer, _) in FENCES {
            assert!(may_depend("test", layer), "test must depend on {layer}");
        }
        for (layer, _) in FENCES {
            if *layer != "test" {
                assert!(
                    !may_depend(layer, "test"),
                    "{layer} must not depend on test"
                );
            }
        }
    }

    #[test]
    fn build_toolchain_is_fenced_to_itself() {
        assert!(may_depend("build-toolchain", "build-toolchain"));
        assert!(!may_depend("build-toolchain", "foundation"));
        assert!(!may_depend("apps", "build-toolchain"));
    }
}
