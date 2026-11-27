# Status

Where the code stands against [SYSTEM-DESIGN.md](SYSTEM-DESIGN.md), subsystem by
subsystem. This is the only document that records status; update it in the same commit
as the code it describes. Gaps are numbered GAP-01 onward and bugs BUG-01 onward; the
numbers are permanent, and a closed item is marked closed, not deleted.

## Summary

Audit of `darwin-clean` at `b1746b5ce` (four reviewers: build harness, bootloader,
kernel side, toolchain and repository hygiene), scored against the section 2.7 gate of
8/10. No component passes the gate.

| Area | Score | Verdict |
|---|---|---|
| `Tools/rv` (WP0.1–WP0.3) | 4/10 | rework; Mach-O output and kind handling missing (BUG-27, GAP-01–GAP-06) |
| Stage-1 bootstrap (WP0.4) | 3/10 | rework; the build step has never run (BUG-22) |
| `Boot/loader` (WP0.9) | 2/10 | rework; no device tree, smokes pass on a panic |
| `Tools/mig` | 5/10 | patches and paths need fixing (GAP-11, GAP-55) |
| CI and gates | 3/10 | partial coverage (GAP-09, GAP-10, GAP-33, GAP-56) |
| FreeBSDPort | — | removed from the tree (WP0.0); rewrite under S11 (BUG-11, BUG-19, GAP-24) |
| IOUSBFamily reconstruction | — | deleted (WP0.0); native stack in S10 (GAP-42, GAP-43) |
| RavynXHCIPort | — | removed from the tree (WP0.0); rewrite when S10 starts (BUG-17, GAP-48) |
| RavynAHCIPort | — | removed from the tree (WP0.0); rewrite when S10 starts (GAP-48) |
| AppleAPIC | — | removed from the tree (WP0.0); re-vendor, then patch (BUG-20) |
| ApplePS2Controller | — | removed from the tree (WP0.0); re-vendor controller, rewrite keyboard files (GAP-45) |
| IOACPIFamily (PureDarwin edits) | — | removed from the tree (WP0.0); re-vendor, then patch (BUG-14, GAP-47) |
| hfs | — | removed from the tree (WP0.0); re-vendor from the tagged release (GAP-44) |

The W0 wave report claimed WP0.1–WP0.4 and WP0.9 as delivered. They are merged but not
done: each is open below until it meets its "Done when" and passes review.

Both arm64 and x86_64 are primary (ADR-19). The build tool, the bootloader's UEFI
plumbing, the vendored mig and the CI gates exist but fail review. No Darwin program has run, the kernel does not
link, and nothing above the kernel exists yet. Tiers T0–T5 are all open.

What exists, and whether it is a real implementation:

- `Tools/rv/` — real, small, well-structured Rust; three unit tests; several documented
  behaviors not yet implemented (GAP-01 to GAP-06).
- `Boot/loader/` — real UEFI code that boots ELF test kernels on three architectures;
  its Mach-O and x86 hand-off paths are broken (BUG-01 to BUG-04).
- `Boot/mkfat/`, `Boot/elf2pe/`, `Tools/mig/rvpatch.c` — real, with unit tests.
- `Tools/mig/` — Apple's migcom, pristine, with three patches, built with byacc.
- `Tools/ci/` — real gates with incomplete coverage (GAP-09, GAP-10).
- `Tests/apps/hello`, `Tests/apps/hello-cli` — test programs that make Linux system
  calls; neither is a Darwin program (GAP-07). Moved from `Apps/` by WP0.0.
- `Legacy/` — removed by WP0.0 (ADR-30, GAP-60); it is read only as the old tree
  (`old:` paths, `git show 5112c72bc:<path>`).

## Bugs

- **BUG-01** Closed. `Boot/loader/macho.c` read `LC_SEGMENT_64` fields at wrong offsets: vmaddr
  at 16 (should be 24), file offset from the vmsize field, file size from the file-offset
  field, memory size from the file-size field.
- **BUG-02** Partly fixed: the entry point and `LC_MAIN` are corrected; still open:
  `macho_phys` hard-codes the x86 base, `filesize <= vmsize` is not checked, and segments
  beyond the 32nd are dropped without an error. Original report: `Boot/loader/macho.c` reads the x86 entry point from thread-state index 20
  (`gs`) instead of 16 (`rip`); `LC_MAIN` is defined as `0x28` instead of `0x80000028`
  and its `entryoff` (a file offset) is used as a virtual address; no bounds checks on
  segment offsets and sizes; `macho_phys` hard-codes the x86 kernel base, which is wrong
  for arm64 kernels.
- **BUG-03** Mostly fixed: the jump target and segment reloads are corrected; still open:
  `EFER.LME` and `CR4.PAE` are not cleared before entering 32-bit protected mode without
  paging, and the trampoline is not placed below 4 GB. Original report: `Boot/loader/handoff.S`: after the 64→32 switch it jumps to `eax`, which
  holds the `boot_args` address, not the kernel entry; data segments are not reloaded.
- **BUG-04** Mostly fixed. Original report: `Boot/loader/main.c`: the memory map in `boot_args` is captured before the
  final allocations; the map used for `ExitBootServices` never reaches the kernel.
- **BUG-05** `Boot/loader/main.c`: `deviceTreeP` is zero; XNU then parses physical
  address 0 as a device tree. No device tree is built at all, so the required
  `/chosen/random-seed`, `/chosen/memory-map` and `/efi/platform` properties are absent.
  Video, runtime-services fields and `Revision` 1 (needed for a kernelcache) are unset;
  the command line is a placeholder.
- **BUG-06** The arm64 loader path passes the x86 `boot_args` and leaves the MMU and
  caches on at hand-off.
- **BUG-07** `Kernel/port/arm64/pmap.c` consists of functions that ignore their arguments
  and return success — stubs, which the design forbids.
- **BUG-08** `Kernel/build/driver/compile.sh` defines `__i386__` for x86_64, selecting
  32-bit paths in XNU headers.
- **BUG-09** Closed. `Apps/hello` and `Apps/hello-cli` were both named `hello`;
  `rv` rejected duplicate names with `E-DUP-NAME`, so every `rv` command failed on
  the tree as committed. The tree now follows the section 3.2 layout: both programs
  live in `Tests/apps/`, each manifest carries its own name, and their layer is
  `test` per the section 3.2 layer table. `rv graph` and `rv build` pass on the
  moved tree.
- **BUG-10** `rv shell` prints an environment bash rejects and points at a directory with
  no `build.ninja`.
- **BUG-11** `Kernel/Extensions/FreeBSDPort/shim` calls `ifnet_allocate_extended` with
  the wrong arguments for XNU's declaration.
- **BUG-12** Closed. `Boot/build.sh:113` names `loader/aarch64/efi-objlst.ld`, which does not
  exist, in a freshness test; harmless today, but a dead reference.
- **BUG-13** `Boot/loader/handoff.S` uses absolute 32-bit relocations, so the loader works
  only when the firmware places it below 4 GB; nothing checks this.
- **BUG-14** `IOACPIFamily`: the two `evaluateObject` overloads call each other with no
  base case; the first `_STA` evaluation overflows the kernel stack.
- **BUG-15** `Kernel/port/riscv64/trap.S` adjusts `sp` by 256 bytes on entry and does
  not restore it on every exit path; each trap leaks 256 bytes of stack.
- **BUG-16** `Kernel/port/arm64` references `arm64_exception_entry`, which nothing
  defines.
- **BUG-17** `RavynXHCIPort::stop` frees ring and context memory before halting the
  controller; the controller can DMA into freed memory, and objects are used after
  release.
- **BUG-18** RavynXHCIPort and RavynAHCIPort call `IOMemoryMap::read32`, which does not
  exist in XNU's `IOMemoryMap`.
- **BUG-19** FreeBSDPort's `ticks` never advances, so every timeout in the shim either
  fires at once or never.
- **BUG-20** `AppleAPIC` (8259 path) calls `kprintf` on every interrupt.
- **BUG-21** FreeBSDPort calls `va_start` with its arguments reversed.
- **BUG-22** `Tools/toolchain/llvm/build.sh:282` runs `cmake -G ninja`; generator names
  are case-sensitive, so the stage-1 build has never run. The script also requires
  `byacc` (`:264`) without using it, and strips with whatever `llvm-strip` is on `PATH`
  with errors discarded (`:313-314`).
- **BUG-23** `Tools/toolchain/stage1.sh:111`: the sed expression never matches, so the
  stamp's `pin:` field is always empty.
- **BUG-24** `Boot/testkernel`: the stack pointer is set to the bottom of `__stack`
  instead of the top.
- **BUG-25** `Boot/loader`: loader data the kernel reads is allocated as
  `EfiRuntimeServicesData` outside the range `ksize` covers; `SetVirtualAddressMap` is
  never called; `boot_args` `Revision` is 0 and the command line is the placeholder
  `rvynos`.
- **BUG-26** The boot smokes (`Boot/smoke/*`) pass on any panic message, so a kernel
  that panics at once is reported as a success; the ESP paths they write do not match
  where the loader looks.
- **BUG-27** `rv`: a manifest `name` such as `../../x` resolves outside `build/`;
  manifest discovery follows symbolic links; `kind` is read but ignored, so a dylib is
  built as an `ar` archive; library code calls `process::exit` and `unwrap`.
- **BUG-28** `Kernel/Extensions/hfs/core/BTreeMiscOps.c:181-201` was edited in place to
  relax `VerifyHeader`, so corrupt B-tree headers are accepted.

## Gaps by subsystem

**S01 build harness**
- GAP-01 rv emits ELF; no Mach-O output (needs ADR-03).
- GAP-02 No dependency files in the compile rule; header edits rebuild nothing.
- GAP-03 Only `.c` sources compile; `.m`, `.mm`, `.cpp`, `.S` are skipped silently. No
  dylib, framework, kext or bundle outputs.
- GAP-04 No sysroot is passed or assembled.
- GAP-05 Unknown manifest keys are ignored; `state`, `tests`, `vendored` unread.
- GAP-06 `image`, `test`, `vendors`, `sdk`, `bootstrap`, `audit size`, `kernelcache` not
  implemented; `graph --forbid` does nothing.
- GAP-54 `rv` has no `--root`; cycle and layer errors lack the `E-CYCLE` and `E-LAYER`
  codes; there are no negative tests; the fixtures in `Tests/rv/positive-schema` are
  wired to nothing and one uses a key (`match`) outside the schema.

**S02 toolchain**
- GAP-07 No Darwin program exists; the test programs use Linux system calls.
- GAP-08 No stage-1 toolchain; CI uses the distribution's clang, unpinned; no
  `rust-toolchain.toml`; `cargo` not run with `--locked`; `rv` is dynamically linked.
- GAP-09 The license audit scans only the three manifested directories and only some file
  types; no crate scanning; no fingerprinting.
- GAP-10 The host-tool gate scans only some directories: it skips `Tools/toolchain/`,
  `Kernel/build/` (gcc, `-lgcc`, `grub-file`), `Kernel/Extensions/**/*.sh`,
  `Tools/build.sh` and every Makefile; its deny list lacks `flex`, `grub`, `sed`, `awk`,
  `perl` and `apt-get`, and has no rule confining `cmake` and `python3` to the llvm and
  swift components. With the full list, 26 files fail. In Rust it catches only a bare
  `Command::new("x")` literal.
- GAP-56 The license audit accepts `Unlicense`, matches license names by prefix (so
  `BSD-3-Clause OR GPL-2.0` passes) and lacks most ADR-24 licenses. The format gate skips
  `Boot/` though its message claims it, the Ravyn kexts, `Kernel/build` and `Tests`;
  clang-format is unpinned.
- GAP-55 Stage-1 is incomplete: `--verify` re-hashes what the same run wrote instead of
  rebuilding and comparing; the cache key omits the host compiler, cmake and ninja
  versions; no `-ffile-prefix-map`; llvm-objdump, llvm-readtapi, llvm-lipo, dsymutil,
  libclang, byacc, unifdef, migcom, rvpatch, mkfat and elf2pe are not built; no
  `Tools/toolchain/*/manifest.toml`; no tests; the scripts use host `sed` and `awk`;
  `.cargo/config.toml` forces `linker=clang` for every target and there is no musl
  target. mig: `--regen-lexer` uses wrong paths, `patches/series` is ignored, and the
  patches' `fate:` lines use values section 2.5 does not allow.
- GAP-11 The committed mig lexer was generated by flex, which runs GNU m4. `Tools/mig/build.sh`
  installs packages with `sudo apt-get`; `_PROVENANCE` names old paths.

**S03 SDK** — GAP-29: not started. No `VENDORS` file at the root (the old
`Legacy/VENDORS` was removed with WP0.0; no xnu pin).

**S04 bootloader** — BUG-02 to BUG-06, BUG-13, BUG-24 to BUG-26. The source uses tabs
against `.clang-format`. CI does not build the loader or run the
boot smokes. The arm64 smoke uses `cortex-a57`, which cannot run the RVVIRT target.

**S05 kernel build**
- GAP-12 Closed: `Kernel/build/` was removed with WP0.0; the kernel build outside
  `rv` no longer exists in the tree. Rebuilding it inside `rv` is the W1 kernel
  work (GAP-12's original scope moves to W1).
- GAP-13 Closed: the `Kernel/build/mig` copy was removed with WP0.0;
  `Tools/mig` (138) is the one the design keeps.
- GAP-14 The vendored XNU has been edited in place: 19 files, about 9,600 lines,
  including generated system-call tables and hand edits. These become patches and build
  outputs (ADR-10).
- GAP-15 No link: the link step resolves against a stub object; the listed undefined
  symbols cover one object only.
- GAP-16 `libfirehose_kernel` is not built; its source is libdispatch's firehose buffer
  (Apache-2.0), seeded in `old:Legacy/Libraries/libfirehose_kernel`. `libcc_kext` has no
  replacement; corecrypto is closed (ADR-12).
- GAP-17 Closed. The GRUB-based rig in `Kernel/build/boot/` was removed with
  `Kernel/build/` in WP0.0.

**S06 x86_64 platform**
- GAP-18 No platform expert, CPU, interrupt-controller or ACPI kexts are built.
- GAP-47 `IOACPIFamily` has no AML interpreter, prints `STUB` from unimplemented paths,
  and `startCPU` is a stub, so there is no SMP (BUG-14). Plan: uACPI (MIT) under our
  own platform expert, per S06.
- GAP-53 No PCIe ECAM support; configuration space is reached only through port I/O.

**S07 arm64 platform** — GAP-19: no RVVIRT board; `Kernel/port/arm64/` to be deleted
(BUG-07, BUG-16).

**S08 riscv64** — GAP-20: no build of `Kernel/port/riscv64/`; no LLVM triple patch
(BUG-15).

**S09 kernelcache** — GAP-21: no tool.

**S10 native drivers**
- GAP-22 No family or driver is built.
- GAP-43 Code that reported success it did not achieve: the IOUSBFamily reconstruction
  (transfers report the full length, device status always 0, pipe resets do nothing);
  `RavynXHCIUSBBus.cpp:64-80` (`UIMClosePipe`, `UIMAbortPipe`, `UIMClearPipeStall` empty
  and successful); `RavynXHCIMassStorageDisk.cpp:215-221` (`doSynchronize` sends no
  SYNCHRONIZE CACHE); `RavynAHCIDisk.cpp:154,167-170` (hard-coded 512-byte sectors).
  The files were removed with WP0.0; the rule stays in force for the native S10 stack.
- GAP-48 RavynXHCIPort and RavynAHCIPort bypass `IODMACommand`, wait with `IOSleep`
  loops, run one command at a time and have no power management. Both are rewritten to
  the S10 driver construction rules.
- GAP-46 `IOPCIFamily` contains patch files for GPL pciutils and an `iig_shims.h` that
  stands in for real `iig` output.
- GAP-52 Several kexts use `com.ravynos.*` bundle IDs where the macOS family's bundle ID
  is required for matching.

**S11 foreign drivers**
- GAP-23 Closed: superseded by ADR-14.
- GAP-24 FreeBSDPort (2/10): 29 functions unimplemented, `sysctl` handlers report
  success without acting (BUG-19, BUG-21); does not compile (BUG-11); builds against XNU private headers, so
  no stable interface is possible; no IOService class, `Info.plist` or build wiring; its
  own `mbuf` duplicates XNU's; its 95 files have no license header; no `LICENSE`.
  The copy in the tree was removed with WP0.0; the rewrite under S11 starts from
  history (`old:Kernel/Extensions/FreeBSDPort`), not from the tree.

**S12 code signing and policy** — GAP-51: `CoreEntitlements` is a forwarder with no
implementation. GAP-25: nothing parses or applies signatures; no
`codesign` tool. GAP-39: XNU panics during boot unless an `img4_interface_t` and the AMFI
trust-cache interface are registered before `trust_cache_runtime_init`
(`osfmk/kern/startup.c:867`, `libkern/img4/interface.c`, `bsd/kern/kern_trustcache.c`);
`TrustCache/API.h` is not in the tree.

**S13–S18 runtime and services** — GAP-50: `Kernel/IOKitUser/stubs/` held stub
functions, which the design forbids; `Kernel/IOKitUser/` was removed with WP0.0 and
the stubs must not come back. GAP-26: none built. Seeds are read from history
(`old:Legacy/…`): Libsystem (macOS 12 era), dyld-1066.8 with its cache builder, objc4,
CF (about 10.11 era), launchd-842, NextBSD libxpc (BSD-2), an os_log reimplementation
(BSD-2), configd, mDNSResponder,
notifyd. All need updating to current tags.

**S19 filesystems** — GAP-27: hfs and msdosfs vendored, unbuilt; no APFS (planned as
the Rust `apfs-core` crate). GAP-44: hfs was edited in place (BUG-28) and neither hfs
nor msdosfs records its Apple release tag.

**S22–S25 graphics and AppKit** — GAP-28: nothing exists yet. The old-tree seeds
(read from history) — AppKit, Foundation, CoreGraphics and QuartzCore — are
Cocotron-lineage (MIT), API about the 10.6 era; the old WindowServer targets FreeBSD
devices and is not reusable; the old `CoreGraphics/Makefile` linked with
`-undefined dynamic_lookup`.

**S20, S21, S26–S36** — GAP-29: not started. (GAP-29 is used for every subsystem that has
no code at all.)

**Repository-wide**
- GAP-60 Closed. The tree is now the section 3.2 layout (ADR-30): `Legacy/`,
  `Kernel/build/`, `Kernel/port/{arm64,riscv64}/`, `Kernel/kext_tools/`,
  `Kernel/IOKitUser/`, the unbuilt kexts in `Kernel/Extensions/`, the Makefile-era
  files in `Tools/` and `Tests/`, and test programs under `Apps/` are removed; the
  old tree is read only from history (`old:` paths, `git show 5112c72bc:<path>`).
  The hello programs moved to `Tests/apps/` and build there. This closes or moves
  GAP-17, GAP-30 (old-tree part), GAP-37, GAP-49 and the old-tree parts of GAP-42
  to GAP-48; those items apply to whatever is re-imported.
- GAP-34 x86_64: XNU panics on non-Intel processors and needs Intel timing data; QEMU
  smokes need an Intel CPU model, and AMD support needs the ADR-20 patch.
- GAP-35 XNU's own host tools (`SETUP/config`, `doconf`) need bison, flex and csh;
  ADR-21 replacements do not exist.
- GAP-36 `rv` rejects dependencies within one layer and knows only seven layers —
  `build-toolchain` and `test` are missing (`Tools/rv/src/graph.rs:5-13`); it checks
  `targets` on every manifest rather than the selected ones (`main.rs:139-148`);
  manifest discovery no longer skips the removed `Legacy/`; the manifest has no keys for
  output kind, versions, re-exports, install path, external source roots, build hooks or
  upward links (SYSTEM-DESIGN S01).
- GAP-37 Closed. `Kernel/build/mig/bin/migcom` was removed with `Kernel/build/` in
  WP0.0; `Tools/mig/_PROVENANCE` was removed with WP0.0 and provenance moves to
  VENDORS (GAP-58); the patches live in `Tools/mig/patches`.
- GAP-40 GitHub's default CI image ships bison, flex and m4; CI needs its own container
  (SYSTEM-DESIGN section 2.2).
- GAP-45 Files with no license header beyond GAP-42: the keyboard files of
  `ApplePS2Controller` (PureDarwin), all 95 files of FreeBSDPort. Without a stated
  license they are not used; they are rewritten (ADR-27).
- GAP-49 Closed. The `Legacy` kext build under `Kernel/Extensions` (Makefiles and
  scripts) was dead and built nothing; removed with WP0.0.
- GAP-57 SPDX: 173 of our source and build files have none: `RavynAHCIPort` and
  `RavynXHCIPort` (16 files, MIT text, not BSD-2-Clause), `Kernel/build` (22),
  `Tools/mig/include/mach` (5), the old `Tools/` scripts and `Tests/*.m`, every
  `manifest.toml`, the CI workflow, `.cargo/config.toml` and `Tools/rv/Cargo.toml`.
- GAP-58 Provenance: `Kernel/_PROVENANCE` has no tags or commits, lists
  `AppleI386GenericPlatform` and `AppleIOACPIFamily`, which are not in the tree, and
  omits IOHIDFamily, IOUSBFamily, hfs, msdosfs, AppleFileSystemDriver, AppleI386PCI,
  IOACPIFamily and IOKitUser. No vendored kext uses the `upstream/` plus `patches/`
  layout. `Kernel/build/mig` (375 files) and `Tools/mig/tests/mach` (22 files, including
  a CDDL `sdt.h` outside XNU DTrace, against ADR-22) are copied Apple trees with no
  provenance.
- GAP-59 Commit hygiene: no merge carries a review score (section 2.7); most subjects
  exceed 50 characters; scope prefixes mix `Docs:` and `docs:`. Stray files: `Tools/mig-mvtmp/.gitignore`,
  `Apps/.tmp_bad_manifest`.
- GAP-42 `Kernel/Extensions/IOUSBFamily/src/IOUSBController.cpp`, `IOUSBControllerV2.cpp`,
  `IOUSBDevice.cpp`, `IOUSBInterface.cpp` and `include/IOKit/usb/IOUSBController.h` are
  PureDarwin's reconstruction of Apple's closed classes and carry no license header.
  They are not built; they are replaced by a native USB host stack designed in S10
  (ADR-27).
- GAP-41 XNU's system-call and libsyscall generators use host sed, awk and Perl
  (`bsd/kern/makesyscalls.sh`, `libsyscall/xcodescripts/*.pl`); no Rust replacements
  exist.
- GAP-38 Seeds under APSL-1.1/1.2 (`IOACPIFamily`, `AppleAPIC`, `AppleI386PCI`, much of
  `IOPCIFamily`, parts of `hfs`) and CDDL DTrace files in XNU need ADR-22 confirmed.
- GAP-30 Closed. `Legacy/` — with its GPL/LGPL/AGPL material: autotools
  `config.guess`/`config.sub` and m4 files, GCC dump files under
  `Legacy/BSD/lib/ncurses/obj_s`, LGPL `dtrace_ctf`, NSS-derived tri-licensed files
  in Security, and the URW fonts — was removed with WP0.0. The remaining open GPL
  item is GAP-46 (`IOPCIFamily` pciutils patches). Vendored generated output still in
  the tree: `Kernel/xnu/libkern/c++/OSUnserialize.cpp` and the
  `OSUnserializeXML` headers, `Tools/mig/upstream/migcom.tproj/parser.c` (ADR-17
  requires regeneration with byacc); committed flex output in the mig copy.
- GAP-31 `Kernel/_PROVENANCE` described the wrong source for the vendored XNU;
  the file was removed with WP0.0 and provenance moves to VENDORS (GAP-58).
- GAP-32 Paths named in older documents that do not exist: `Docs/rfc/`,
  `.mlai/DECISIONS.md`, `Tests/perf/`, `Tests/kpi/`,
  `Tools/license-audit.py`. `Tests/apps/` now exists (WP0.0); top-level
  `Libraries/`, `CoreServices/`, `BSD/`, `SysApps/` were removed with `Legacy/`.
- GAP-33 CI runs only the x86_64 user-space build: no `cargo test`, clippy, arm64 or
  riscv64 builds, loader build, mig build, boot smokes, Mach-O assertion or negative
  tests. `Apps/.tmp_bad_manifest` was a stray file; removed with WP0.0.

## Exit criteria for W0 and W1

| Criterion (SYSTEM-DESIGN section 6) | State |
|---|---|
| W0: the tree holds only the section 3.2 layout | closed (GAP-60) |
| W0: `rv` builds the tree with no duplicate names | open (BUG-27) |
| W0: `rv` produces Mach-O for x86_64 and arm64 | open (GAP-01) |
| W0: stage-1 toolchain builds and `--verify` reproduces it | open (BUG-22, BUG-23, GAP-55) |
| W0: every merged component scores at least 8/10 in review | open (all areas in the summary) |
| W0: negative tests fail as designed in CI | open (GAP-33) |
| W0: audit and host-tool gate cover the tree | open (GAP-09, GAP-10) |
| W0: no GPL material in the repository | open (GAP-46) |
| W0: `VENDORS` at the root | open |
| W0: correct `_PROVENANCE` everywhere | open (GAP-31, GAP-58) |
| W0: SPDX on every file we own | open (GAP-45, GAP-57) |
| W0: every section 5.6 gate runs in CI | open (GAP-33, GAP-40) |
| W1: kernel links with no stubs, both arches | open (GAP-12–GAP-16, GAP-41) |
| W1: loader boots XNU to its banner, both arches | open (BUG-02–BUG-06, BUG-24–BUG-26, GAP-39) |
| W1: T0 passes on both arches | open (GAP-07) |
| W1: P2, P9, P10 recorded | open |
| W0: GRUB rig and `Kernel/build/` removed | closed (GAP-17) |
