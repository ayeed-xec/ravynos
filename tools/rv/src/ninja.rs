use std::collections::BTreeMap;

pub fn emit(
    tree_root: &std::path::Path,
    manifests: &BTreeMap<String, crate::manifest::Manifest>,
    deps: &BTreeMap<String, Vec<String>>,
    target: &str,
) -> String {
    let mut out = String::new();
    out.push_str(&format!(
        "target = {target}\nrule cc\n  command = clang --target=$target -ffreestanding -fno-builtin -nostdinc -c $in -o $out\nrule link\n  command = clang -nostdlib -static $in -o $out\n\n"
    ));
    let _ = deps;
    let _ = tree_root;
    for m in manifests.values() {
        let comp_dir = m.name.clone();
        let c_srcs: Vec<String> = m
            .sources
            .iter()
            .filter(|s| s.ends_with(".c"))
            .cloned()
            .collect();
        for src in c_srcs.clone() {
            let abs_src = format!("{}/{}", m.dir.display(), src);
            let obj = src.replace('/', "_");
            out.push_str(&format!(
                "build {comp_dir}/{obj}.o: cc ../../{abs_src}\n  target = {target}\n"
            ));
        }
        let objs: Vec<String> = c_srcs
            .iter()
            .map(|s| format!("{comp_dir}/{}", s.replace('/', "_") + ".o"))
            .collect();
        if objs.is_empty() {
            continue;
        }
        match m.exports.libs.first() {
            Some(lib) => out.push_str(&format!("build {comp_dir}/{lib}: link {}\n", objs.join(" "))),
            None => out.push_str(&format!("build {comp_dir}/{}: link {}\n", m.name, objs.join(" "))),
        }
    }
    out
}
