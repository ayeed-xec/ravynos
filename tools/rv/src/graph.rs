use std::collections::BTreeMap;

const FENCES: &[(&str, &[&str])] = &[
    ("bootloader", &[]),
    ("kernel", &[]),
    ("foundation", &["kernel"]),
    ("server", &["foundation", "kernel"]),
    ("frameworks", &["foundation"]),
    ("apps", &["frameworks", "foundation", "server"]),
    ("compat", &["kernel"]),
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
        .map(|(_, allowed)| allowed.contains(&to))
        .unwrap_or(false)
}

pub fn resolve(
    manifests: &BTreeMap<String, crate::manifest::Manifest>,
    forbid: bool,
) -> BTreeMap<String, Vec<String>> {
    let mut graph = BTreeMap::new();
    for m in manifests.values() {
        for dep in &m.deps.internal {
            let Some(dep_m) = manifests.get(dep) else {
                eprintln!(
                    "error: {} declares dependency '{}' but no manifest defines it",
                    m.name, dep
                );
                std::process::exit(1);
            };
            if !may_depend(&m.layer, &dep_m.layer) {
                eprintln!(
                    "error: {} -> {} (private) violates layer rules\n       {} may not depend on {} layer\n       (02-architecture.md, layer table)",
                    m.name, dep, m.layer, dep_m.layer
                );
                std::process::exit(1);
            }
        }
        graph.insert(m.name.clone(), m.deps.internal.clone());
    }
    let mut state = BTreeMap::new();
    for comp in graph.keys() {
        visit(comp, &graph, &mut state);
    }
    let _ = forbid;
    graph
}

fn visit(
    comp: &str,
    graph: &BTreeMap<String, Vec<String>>,
    state: &mut BTreeMap<String, u8>,
) {
    match state.get(comp) {
        Some(2) => return,
        Some(1) => {
            eprintln!("error: dependency cycle through '{comp}'");
            std::process::exit(1);
        }
        _ => {}
    }
    state.insert(comp.to_string(), 1);
    for dep in &graph[comp] {
        visit(dep, graph, state);
    }
    state.insert(comp.to_string(), 2);
}

pub fn closure(
    graph: &BTreeMap<String, Vec<String>>,
    selected: &std::collections::BTreeSet<String>,
    order: &mut Vec<String>,
) {
    let mut state = BTreeMap::new();
    for comp in selected {
        emit(comp, graph, selected, &mut state, order);
    }
}

fn emit(
    comp: &str,
    graph: &BTreeMap<String, Vec<String>>,
    selected: &std::collections::BTreeSet<String>,
    state: &mut BTreeMap<String, u8>,
    order: &mut Vec<String>,
) {
    if state.get(comp).copied().unwrap_or(0) == 2 {
        return;
    }
    state.insert(comp.to_string(), 2);
    for dep in &graph[comp] {
        if selected.contains(dep) {
            emit(dep, graph, selected, state, order);
        }
    }
    order.push(comp.to_string());
}
