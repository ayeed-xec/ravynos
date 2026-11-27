# Status

Where the code stands against [SYSTEM-DESIGN.md](SYSTEM-DESIGN.md), subsystem by
subsystem. This is the only document that records status; update it in the same commit
as the code it describes. Gaps are numbered GAP-01 onward and bugs BUG-01 onward; the
numbers are permanent, and a closed item is marked closed, not deleted.

## Summary

Both arm64 and x86_64 are primary (ADR-19). The build tool, the bootloader's UEFI
plumbing, the vendored mig and the CI license gates exist and are reasonably built. No Darwin program has run, the kernel does not
link, and nothing above the kernel exists outside `Legacy/`. Tiers T0–T5 are all open.

What exists, and whether it is a real implementation:

- `Tools/rv/` — real, small, well-structured Rust; three unit tests; several documented
  behaviors not yet implemented (GAP-01 to GAP-06).
- `Boot/loader/` — real UEFI code that boots ELF test kernels on three architectures;
  its Mach-O and x86 hand-off paths are broken (BUG-01 to BUG-04).
- `Boot/mkfat/`, `Boot/elf2pe/`, `Tools/mig/rvpatch.c` — real, with unit tests.
- `Tools/mig/` — Apple's migcom, pristine, with three patches, built with byacc.
- `Tools/ci/` — real gates with incomplete coverage (GAP-09, GAP-10).
- `Kernel/build/` — a separate kernel compile driver outside `rv`, with hand-written
  configuration; compiles files in batches; does not link (GAP-12 to GAP-15).
- `Kernel/port/arm64/` — stub functions that report success (BUG-07); obsolete under
  ADR-11.
- `Kernel/port/riscv64/` — real but small SBI, `satp` and trap code; built by nothing.
- `Kernel/Extensions/FreeBSDPort/` — a partial driver shim that does not compile against
  XNU and is not wired into any build (GAP-24).
- `Apps/hello`, `Apps/hello-cli` — test programs that make Linux system calls; neither is
  a Darwin program (GAP-07).
- `Legacy/` — upstream ravynOS; useful seeds listed per subsystem below; contains files
  that violate the license rule (GAP-30).

## Bugs

- **BUG-01** `Boot/loader/macho.c` reads `LC_SEGMENT_64` fields at wrong offsets: vmaddr
  at 16 (should be 24), file offset from the vmsize field, file size from the file-offset
  field, memory size from the file-size field.
- **BUG-02** `Boot/loader/macho.c` reads the x86 entry point from thread-state index 20
  (`gs`) instead of 16 (`rip`); `LC_MAIN` is defined as `0x28` instead of `0x80000028`
  and its `entryoff` (a file offset) is used as a virtual address; no bounds checks on
  segment offsets and sizes; `macho_phys` hard-codes the x86 kernel base, which is wrong
  for arm64 kernels.
- **BUG-03** `Boot/loader/handoff.S`: after the 64→32 switch it jumps to `eax`, which
  holds the `boot_args` address, not the kernel entry; data segments are not reloaded.
- **BUG-04** `Boot/loader/main.c`: the memory map in `boot_args` is captured before the
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
- **BUG-09** `rv` silently overwrites one manifest with another when two share a name
  (`Apps/hello` and `Apps/hello-cli` are both `hello`); the one built is `hello-cli`.
- **BUG-10** `rv shell` prints an environment bash rejects and points at a directory with
  no `build.ninja`.
- **BUG-11** `Kernel/Extensions/FreeBSDPort/shim` calls `ifnet_allocate_extended` with
  the wrong arguments for XNU's declaration.
- **BUG-12** `Boot/build.sh:113` names `loader/aarch64/efi-objlst.ld`, which does not
  exist, in a freshness test; harmless today, but a dead reference.
- **BUG-13** `Boot/loader/handoff.S` uses absolute 32-bit relocations, so the loader works
  only when the firmware places it below 4 GB; nothing checks this.

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

**S02 toolchain**
- GAP-07 No Darwin program exists; the test programs use Linux system calls.
- GAP-08 No stage-1 toolchain; CI uses the distribution's clang, unpinned; no
  `rust-toolchain.toml`; `cargo` not run with `--locked`; `rv` is dynamically linked.
- GAP-09 The license audit scans only the three manifested directories and only some file
  types; no crate scanning; no fingerprinting.
- GAP-10 The host-tool gate scans only some directories: it skips `Kernel/build/` (gcc,
  GNU make, GRUB) and `Tools/build.sh`, and its deny list lacks `flex` and `grub`.
- GAP-11 The committed mig lexer was generated by flex, which runs GNU m4. `Tools/mig/build.sh`
  installs packages with `sudo apt-get`; `_PROVENANCE` names old paths.

**S03 SDK** — GAP-29: not started. No `VENDORS` file at the root (only `Legacy/VENDORS`, no xnu
pin).

**S04 bootloader** — BUG-01 to BUG-06, BUG-12. CI does not build the loader or run the
boot smokes. The arm64 smoke uses `cortex-a57`, which cannot run the RVVIRT target.

**S05 kernel build**
- GAP-12 The kernel build is outside `rv`, with hand-written configuration in
  `Kernel/build/config-include/`.
- GAP-13 Two mig copies: `Kernel/build/mig` (bootstrap_cmds-99.231.1) produced the
  committed kernel mig output; `Tools/mig` (138) is the one the design keeps.
- GAP-14 The vendored XNU has been edited in place: 19 files, about 9,600 lines,
  including generated system-call tables and hand edits. These become patches and build
  outputs (ADR-10).
- GAP-15 No link: the link step resolves against a stub object; the listed undefined
  symbols cover one object only.
- GAP-16 `libfirehose_kernel` is not built; its source is libdispatch's firehose buffer
  (Apache-2.0), seeded in `Legacy/Libraries/libfirehose_kernel`. `libcc_kext` has no
  replacement; corecrypto is closed (ADR-12).
- GAP-17 The GRUB-based rig in `Kernel/build/boot/` remains.

**S06 x86_64 platform** — GAP-18: no platform expert, CPU, interrupt-controller or ACPI
kexts built; seeds in `Kernel/Extensions/` unaudited.

**S07 arm64 platform** — GAP-19: no RVVIRT board; `Kernel/port/arm64/` to be deleted.

**S08 riscv64** — GAP-20: no build of `Kernel/port/riscv64/`; no LLVM triple patch.

**S09 kernelcache** — GAP-21: no tool.

**S10 native drivers** — GAP-22: no family or driver is built.

**S11 foreign drivers**
- GAP-23 Closed: superseded by ADR-14.
- GAP-24 FreeBSDPort: does not compile (BUG-11); builds against XNU private headers, so
  no stable interface is possible; no IOService class, `Info.plist` or build wiring; its
  own `mbuf` duplicates XNU's; files lack SPDX headers; no `LICENSE`.

**S12 code signing and policy** — GAP-25: nothing parses or applies signatures; no
`codesign` tool. GAP-39: XNU panics during boot unless an `img4_interface_t` and the AMFI
trust-cache interface are registered before `trust_cache_runtime_init`
(`osfmk/kern/startup.c:867`, `libkern/img4/interface.c`, `bsd/kern/kern_trustcache.c`);
`TrustCache/API.h` is not in the tree.

**S13–S18 runtime and services** — GAP-26: none built. Seeds in `Legacy/`: Libsystem (macOS 12
era), dyld-1066.8 with its cache builder, objc4, CF (about 10.11 era), launchd-842,
NextBSD libxpc (BSD-2), an os_log reimplementation (BSD-2), configd, mDNSResponder,
notifyd. All need updating to current tags.

**S19 filesystems** — GAP-27: hfs and msdosfs vendored, unbuilt; no APFS.

**S22–S25 graphics and AppKit** — GAP-28: nothing outside `Legacy/`. `Legacy/`'s
AppKit, Foundation, CoreGraphics and QuartzCore are Cocotron-lineage (MIT), API about the
10.6 era; its WindowServer targets FreeBSD devices and is not reusable;
`Legacy/Frameworks/CoreGraphics/Makefile` links with `-undefined dynamic_lookup`.

**S20, S21, S26–S36** — GAP-29: not started. (GAP-29 is used for every subsystem that has
no code at all.)

**Repository-wide**
- GAP-34 x86_64: XNU panics on non-Intel processors and needs Intel timing data; QEMU
  smokes need an Intel CPU model, and AMD support needs the ADR-20 patch.
- GAP-35 XNU's own host tools (`SETUP/config`, `doconf`) need bison, flex and csh;
  ADR-21 replacements do not exist.
- GAP-36 `rv` rejects dependencies within one layer and knows only seven layers —
  `build-toolchain` and `test` are missing (`Tools/rv/src/graph.rs:5-13`); it checks
  `targets` on every manifest rather than the selected ones (`main.rs:139-148`); manifest
  discovery does not skip `Legacy/` (`manifest.rs:52-58`); the manifest has no keys for
  output kind, versions, re-exports, install path, external source roots, build hooks or
  upward links (SYSTEM-DESIGN S01).
- GAP-37 `Kernel/build/mig/bin/migcom` is a committed host binary built with GCC.
  `Tools/mig/_PROVENANCE` names `ravynos/build/mig/patches`; the patches are in
  `Tools/mig/patches`.
- GAP-40 GitHub's default CI image ships bison, flex and m4; CI needs its own container
  (SYSTEM-DESIGN section 2.2).
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
- GAP-30 `Legacy/` contains GPL/LGPL/AGPL material: autotools `config.guess`/`config.sub`
  and m4 files, GCC dump files under `Legacy/BSD/lib/ncurses/obj_s`, LGPL `dtrace_ctf`,
  NSS-derived tri-licensed files in Security, and URW fonts whose license must be
  checked. An `SDKSettings.json` that appears copied from Apple's SDK. The toolchain
  `_PROVENANCE` calls `dtrace_ctf` LGPL, but its files are CDDL and APSL (ADR-22).
  `Legacy/Libraries/_PROVENANCE` labels libdispatch APSL; it is Apache-2.0.
  `Legacy/Developer/Default.xctoolchain/iig/iig.cpp` has no license header.
  Vendored Bison output: `Kernel/xnu/libkern/c++/OSUnserialize.cpp` and the
  `OSUnserializeXML` headers, `Kernel/IOKitUser/IOCFUnserialize.tab.c`,
  `Tools/mig/upstream/migcom.tproj/parser.c` (ADR-17 requires regeneration with byacc);
  committed flex output in both mig copies.
- GAP-31 `Kernel/_PROVENANCE` describes the wrong source for the vendored XNU.
- GAP-32 Paths named in older documents that do not exist: `Docs/rfc/`,
  `.mlai/DECISIONS.md`, `Tests/perf/`, `Tests/apps/`, `Tests/kpi/`,
  `Tools/license-audit.py`, and top-level `Libraries/`, `CoreServices/`, `BSD/`,
  `SysApps/` (now under `Legacy/`).
- GAP-33 CI runs only the x86_64 user-space build: no `cargo test`, clippy, arm64 or
  riscv64 builds, loader build, mig build, boot smokes, Mach-O assertion or negative
  tests. `Apps/.tmp_bad_manifest` is a stray file.

## Exit criteria for W0 and W1

| Criterion (SYSTEM-DESIGN section 6) | State |
|---|---|
| W0: `rv` produces Mach-O for x86_64 and arm64 | open (GAP-01) |
| W0: negative tests fail as designed in CI | open (GAP-33) |
| W0: audit and host-tool gate cover the tree | open (GAP-09, GAP-10) |
| W0: no GPL material in the repository | open (GAP-30) |
| W0: `VENDORS` at the root | open |
| W0: correct `_PROVENANCE` everywhere | open (GAP-31, GAP-37) |
| W0: every section 5.6 gate runs in CI | open (GAP-33, GAP-40) |
| W1: kernel links with no stubs, both arches | open (GAP-12–GAP-16, GAP-41) |
| W1: loader boots XNU to its banner, both arches | open (BUG-01–BUG-06, GAP-39) |
| W1: T0 passes on both arches | open (GAP-07) |
| W1: P2, P9, P10 recorded | open |
| W1: GRUB rig and `Kernel/build/` removed | open (GAP-17) |
