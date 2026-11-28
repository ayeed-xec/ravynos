# Status

Where the code stands against [SYSTEM-DESIGN.md](SYSTEM-DESIGN.md), subsystem by
subsystem. This is the only document that records status; update it in the same commit
as the code it describes. Gaps are numbered GAP-01 onward and bugs BUG-01 onward; the
numbers are permanent, and a closed item is marked closed, not deleted.

## Summary

Every score below was given on 2026-11-28 by a reviewer agent that did not write the
code it scored, against the section 2.7 gate (correctness, tests, design fit,
readability, hygiene, 0–2 each; 8/10 to pass, no criterion at 0). Earlier scores in
this file were given by the agents that wrote the code; they are withdrawn. The
`Boot/loader` score of 6/10, for example, followed a 2/10 with no change under `Boot/`.

No area passes the gate. The tree builds Mach-O for x86_64 and arm64 with the macOS 27.2
target, `libhello` links as `MH_DYLIB`, the build tools are a reproducible stage-1
component, and CI runs every gate that exists, the four boot smokes and the Mach-O
assertion, all inside the pinned gate image. Some section 5.6 checks do not exist yet
(10 of the 12 S01 negative cases, GAP-65; the mig golden-output comparison). What keeps
each area below 8 is listed in its row.

| Area | Score | Reviewed by | Verdict |
|---|---|---|---|
| `Tools/rv` (WP0.1–WP0.3) | 4/10 | independent reviewer agent, at `aba318d45b` | Mach-O for both arches, schema, layer and cycle checks work; missing: the include scan, `E-LICENSE`/`E-NO-SPDX`/`E-FORMAT`, hook execution, sysroot staging, per-target `flags`, `source_root`, four S01 commands, and 10 of the 12 S01 negative cases (GAP-65); `rv shell` (BUG-10) and `rv sdk` (BUG-31) are broken |
| Stage-1 bootstrap (WP0.4) | 5/10 | independent reviewer agent, at `aba318d45b` | the build tools build and `--verify` reproduces them in CI; the LLVM component has no completed CI run, lacks libclang and `llvm-readtapi` (GAP-61), does not run in the gate image, and CI does not build with stage 1 (GAP-40) |
| `Boot/loader` (WP0.9) | 3/10 | independent reviewer agent, at `5b77e75b04` | the x86_64 Mach-O path boots an identity-linked `LC_MAIN` test kernel and the four smokes are strict (BUG-26 closed); no device tree, `Revision` 0, no `SetVirtualAddressMap`, x86-only `macho_phys`, the arm64 hand-off passes x86 `boot_args` with the MMU on (BUG-02 to BUG-06, BUG-13, BUG-24, BUG-25, BUG-30); every loader `.c` file fails clang-format and `Boot/` is outside the format gate |
| `Tools/mig` | 6/10 | independent reviewer agent, at `aba318d45b` | pristine tree plus the rvpatch series, byacc parser and hand-written lexer build and pass the smoke; no golden-output test, and `-arch` is dropped so a cross-arch run silently targets the host (BUG-32) |
| `Tools/xnuhook` (parser step) | 7/10 | independent reviewer agent, at `aba318d45b` | deterministic contract-v1 plan for the two byacc parsers with real-byacc tests; the output is never compiled, rv never runs the hook, and the pruned list repeats `VENDORS` |
| CI and gates | 4/10 | independent reviewer agent, at `aba318d45b` | scored before every gate moved into the pinned image (GAP-40); open then and now: the host-tool deny list lacks `flex`, `sed`, `awk` and `perl` (GAP-10), 10 of the 12 S01 negative cases and the mig golden test do not exist, and nothing builds with stage 1 |
| FreeBSDPort | — | — | removed from the tree (WP0.0); rewrite under S11 (BUG-11, BUG-19, GAP-24) |
| IOUSBFamily reconstruction | — | — | deleted (WP0.0); native stack in S10 (GAP-42, GAP-43) |
| RavynXHCIPort | — | — | removed from the tree (WP0.0); rewrite when S10 starts (BUG-17, GAP-48) |
| RavynAHCIPort | — | — | removed from the tree (WP0.0); rewrite when S10 starts (GAP-48) |
| AppleAPIC | — | — | removed from the tree (WP0.0); re-vendor, then patch (BUG-20) |
| ApplePS2Controller | — | — | removed from the tree (WP0.0); re-vendor controller, rewrite keyboard files (GAP-45) |
| IOACPIFamily (PureDarwin edits) | — | — | removed from the tree (WP0.0); re-vendor, then patch (BUG-14, GAP-47) |
| hfs | — | — | removed from the tree (WP0.0); re-vendor from the tagged release (GAP-44) |

The W0 wave report claimed WP0.1–WP0.4 and WP0.9 as delivered. They are merged but not
done: each is open below until it meets its "Done when" and passes review.

Both arm64 and x86_64 are primary (ADR-19). No Darwin program has run (GAP-07), the
kernel does not link (GAP-15), and nothing above the kernel exists yet. Tiers T0–T5 are
all open.

What exists, and whether it is a real implementation:

- `Tools/rv/` — real Rust with unit tests run by `cargo test` in CI; much of S01 is
  not implemented yet (see its row above and GAP-65).
- `Tools/toolchain/` — the two stage-1 components, LLVM and the build tools, each with
  its own cache key and a rebuild-and-compare `--verify` (GAP-55).
- `Tools/xnuhook/` — the S05 hook, planning only the byacc parsers so far.
- `Boot/loader/` — real UEFI code that boots ELF test kernels on three architectures
  and a Mach-O test kernel on x86_64; the XNU hand-off is not there (BUG-02 to BUG-06).
- `Boot/mkfat/`, `Boot/elf2pe/`, `Tools/mig/rvpatch.c` — real, with unit tests, built as
  stage-1 tools.
- `Tools/mig/` — Apple's migcom, pristine, with three patches, built with the stage-1
  byacc.
- `Tools/ci/` — the gates, the Mach-O assertion and the pinned gate image
  (`Containerfile`); coverage gaps in GAP-10 and GAP-40.
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
- **BUG-07** Moot in the tree: `Kernel/port/arm64/` was removed with WP0.0, and the
  rule stands for its rewrite (S07). `Kernel/port/arm64/pmap.c` consisted of functions
  that ignored their arguments and returned success — stubs, which the design forbids.
- **BUG-08** Moot in the tree: `Kernel/build/` was removed with WP0.0; the W1 kernel
  build in `Tools/xnuhook` must not repeat it. `Kernel/build/driver/compile.sh` defined
  `__i386__` for x86_64, selecting 32-bit paths in XNU headers.
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
- **BUG-15** Moot in the tree: `Kernel/port/riscv64/` was removed with WP0.0; it applies
  to the S08 rewrite. `Kernel/port/riscv64/trap.S` adjusted `sp` by 256 bytes on entry and
  did not restore it on every exit path; each trap leaked 256 bytes of stack.
- **BUG-16** Moot in the tree: `Kernel/port/arm64/` was removed with WP0.0. It
  referenced `arm64_exception_entry`, which nothing defined.
- **BUG-17** `RavynXHCIPort::stop` frees ring and context memory before halting the
  controller; the controller can DMA into freed memory, and objects are used after
  release.
- **BUG-18** RavynXHCIPort and RavynAHCIPort call `IOMemoryMap::read32`, which does not
  exist in XNU's `IOMemoryMap`.
- **BUG-19** FreeBSDPort's `ticks` never advances, so every timeout in the shim either
  fires at once or never.
- **BUG-20** `AppleAPIC` (8259 path) calls `kprintf` on every interrupt.
- **BUG-21** FreeBSDPort calls `va_start` with its arguments reversed.
- BUG-22 Fixed: `Tools/toolchain/llvm/build.sh:62` uses `-G Ninja` (generator names are
  case-sensitive); the `byacc` requirement is gone because the LLVM build never runs a
  parser generator; stripping uses the `llvm-strip` the build just produced instead of
  whatever is on `PATH`.
- BUG-23 Fixed: the pin extraction is `sed -n 's/^\* \(Tag\|Commit\): /  \1: /p'`
  (`Tools/toolchain/stage1.sh:111`), which now yields both lines of `_PROVENANCE`.
- **BUG-24** `Boot/testkernel`: the stack pointer is set to the bottom of `__stack`
  instead of the top. Fixed in the three ELF start files; still open in the Mach-O one:
  `start-macho.S:60` defines `rv_stack_top` with `.zerofill __DATA,__stack,rv_stack_top,…`,
  which names the section start, so `%rsp` starts at `0x406000` and the stack grows down
  into `__TEXT`. The smoke passes only because the call chain is shallow (independent
  Boot/ review, `darwin-clean` at `5b77e75b04`).
- **BUG-25** `Boot/loader`: loader data the kernel reads is allocated as
  `EfiRuntimeServicesData` outside the range `ksize` covers; `SetVirtualAddressMap` is
  never called; `boot_args` `Revision` is 0 and the command line is the placeholder
  `rvynos`.
- BUG-26 Closed. The boot smokes passed on any panic block, so a test kernel that
  panicked on bad `boot_args` was reported as a success: with the version check
  forced to fail, `Boot/run.sh` printed "structured panic block captured" and
  exited 0 on a log reading `RVPSTR bootargs-version-bad`. `Boot/expect-probe.sh`
  now requires exactly the lines `RVTEST boot`, `RVTEST handoff-ok`, one panic
  block and `RVPSTR probe-reached`, in that order, and all three runners use it;
  the same forced failure now exits 1. Twelve fake-log cases in
  `Tests/tools/run.sh` cover it (wrong reason, no hand-off, two panics,
  truncated, reversed, CRLF, firmware escapes). The ESP-path half did not hold
  for the current tree: `Boot/image.sh` writes `KERNEL.ELF` at the ESP root and
  the loader looks for `\kernel.elf` (`Boot/loader/main.c:208`), the same name on
  FAT, and every smoke reaches `RVTEST handoff-ok`, which needs the loaded
  kernel. Evidence: all four smokes green under the new check in
  https://github.com/ayeed-xec/ravynos/actions/runs/46135899021 (job "boot smokes
  (QEMU TCG)", each step printing "hand-off validated, deliberate probe panic
  captured").
- BUG-27 Closed. A manifest `name` must be one path component (no `/`, `\`, `..` or
  leading `.`), so it can no longer resolve outside `build/`; manifest discovery skips
  symbolic links; duplicate names are covered by a negative fixture. `kind` decides
  the output: `Foundation/libhello` (`kind = "dylib"`) links as `MH_DYLIB` and CI
  asserts it. No library code calls `process::exit` any more (`rv bootstrap` returned
  its usage error through `exit` until wp/gap55-small-tools); the remaining `exit`
  calls are in `main.rs`, the binary's own entry point.
- **BUG-28** `Kernel/Extensions/hfs/core/BTreeMiscOps.c:181-201` was edited in place to
  relax `VerifyHeader`, so corrupt B-tree headers are accepted.
- BUG-29 Closed. The stage-1 `ld64.lld` built from `swiftlang/llvm-project` at
  `swift-6.4.3-RELEASE` refused every Mach-O input, so stage 1 could not link a
  ravynOS target; fixed by ADR-31 (details under "Gap closure after P4"). This
  entry was also numbered BUG-28 until the numbers were made unique; BUG-28 is
  the hfs edit.
- **BUG-30** `Boot/testkernel/kernel.c:51-53` reads a memory descriptor's
  NumberOfPages from bytes 16–23, which is `VirtualStart`; the field is at offset 24
  (`Boot/loader/efi.h:37-44`). Every smoke log prints `conventional_pages=0x00000000`
  and nothing checks it (independent Boot/ review, `5b77e75b04`).
- **BUG-31** `rv sdk` looks for `lib<name>.dylib`, so for component `libhello` it looks
  for `liblibhello.dylib` and installs no library (`Tools/rv/src/main.rs:430`;
  independent area review, `aba318d45b`).
- **BUG-32** `Tools/mig/patches/03-mig-sh-linux-port.patch` drops `-arch` from the
  preprocessor line and takes the arch from `uname -m`, so `mig -arch arm64` on an x86_64
  host silently generates host-arch stubs (independent area review, `aba318d45b`).

## Gaps by subsystem

**S01 build harness** — the entries below were written at `b1746b5ce`; where the code
has since changed, the entry says so and points at the evidence.
- GAP-01 Closed. rv emits Mach-O for `x86_64-ravynos` and `arm64-ravynos` with the
  ADR-03 triple `<arch>-apple-macosx27.2`, `-platform_version macos 27.2 27.2` and
  chained fixups (`Tools/rv/src/ninja.rs`, unit test
  `darwin_targets_use_the_adr03_triple_and_os_version`); CI checks every linked
  output with `Tools/ci/check-macho.sh` (magic, CPU, file type, `LC_BUILD_VERSION`
  platform macos, minos 27.2). It emitted `--target=<arch>-apple-macos11` and
  `macos 11.0 11.0` until this change, against ADR-01 and the S01 target table.
  Evidence: https://github.com/ayeed-xec/ravynos/actions/runs/46139710547, step "Linked outputs pass the S02 Mach-O
  assertion" in "rv gates" and the same check inside the pinned image.
- GAP-02 Closed (see "Gap closure after P4").
- GAP-03 Closed (see "Gap closure after P4").
- GAP-04 Closed (see "Gap closure after P4").
- GAP-05 Closed. Every manifest table is `deny_unknown_fields`; `state` is checked
  against `working | incomplete | experimental` and `vendored` is parsed. The
  `unknown-key` case in `Tests/rv/negative/run.sh` fails with `E-UNKNOWN-KEY`.
- GAP-06 Partly closed (see "Gap closure after P4"). Still open: `rv graph --forbid`
  is accepted and ignored (`Tools/rv/src/main.rs`, `"--forbid" => {}`).
- GAP-54 Closed (see "Gap closure after P4").
- GAP-65 S01 is far from done (independent area review, `aba318d45b`). Missing: the
  include scan (`E-HOST-HEADER`, `E-UNDECLARED-DEP`), the `E-LICENSE`, `E-NO-SPDX` and
  `E-FORMAT` codes, license fingerprinting of vendored trees, the Mach-O assertion inside
  rv after each link, hook execution (`E-HOOK-DETERMINISM`), `flags.rs` and the
  per-target `flags` key, `source_root`, the `host` target and cargo host tools, `.defs`,
  `.y` and `.iig` rules, staging into `build/<target>/sysroot/`, the commands
  `rv build xnu`, `rv build sdk`, `rv vendors import` and `rv graph <name>`, and 10 of the
  12 negative cases S01 lists (only `dup-name` and `unknown-key` exist).
- S01 done-when 3 (no gap number; it was never filed): `rv build -t x86_64-ravynos libhello` and
  `-t arm64-ravynos` must produce `libhello.dylib` of file type `MH_DYLIB`.
  `Foundation/libhello` is now `kind = "dylib"` with install name
  `/usr/lib/libhello.dylib`, `hello-cli` links it, and CI asserts `MH_DYLIB` on both
  arches (https://github.com/ayeed-xec/ravynos/actions/runs/46139710547). libhello and hello-cli dropped `riscv64-ravynos`: a riscv64 dylib needs
  the Mach-O target of ADR-03. Also fixed: `rv build` checked `targets` on every
  manifest instead of the selected ones (S01); unit tests in `Tools/rv/src/main.rs`.

**S02 toolchain**
- GAP-07 No Darwin program exists. `Tests/apps/hello` and `Tests/apps/hello-cli` link
  as Mach-O for macOS 27.2, but they write and exit through Linux system calls (`syscall`
  1/60 on x86_64, `svc 0` 64/93 on arm64), so they are file-format fixtures for the
  Mach-O assertion and not Darwin programs. `libhello` makes no system call. The first
  Darwin program is the T0 program of W1.
- GAP-08 Partly closed. `rust-toolchain.toml` pins Rust 1.97.0 with rustfmt and clippy,
  and CI calls cargo with `--locked`. Still open: the gates build with the gate image's
  pinned clang-18, not yet with stage 1 (GAP-40 step 2); `rv` is dynamically linked against glibc
  (no musl target); `.cargo/config.toml` forces `linker=clang` for every target.
- GAP-09 Closed for licensing: the audit now walks the whole tree instead of only the
  manifested directories. A file is accepted when it carries an SPDX header, sits in a
  tree pinned by `VENDORS` (whose entry states the license), or is a generated or tool
  build output. Still open: crate scanning and fingerprinting.
- GAP-10 Partly closed: the gate now scans every tracked `*.sh`, `*.yml` and `*.rs`
  file. Still open (independent area review, `aba318d45b`): the deny list lacks `flex`,
  `lex`, `sed`, `awk` and `perl`, so the stage-1 key scripts' `awk` passes (GAP-63);
  the Containerfile and other build files are not scanned; `cmake` and `python3` are not
  confined to the llvm and swift components; there are no planted-failure tests.
  Original report: the host-tool gate scans only some directories: it skips `Tools/toolchain/`,
  `Kernel/build/` (gcc, `-lgcc`, `grub-file`), `Kernel/Extensions/**/*.sh`,
  `Tools/build.sh` and every Makefile; its deny list lacks `flex`, `grub`, `sed`, `awk`,
  `perl` and `apt-get`, and has no rule confining `cmake` and `python3` to the llvm and
  swift components. With the full list, 26 files fail. In Rust it catches only a bare
  `Command::new("x")` literal.
- GAP-56 The license audit accepts `Unlicense`, matches license names by prefix (so
  `BSD-3-Clause OR GPL-2.0` passes) and lacks most ADR-24 licenses. The format gate skips
  `Boot/` though its message claims it, the Ravyn kexts, `Kernel/build` and `Tests`;
  clang-format is unpinned.
- GAP-55 Open until CI shows it (owner decision: it closes only on a green run of
  `rv bootstrap` and `rv bootstrap --verify` in CI). What its first report listed is
  now done, with these checks:
  - `--verify` rebuilds into a scratch directory and diffs `tools.sha256` against
    the recorded list, for both components (`stage1.sh`, `buildtools.sh`); host-tool
    tests tamper with the recorded hashes and with an installed tool and expect a
    refusal.
  - The LLVM key covers the host cmake, ninja and clang versions (`llvm/key.sh`);
    `-ffile-prefix-map` is set; `llvm-objdump`, `llvm-lipo` and `dsymutil` are built
    (key `04c9139e…`, verified end to end, entry below).
  - byacc, unifdef, rvpatch, mkfat, elf2pe, mig and migcom are a separate stage-1
    component, `Tools/toolchain/buildtools.sh`, with its own key: byacc and unifdef
    are fetched at their `VENDORS` pins and compiled with clang alone. Host-tool
    tests change each key's inputs and check that only that key moves. The rv gates
    job builds the component (or restores it from the cache) and puts its `bin` on
    PATH, and checks that `byacc`, `unifdef`, `rvpatch` and `mig` resolve there;
    that step is what turned the red gates job green
    (https://github.com/ayeed-xec/ravynos/actions/runs/46137001656, job "rv gates":
    the four gen-parsers cases that failed with "byacc is required" pass).
  - mig: `--regen-lexer` is gone, `patches/series` is the order the patches apply
    in, and the `fate:` lines use the section 2.5 values.
  - CI runs `rv bootstrap <component>` and `rv bootstrap --verify <component>` in
    `stage1-llvm.yml` and `stage1-buildtools.yml`, each cached under its own key.
  The run that closes this gap is not recorded yet. Split out of GAP-55: GAP-61,
  GAP-62, GAP-63, GAP-64; the musl and `.cargo/config.toml` items are under GAP-08.
- GAP-61 libclang and `llvm-readtapi` are not in the LLVM component. They are added
  at the component's next rebuild, before W1; the owner decided not to rebuild LLVM
  for them alone.
- GAP-62 The stage-1 components have no `Tools/toolchain/<tool>/manifest.toml`; rv
  does not build host tools from manifests yet, so `stage1.sh` and `buildtools.sh`
  stand in for it (S02 Location).
- GAP-63 The stage-1 scripts use host `sed`, `awk`, `find` and `xargs` to read pins
  and compute keys. None of their output ships, but section 2.2 lists GNU `sed` and
  `awk` as not allowed in build tooling.
- GAP-64 GitHub runs scheduled workflows only on the default branch, `darwin`, and
  serves a branch only its own caches and the default branch's. Until the stage-1
  workflows are on `darwin`, the weekly verify does not run and each branch builds
  LLVM from a cold cache once.
  `stage1-buildtools.yml` runs inside the pinned gate image; `stage1-llvm.yml` still
  runs on the bare `ubuntu-latest` runner with its apt cmake, ninja and clang (S02
  done-when a), so a runner-image update that changes one of them moves the LLVM key
  and costs a full rebuild. It moves into the image with the next LLVM build (GAP-40
  step 3).
- GAP-11 Closed. The mig lexer is hand-written (`Tools/mig/generated/lexxer.c`; GAP-46),
  and `Tools/mig/build.sh` no longer installs anything: it takes byacc and rvpatch
  from the stage-1 build tools and stops with "build the stage-1 tools" without
  them. CI builds mig and runs its smoke that way
  (https://github.com/ayeed-xec/ravynos/actions/runs/46137001656, step "mig builds
  from the pinned tree plus its patches"). The smoke also fails now on a
  preprocessor error, which it used to pass over (`tests/trivial.defs` had a `#`
  SPDX line).

**S03 SDK** — GAP-29: not started. (`VENDORS` exists at the root and pins xnu; see
GAP-31.)

**S04 bootloader** — BUG-02 to BUG-06, BUG-13, BUG-24, BUG-25. The source uses tabs
against `.clang-format`. CI builds the loader and runs the x86_64, Mach-O, arm64 and
riscv64 smokes inside the pinned gate image
(https://github.com/ayeed-xec/ravynos/actions/runs/46141609223, job "boot smokes"). The arm64 smoke uses
`cortex-a57`, which cannot run the RVVIRT target.

**S05 kernel build**
- GAP-12 Closed: `Kernel/build/` was removed with WP0.0; the kernel build outside
  `rv` no longer exists in the tree. Rebuilding it inside `rv` is the W1 kernel
  work (GAP-12's original scope moves to W1). Commit `0000cf5991` recreated
  `Kernel/build/` for `gen-parsers.sh`, against ADR-30; that script moved into
  `Tools/xnuhook` and the directory is gone again.
- GAP-13 Closed: the `Kernel/build/mig` copy was removed with WP0.0;
  `Tools/mig` (138) is the one the design keeps.
- GAP-14 Closed: `Kernel/xnu` is byte-identical to tag `xnu-12377.121.6`
  (tree `cba06fba7c`); the 11 hand edits were reverted, the 6 generated system-call
  tables and headers were deleted, and the 2 `CoreEntitlements/V2` forwarders amfi
  needs became `Kernel/patches/entitlements-v2-headers/0001-*.patch`. The mig copy's
  committed `parser.c` and `lexxer.c` were removed the same way. Pins live in
  `VENDORS`; `rv vendors check` fails CI on any drift (ADR-10).
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
- GAP-46 GPL-derived output is pruned, not shipped. `parser.c` and `lexxer.c`
  were removed from `Tools/mig/upstream/migcom.tproj/`; `VENDORS` lists both
  under `prune`, so `rv vendors check` fails if either reappears. The build
  parses with byacc and uses the hand-written scanner at
  `Tools/mig/generated/lexxer.c`; no flex or bison output remains in mig.
  Closed for XNU the same way: the three Bison-generated files
  (`libkern/c++/OSUnserialize.cpp` and both
  `OSUnserializeXMLSharedImplementation.h` copies) are removed from the tree,
  listed under `VENDORS.prune`, and the pin is updated to the pruned tree
  (`c35cb043…`). The `xnu` build hook, `Tools/xnuhook`, plans their regeneration
  with byacc (`-p OSUnserialize`, `-p OSUnserializeXML`) and refuses to plan if a
  pruned copy is back; the W1 kernel build compiles that output. An earlier
  `Kernel/build/gen-parsers.sh` ran `byacc -P` on both grammars with no prefix,
  which made `OSUnserialize.y` reentrant against its own `yylex()` and gave both
  parsers the same `yyparse`; it is gone. The `IOPCIFamily` pciutils patches
  and `iig_shims.h` named in older text are no longer in the tree.
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
- GAP-36 Closed (see "Gap closure after P4"; the `targets` part in the S01 done-when 3
  note above). Original report: `rv` rejects dependencies within one layer and knows only seven layers —
  `build-toolchain` and `test` are missing (`Tools/rv/src/graph.rs:5-13`); it checks
  `targets` on every manifest rather than the selected ones (`main.rs:139-148`);
  manifest discovery no longer skips the removed `Legacy/`; the manifest has no keys for
  output kind, versions, re-exports, install path, external source roots, build hooks or
  upward links (SYSTEM-DESIGN S01).
- GAP-37 Closed. `Kernel/build/mig/bin/migcom` was removed with `Kernel/build/` in
  WP0.0; `Tools/mig/_PROVENANCE` was removed with WP0.0 and provenance moves to
  VENDORS (GAP-58); the patches live in `Tools/mig/patches`.
- GAP-40 Partly closed. GitHub's runner image ships bison, flex and m4, so CI needs its
  own container (SYSTEM-DESIGN 2.2, 5.6). `Tools/ci/Containerfile` is that container:
  base `ubuntu:24.04` pinned by digest; apt reads only the 2026-11-27 archive snapshot,
  pinned above anything installed, and the image build fails if any installed package
  differs from the snapshot (ca-certificates and openssl are bootstrapped from the live
  archive and then brought back to their snapshot versions); Rust 1.97.0 from a
  checksum-pinned rustup with auto-install off, so a `rust-toolchain.toml` that names
  another version fails the gate; and the build fails if bison, flex or m4 is present.
  Every CI gate runs inside it: `Tools/ci/gates.sh` (the stage-1 build tools, lint,
  host-tool, SPDX, format and commit-subject gates over every commit of the push,
  `Tests/tools/run.sh`, rv's and xnuhook's tests and clippy, `rv test`, the exit-3
  checks, `rv vendors check`, `rv build` for both Darwin targets with the Mach-O
  assertion, `rv audit license`, the negative fixtures and the mig smoke), the four
  boot smokes, and `stage1-buildtools.yml`. No CI job installs from the runner's apt
  except `stage1-llvm.yml`.
  Still open, in order:
  1. GNU binutils is in the image as a dependency of clang-18 and libc6-dev. Nothing
     runs it (the host-tool gate), but section 2.2 says the image holds none of the
     disallowed tools; it goes when the image's clang comes from stage 1 (step 2).
  2. After the next LLVM build (GAP-61: libclang, `llvm-readtapi`) is cached on the
     default branch (GAP-64): run `stage1-llvm.yml` in the image (S02 done-when a), put
     the stage-1 `bin` first on PATH in `gates.sh` and the smokes so rv and the test
     kernels build with stage-1 clang and `ld64.lld`, and drop clang-18 from the image.
  3. Link rv statically against musl (GAP-08).
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
  in Security, and the URW fonts — was removed with WP0.0. GAP-46, the last GPL
  item, is now closed: the migcom parser and scanner and XNU's two Bison outputs
  are pruned from the vendored trees, listed under `VENDORS.prune`, and
  regenerated with byacc by the build.
- GAP-31 Closed for xnu: `VENDORS` records the upstream, tag, commit and tree for it
  and for migcom, and `rv vendors check` verifies both against git in CI. `_PROVENANCE`
  files for other trees still need the same treatment (GAP-58).
- GAP-32 Paths named in older documents that did not exist: `Docs/rfc/`,
  `.mlai/DECISIONS.md`, `Tools/license-audit.py`. The legacy `Docs/DESIGN.md` now
  points at `Docs/design/`, `Docs/design/DECISIONS.md`, `Tools/ci/check-spdx.sh` and
  `rv audit licenses`, and `Tests/tools/run.sh` fails if any of the three returns.
  `Tests/perf/` and `Tests/kpi/` are referenced only as planned W2 paths. `Tests/apps/`
  exists (WP0.0); top-level `Libraries/`, `CoreServices/`, `BSD/`, `SysApps/` were
  removed with `Legacy/`.
- GAP-33 Closed. CI runs `cargo test`, clippy with `-D warnings`, `rv vendors check`,
  the x86_64 and arm64 builds, the Mach-O assertion, the negative fixtures, the mig
  build and smoke, the loader build and all four boot smokes (`.github/workflows/ci.yml`).
  riscv64 user space is not built (ADR-03).

## Exit criteria for W0 and W1

### W0 phase plan

The remaining W0 work runs in four phases on parallel worktrees, merged in order.
Every phase branch must pass the full gate suite (format, host-tool, cargo test,
clippy, vendors, negative fixtures) and score 8/10 before it merges.

- **P1 — rv core (worktree wt-B, branch `wp/rv`)**: GAP-01 — emit Mach-O for
  x86_64 and arm64 with the existing ELF path kept for riscv64; honour `kind`
  (dylib builds a dylib, not an `ar` archive); replace `process::exit`/`unwrap`
  in library code with error returns (BUG-27 remainder, GAP-02–06).
- **P2 — stage-1 toolchain (wt-A, `wp/toolchain`)**: run the stage-1 build to
  completion, make `--verify` rebuild from source and compare hashes instead of
  trusting its own output (GAP-55), and record the cache layout.
- **P3 — gates and provenance (wt-C, `wp/gates-ci`)**: host-tool gate covers
  `Tools/toolchain/` and every script (GAP-10); the §5.6 gate list runs in CI
  (GAP-40); SPDX sweep over the remaining files (GAP-45/57); `_PROVENANCE` for
  the remaining trees (GAP-58).
- **P4 — re-audit (lead)**: after P1–P3 merge, rescore every area in the summary
  table against the 8/10 bar and update STATUS.md.

P1–P3 are merged (`f9115e8b0a`, `b8a715c9ec`, `93a3f2213b` + `cddbe0a56c`, merged as
`f2cdb4d701` and `43757342b4`) and P4 updated the summary table above. What those
phases did not finish is named in the exit criteria: CI still runs on the runner image
rather than the `Tools/ci/Containerfile` build (GAP-40). The stage-1 reproducible
rebuild, which P2 left unproven, was observed end to end afterwards under cache key
`af8a5c44…` (GAP-55 entry below).

### Gap closure after P4

The entries above record the tree at `b1746b5ce`. What changed since, with the check
that backs it:

- GAP-46 **closed**: XNU's `libkern/c++/OSUnserialize.cpp` and both
  `OSUnserializeXMLSharedImplementation.h` copies are removed and listed under
  `VENDORS.prune`; the pin is the pruned tree `c35cb043…`; `Tools/xnuhook` plans
  their byacc regeneration and its `cargo test` runs the plan with the stage-1 byacc
  (prefixes, which parser is reentrant, determinism, refused requests).
- GAP-02 **closed**: compile rules carry `-MMD -MF $out.d` with `depfile`/`deps = gcc`;
  touching `Foundation/libhello/include/hello.h` recompiles and relinks.
- GAP-03 **closed**: `.c`, `.m`, `.mm`, `.cpp`/`.cc`/`.cxx` and `.S`/`.s` have rules,
  and each kind links where its layout puts it: `X.framework/Versions/<v>/X` with
  `Versions/Current` symlinks and an `@rpath` install name; `<b>.app`, `<b>.xpc`,
  `<b>.dext`, `<b>.plugin`, `<b>.bundle` and `<b>.kext` under `Contents/MacOS/`, with
  `Info.plist` copied in. A kext links with Apple `ld64 -kext` because `ld64.lld` has no
  kext mode. Eleven unit tests in `Tools/rv/src/ninja.rs`.
- GAP-04 **closed**: compile and link carry `-isysroot ../sysroot/<target>`, links add
  `-L ../sysroot/<target>/usr/lib`, and a dependent links a producer's exported `.tbd`
  stub as `-l<name>`, or the built artifact by path when there is no stub (unit-tested).
  `rv build` succeeds for x86_64 and arm64. `riscv64-ravynos` is refused with
  `E-NOT-BUILT S08` (exit 3) until the ADR-03 riscv64 Mach-O target exists; before
  that it built ELF for an unknown OS, which ADR-03 rejects.
- GAP-06 **partly closed** (`rv graph --forbid` is still ignored; GAP-06 above): `rv test` runs every `Tests/**/test.toml` (`script`/`host` kinds;
  `qemu` skipped until S09; a file without `kind` is a schema fixture), and `rv image`
  validates `Tests/images/<name>` or `Images/<name>` before reporting E-NOT-BUILT S09.
  `kernelcache` (S09), `codesign` (S12) and `symbolicate` (S34) report E-NOT-BUILT and
  exit 3, which CI asserts.
- GAP-36 **closed**: nine layer fences and same-layer dependencies hold, and the keys
  beyond `kind`/`deps` are live: `current_version`, `compatibility_version` and
  `umbrella` become linker options, `reexports` become `-reexport_library` (and must be
  a declared dependency, `E-REEXPORT`), `deps.upward` becomes `-upward_library`, and
  `install_path` must be absolute. Four new unit tests; the three-target build still
  links.
- GAP-54 **closed**: `rv --root <tree>` works from any directory and the failures carry
  `E-CYCLE`, `E-LAYER`, `E-DEP-UNKNOWN` and `E-ROOT`.
- GAP-56 **closed**: licences match by exact SPDX id, case-insensitively, with `0BSD`
  accepted and the look-alikes (`BSD-4-Clause`, `MIT-0`) refused; three unit tests.
- GAP-40 **partly closed**. This entry records the state at `43757342b4`; the image no
  longer installs Ubuntu's `cargo`, `rustc` or `rustfmt` and no longer builds from the
  `ubuntu:24.04` tag (see the GAP-40 entry above). At the time, `Tools/ci/Containerfile` was missing
  `clang-format-18`, `cargo`, `rustc` and `rustfmt`, which the format gate needs, so the
  image could never have run it; it installs them now and links `clang-format`. The image
  builds from `ubuntu:24.04` and every gate passes inside it: `check-format.sh`,
  `check-spdx.sh`, `check-host-tools.sh` and `Tests/tools/run.sh` (78 passed, 0 failed,
  three consecutive runs). The new `gates-image` CI job builds the same image and runs
  the same gates in it. This sandbox needed two local accommodations, both outside the
  Containerfile: its containers filter plain HTTP and proxy TLS, so the base image was
  built with `https` sources and the host trust store (`ubuntu:24.04-sandbox`, passed as
  `--build-arg BASE`), and the bind-mounted repo needed `safe.directory`. A CI runner
  with an open network needs neither. Found on the way: the gate scripts treat a
  `git ls-files` failure as "nothing to check" in `check-spdx.sh` and
  `check-host-tools.sh` (both exit 1) but `check-format.sh` printed the error and still
  exited 0 when run against an untrusted mount; the ownership cause is gone in CI.
- BUG-29 **closed** (linker verified): the stage-1 toolchain was built from
  `swiftlang/llvm-project` at `swift-6.4.3-RELEASE`, whose lld refuses every Mach-O input
  (`This version of lld does not support linking for platform macOS`, reproduced by
  linking one object with that `ld64.lld`), so stage 1 could not link a ravynOS target.
  The pin is now upstream `llvm/llvm-project` at `llvmorg-21.1.6` and the tree builds
  `X86;AArch64;RISCV` (ADR-31). The lld-only target of the new cache entry links
  `x86_64` Mach-O (`file` reports a Mach-O executable, exit 0) and RISCV codegen
  compiles. The full upstream stage-1 install then completed (`stage1 verify OK:
  a72995bd…c4c77f`, clang 21.1.6, exit 0): its `bin/clang -print-targets` lists
  `riscv64`, `riscv32`, `aarch64` and `x86-64`, and its `bin/ld64.lld` links an x86_64
  Mach-O executable (exit 0).
- GAP-55 **partial** (verify defect fixed). `stage1.sh --verify` could never pass: the
  build recorded `sha256sum clang clang++ …` (11 names) while verify recomputed
  `sha256sum ./*` from inside `bin/`, so the fresh list carried the `./` prefix and
  four extra files (`clang-21`, `clang-cl`, `clang-cpp`, `llvm-objcopy`). The first real
  run failed on that alone. Both sides now call `Tools/toolchain/llvm/hash-tools.sh`,
  which owns the tool list; it prints one line per tool and does not depend on the
  working directory. Checked on the two existing builds of `a72995bd…`: the recorded
  `tools.sha256` and the hash list of the independent `--verify` rebuild diff empty, so
  the stage-1 tools are byte-reproducible. Seven host-tool tests cover the helper
  (`Tests/tools/run.sh`, 52 passed). A second defect showed up on the next run: with a
  cold cache the verify block went straight to the rebuild and reported a missing
  `tools.sha256` as a hash mismatch, after ninety minutes of compiling; it now refuses
  at once with "not built … run `rv bootstrap` first" (checked: exit 1 in under a
  second, no rebuild), and `hash-tools.sh` is part of the cache key because it decides
  what the recorded hashes cover. The `--verify` run under key `af8a5c44…` then
  completed end to end: the install built and stamped the key, `--verify` rebuilt it
  into `build/toolchain/af8a5c44…/.verify.<pid>`, the two `tools.sha256` lists diffed
  empty, and the re-hashed installed binaries passed `sha256sum -c` (job
  `stage1-build-verify`, exit 0, `stage1 verify OK: af8a5c44…`, clang 21.1.6).
  Since then the key moved to `llvm/key.sh`, which hashes the host cmake, ninja and
  clang versions along with the scripts, and the tool set gained `llvm-objdump`,
  `llvm-lipo` and `dsymutil` with `-ffile-prefix-map`. Key `04c9139e…` then reproduced
  the same way under the 14-tool list: the install built and stamped the key,
  `--verify` rebuilt it into `build/toolchain/04c9139e…/.verify.<pid>`, the two
  `tools.sha256` lists diffed empty, and the rebuilt tools re-hashed clean (job
  `stage1-key04`, exit 0, `stage1 verify OK: 04c9139e…`, clang 21.1.6). Stage-1 still
  did not build `libclang`, byacc, rvpatch, mkfat or elf2pe at that point (the build
  tools are a separate component now, GAP-55 above). BUG-29 was the reason the
  earlier pass proved nothing about linking; ADR-31 changes the source tree.
- GAP-59 **closed** (ADR-32): the stray `Tools/mig-mvtmp/` and
  `Apps/.tmp_bad_manifest` are gone, every commit since the restructure has a subject of
  72 characters or fewer with a lowercase scope, and ROADMAP.md 1.4 requires a merge
  commit to name its score. `Tools/ci/check-commit-subjects.sh` enforces the rules on every commit after the baseline `5112c72bc` and skips what came
  before, and four host-tool cases cover it.
- GAP-32 **closed**: `Docs/DESIGN.md` names only files that exist (`Docs/design/`,
  `Docs/design/DECISIONS.md`, `Tools/ci/check-spdx.sh`, `rv audit licenses`); four
  host-tool cases fail if a retired path returns. `Tests/perf/` and `Tests/kpi/` are
  marked planned-W2 where they appear.

| Criterion (SYSTEM-DESIGN section 6) | State |
|---|---|
| W0: the tree holds only the section 3.2 layout | closed (GAP-60) |
| W0: `rv` builds the tree with no duplicate names | closed (BUG-27; E-DUP-NAME fixture in `Tests/rv/negative/run.sh`) |
| W0: `rv` produces Mach-O for x86_64 and arm64 | closed (GAP-01; `Tools/ci/check-macho.sh` on both arches, https://github.com/ayeed-xec/ravynos/actions/runs/46139710547) |
| W0: `libhello.dylib` is `MH_DYLIB` on both arches (S01 done-when 3) | closed (asserted in CI by `check-macho.sh`, https://github.com/ayeed-xec/ravynos/actions/runs/46139710547; the assertion runs in CI, not yet inside rv, GAP-65) |
| W0: stage-1 toolchain builds and `--verify` reproduces it | open (GAP-55: both components build and `--verify` reproduces them; it closes on a green CI run of `rv bootstrap` and `--verify`, not recorded yet. GAP-61: libclang and `llvm-readtapi` come with the next LLVM rebuild) |
| W0: every merged component scores at least 8/10 in review | open (independent scores at the top of this file: rv 4, stage-1 5, loader 3, mig 6, xnuhook 7, CI 4) |
| W0: negative tests fail as designed in CI | open (GAP-65: the four cases in `Tests/rv/negative/run.sh` run in CI, but only two of the twelve S01 lists exist) |
| W0: audit and host-tool gate cover the tree | open (GAP-10: the deny list lacks flex, sed, awk and perl; GAP-09: no crate scanning or fingerprinting) |
| W0: no GPL material in the repository | closed (GAP-46: migcom's bison/flex output and XNU's Bison outputs pruned and enforced by `VENDORS.prune`; `Tools/xnuhook` plans the XNU parsers' byacc regeneration) |
| W0: `VENDORS` at the root | closed (GAP-14, GAP-31; `rv vendors check` in CI) |
| W0: correct `_PROVENANCE` everywhere | closed (GAP-31, GAP-58; xnu and migcom in `VENDORS`, mig `_PROVENANCE` paths repaired) |
| W0: SPDX on every file we own | closed (GAP-45, GAP-57; 15 remaining files fixed, `Tools/ci/check-spdx.sh` in CI) |
| W0: every section 5.6 gate runs in CI | open (GAP-40: every gate that exists runs inside the pinned image, but nothing builds with stage 1 yet; GAP-65: 10 of the 12 S01 negative cases do not exist; no mig golden-output comparison) |
| W1: kernel links with no stubs, both arches | open (GAP-12–GAP-16, GAP-41) |
| W1: loader boots XNU to its banner, both arches | open (BUG-02–BUG-06, BUG-24, BUG-25, GAP-39) |
| W1: T0 passes on both arches | open (GAP-07) |
| W1: P2, P9, P10 recorded | open |
| W0: GRUB rig and `Kernel/build/` removed | closed (GAP-17) |
