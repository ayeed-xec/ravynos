// SPDX-License-Identifier: BSD-2-Clause
// Copyright (c) 2026 The ravynOS Authors
use serde::Deserialize;
use std::path::PathBuf;

/// An image description (S08): `Tests/images/<name>/image.toml` for a test
/// image, `Images/<name>/image.toml` for an OS image.
#[derive(Debug, Deserialize)]
#[serde(deny_unknown_fields)]
pub struct Spec {
    pub kernel: String,
    #[serde(default)]
    pub kexts: Vec<String>,
    #[serde(default)]
    pub cmdline: String,
    #[serde(default)]
    pub ramdisk: Option<Ramdisk>,
}

#[derive(Debug, Deserialize)]
#[serde(deny_unknown_fields)]
pub struct Ramdisk {
    #[serde(default)]
    pub files: Vec<Copy>,
    #[serde(default)]
    pub dirs: Vec<String>,
}

#[derive(Debug, Deserialize)]
#[serde(deny_unknown_fields)]
pub struct Copy {
    pub from: String,
    pub to: String,
}

impl Spec {
    /// The manifest's own checks: a kernel and every boot file are named, and
    /// ramdisk destinations are absolute, as the boot collection expects.
    pub fn parse(text: &str) -> Result<Spec, String> {
        let spec: Spec = toml::from_str(text).map_err(|e| format!("error: E-SCHEMA: {e}"))?;
        if spec.kernel.is_empty() {
            return Err(String::from("error: E-SCHEMA: kernel is empty"));
        }
        if let Some(ramdisk) = &spec.ramdisk {
            for copy in &ramdisk.files {
                if copy.from.is_empty() {
                    return Err(String::from("error: E-SCHEMA: ramdisk source is empty"));
                }
                if !copy.to.starts_with('/') {
                    return Err(format!(
                        "error: E-SCHEMA: ramdisk destination '{}' is not absolute",
                        copy.to
                    ));
                }
            }
            for dir in &ramdisk.dirs {
                if !dir.starts_with('/') {
                    return Err(format!(
                        "error: E-SCHEMA: ramdisk directory '{dir}' is not absolute"
                    ));
                }
            }
        }
        Ok(spec)
    }
}

/// Find and validate one image. The image is not built yet: the boot
/// collection and kernelcache come from S09, so the caller reports E-NOT-BUILT.
pub fn load(tree: &std::path::Path, name: &str) -> Result<(PathBuf, Spec), String> {
    if name.is_empty() {
        return Err(String::from("error: E-USAGE: rv image -t <target> <name>"));
    }
    for base in ["Tests/images", "Images"] {
        let path = tree.join(base).join(name).join("image.toml");
        if path.is_file() {
            let text = std::fs::read_to_string(&path)
                .map_err(|e| format!("error: read {}: {e}", path.display()))?;
            let spec =
                Spec::parse(&text).map_err(|e| format!("{e}\n       in {}", path.display()))?;
            return Ok((path, spec));
        }
    }
    Err(format!(
        "error: E-IMAGE: no image named '{name}' (Tests/images/{name}/image.toml or Images/{name}/image.toml)"
    ))
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::fs;

    const BOOT_SMOKE: &str = "kernel = \"testkernel\"\nkexts = [\"hfs\"]\n\
                              cmdline = \"debug=0x4e serial=3\"\n\
                              [ramdisk]\nfiles = [{ from = \"t0\", to = \"/sbin/launchd\" }]\n\
                              dirs = [\"/dev\", \"/tmp\"]\n";

    fn tree(label: &str, image: &str) -> PathBuf {
        let dir = std::env::temp_dir().join(format!("rv-image-{}-{label}", std::process::id()));
        fs::create_dir_all(dir.join("Tests/images").join(image)).unwrap();
        fs::write(
            dir.join("Tests/images").join(image).join("image.toml"),
            BOOT_SMOKE,
        )
        .unwrap();
        dir
    }

    #[test]
    fn a_boot_smoke_image_parses() {
        let spec = Spec::parse(BOOT_SMOKE).expect("parses");
        assert_eq!(spec.kernel, "testkernel");
        assert_eq!(spec.kexts, vec![String::from("hfs")]);
        assert_eq!(spec.ramdisk.expect("ramdisk").dirs, vec!["/dev", "/tmp"]);
    }

    #[test]
    fn a_missing_kernel_is_a_schema_error() {
        let err = Spec::parse("kexts = []\n").unwrap_err();
        assert!(err.contains("E-SCHEMA"), "{err}");
    }

    #[test]
    fn an_unknown_key_is_refused() {
        let err = Spec::parse("kernel = \"xnu\"\nbootloader = \"loader\"\n").unwrap_err();
        assert!(err.contains("E-SCHEMA"), "{err}");
    }

    #[test]
    fn a_relative_ramdisk_destination_is_refused() {
        let text = "kernel = \"xnu\"\n[ramdisk]\nfiles = [{ from = \"t0\", to = \"bin/sh\" }]\n";
        let err = Spec::parse(text).unwrap_err();
        assert!(err.contains("not absolute"), "{err}");
    }

    #[test]
    fn load_finds_a_test_image_and_reports_a_missing_one() {
        let dir = tree("find", "boot-smoke");
        let (path, spec) = load(&dir, "boot-smoke").expect("found");
        assert!(path.ends_with("Tests/images/boot-smoke/image.toml"));
        assert_eq!(spec.kernel, "testkernel");
        let err = load(&dir, "nope").unwrap_err();
        assert!(err.contains("E-IMAGE"), "{err}");
        fs::remove_dir_all(&dir).ok();
    }
}
