# 14 — Where the project stands: code against design

This document compares the code in the tree with the design in this directory. It says
what exists, whether it was built the native way the design asks for, which parts of the
design have no code yet, and where the design itself is wrong or silent. Read it before
picking up work; [15-implementation-guide.md](15-implementation-guide.md) turns each gap
into a concrete plan.

The comparison covers everything outside `Legacy/`, on branch `darwin-clean` at the
commit that adds this file. `Legacy/` holds the imported upstream ravynOS tree; none of
it is in the build graph, so none of it counts as progress until a component leaves
`Legacy/` with a manifest.

## The short answer

The project has a sound foundation and almost none of the operating system yet.

- The build and boot plumbing is real, small and clean: a Rust build harness, a
  freestanding UEFI loader that boots test kernels on three architectures, Apple's mig
  building on Linux with no GPL tool in the path, and CI gates that enforce the license
  rule mechanically.
- The kernel does not link. It compiles in tranches through a hand-built driver outside
  the harness, and the first object's link still needs 97 symbols.
- No Darwin program has run. The one user program in the tree, `Apps/hello`, is a Linux
  program: it makes Linux system calls and is an ELF file.
- Nothing above the kernel exists: no dyld, libSystem, launchd, window server, AppKit,
  Metal or SDK.

Measured by the exit criteria in [12-roadmap.md](12-roadmap.md), 4 of 13 criteria in W0
and W1 are met, W1.5 is done, and none of W2–W8 has started. In effort, the parts that are
done are the cheapest ones; the work toward running a Mac GUI application is roughly 3 to
5 percent complete. That figure is a judgment, not a measurement, and it is meant to set
expectations rather than to be tracked.

## What exists, and whether it is native

"Native" here means what [01-principles.md](01-principles.md) means: a real
implementation for this system, written to the contract it claims, with no stubs and no
foreign layer underneath.

| Component | Where | Size | Native? | Notes |
|---|---|---|---|---|
| `rv` build harness | `Tools/rv/` | ~760 lines Rust | yes | Manifests, layer fences, cycle check, ninja emission, SPDX audit. Missing `image`, `test`, `vendors`, `sdk`, `bootstrap`, `audit size` |
| UEFI loader | `Boot/loader/` | ~900 lines C and assembly | yes | Freestanding, no GNU-EFI. Parses Mach-O and ELF, builds an x86 `boot_args` laid out like XNU's `pexpert/i386/boot.h`, exits boot services, hands off on x86_64, arm64 and riscv64 |
| Test kernels | `Boot/testkernel/` | ~200 lines | yes | ELF test kernels that check the hand-off and print a structured panic. They prove the loader, not XNU |
| Boot image tools | `Boot/mkfat/`, `Boot/elf2pe/` | small C | yes | Our own FAT image writer and ELF-to-PE converter replace mtools and GNU objcopy |
| mig | `Tools/mig/` | vendored migcom + 3 patches | yes | Apple's `bootstrap_cmds-138`, pristine, patched by our `rvpatch`, parser from byacc |
| No-GPL gate | `Tools/ci/check-host-tools.sh` | ~60 lines | yes | Fails CI when a script invokes gcc, bison, GNU patch, m4, make, binutils |
| Host-tool tests | `Tests/tools/run.sh` | ~200 lines | yes | Unit tests for `rvpatch`, `mkfat`, `elf2pe` |
| Boot smokes | `Tests/boot/` | 3 scripts | yes | QEMU with OVMF, AAVMF and edk2; assert on serial output |
| Kernel compile driver | `Kernel/build/` | scripts + config stand-ins | partly | Compiles XNU files with hand-written `config-include/` headers instead of XNU's own `config` tool; outside `rv`; carries a second, older mig copy (`bootstrap_cmds-99.231.1`) |
| Kernel link | `Kernel/build/link/` | scripts + stub builder | no | Links against a stub object; `REAL-KERNEL-UNDEFS.txt` lists 97 missing symbols for `zalloc.o` alone |
| arm64 port | `Kernel/port/arm64/` | ~370 lines | **no** | `pmap.c` is a set of functions that ignore their arguments and return success — a stub, which the no-stubs doctrine forbids. It also duplicates XNU's own arm64 pmap |
| riscv64 port | `Kernel/port/riscv64/` | ~360 lines | partly | Real SBI calls, Sv39 `satp` setup and trap entry. Not compiled by anything; only the format gate reads it |
| `Apps/hello` | `Apps/hello/` | ~30 lines | **no** | Exits through Linux system call numbers (60 on x86_64, 93 on arm64 and riscv64) and links as ELF. It runs on Linux, not on XNU |
| `Foundation/libhello` | `Foundation/libhello/` | 1 function | n/a | Harness test fixture |
| FreeBSD driver shim | `Kernel/Extensions/FreeBSDPort/` | ~13k lines, mostly imported vtnet | partly | Shim headers map onto XNU KPIs; FreeBSD 11.4 drivers imported verbatim; not built into a kext yet |
| Other kexts | `Kernel/Extensions/` | upstream | unaudited | `AppleAPIC`, `AppleI386PCI`, `IOACPIFamily`, `RavynAHCIPort`, `RavynXHCIPort` and others came from upstream ravynOS. None has passed the DESIGN.md §6.3 audit |

Code quality, where code exists, is good. The loader handles firmware errors and retries
`ExitBootServices` correctly; `rv` is small, readable Rust with unit tests on its layer
rules; the host tools have their own test runner; every file carries an SPDX header.
What is missing is breadth and checks: `rv` has about 30 functions and 3 tests, the boot smokes
are not in CI, and one real bug sits in the loader (gap 23).

The pattern is clear. Work that was scoped tightly — the loader, the image tools, mig,
the gates — was done natively and well. Work that was scoped as "start the port early"
produced skeletons, and one of them is a stub.

## Exit criteria, checked

| Wave | Criterion | State |
|---|---|---|
| W0 | Hello builds with `rv build` and the output is Mach-O | not met: ELF |
| W0 | Planted layer violation and undeclared dependency fail CI as negative tests | partly: the checks exist, no committed negative case runs in CI |
| W0 | Planted GPL file, missing SPDX header, stripped vendored header fail the audit | partly: SPDX check exists, no negative cases |
| W0 | CI runs `cargo test`, `cargo clippy`, build, audit and format | partly: no `cargo test` or `clippy` |
| W0 | One project `.clang-format`, vendored trees exempt | met |
| W0 | Provenance records for every vendored tree outside `Legacy/` | partly: xnu and mig only |
| W0 | Tier test app sources and the macOS-built fixture set exist | not met |
| W1 | Kernel links with zero stub symbols, through `rv build kernel` | not met |
| W1 | mig vendored, patched, built without GPL tools; generated headers committed | met |
| W1 | T0: a static Mach-O built by our toolchain runs on XNU in QEMU | not met |
| W1 | P2, P9, P10 recorded in CI | not met |
| W1 | Kernel delta as a `why:`/`fate:` patch series | met trivially: no kernel patches yet |
| W1 | Every rule and script is triple-clean | met for current components |
| W1.5 | Tree restructure | met |

## Gaps: design with no code

Numbered so other documents and commits can cite them.

1. **No Mach-O output from the harness.** `rv` passes `--target=<arch>-ravynos` to clang
   and links with `ld.lld -m elf_*`; the result is ELF on all three architectures.
2. **No Darwin user program.** `Apps/hello` uses Linux system call numbers. A Darwin
   program on x86_64 uses `syscall` with `0x2000000 | number`; on arm64 it uses `svc
   #0x80` with the number in `x16`.
3. **No kernel link.** Tranche compiles exist; the link resolves against a stub.
4. **Kernel build outside the harness.** `Kernel/build/` is a separate system with its
   own mig copy and hand-maintained config headers.
5. **No XNU device tree on x86_64.** The loader leaves `boot_args.deviceTreeP` at zero.
   XNU's x86 platform expert reads the EFI device tree that Apple's `boot.efi` builds
   (`/efi`, `/efi/platform`, `/chosen`, the memory map node); without it the real kernel
   panics early.
6. **No arm64 XNU hand-off.** The arm64 path passes the x86-shaped `boot_args`. XNU on
   arm64 expects `pexpert/pexpert/arm64/boot.h`'s structure and an Apple Device Tree
   (ADT), not a flattened device tree (FDT).
7. **No x86 platform expert or ACPI story.** On Apple hardware these are closed kexts.
   The tree carries upstream replacements (`IOACPIFamily`, `AppleAPIC`,
   `AppleI386PCI`) that have not been audited or built.
8. **No kernelcache or kext loading.** Nothing builds a kernelcache or prelinked kext
   set, and nothing in the design says how kexts reach the kernel on arm64, where runtime
   kext loading is not how XNU works.
9. **No SDK.** `rv build sdk` does not exist; no `.tbd` stubs, no `SDKSettings.json`, no
   framework bundle layout.
10. **No stage-1 toolchain.** Builds use the host's clang and lld; `rv bootstrap` does
    not exist.
11. **No dyld, libSystem, objc4, libdispatch, CoreFoundation build.** Their sources sit
    under `Legacy/`.
12. **No launchd.** Apple's current launchd is closed; the design calls for a clean-room
    one and none exists.
13. **No window server, IOSurface, CoreGraphics, CoreText, QuartzCore or AppKit build.**
14. **No Metal.**
15. **No APFS.** HFS+ and msdosfs kexts are in the tree, unbuilt.
16. **No spec system.** DESIGN.md §3 requires a functional spec before implementation of
    anything studied from Apple's behavior; `Docs/spec/` does not exist.
17. **No performance benchmarks.** `Tests/perf/` does not exist; no budget is measured.
18. **No code-signing support.** Nothing parses or verifies `LC_CODE_SIGNATURE`; arm64
    macOS binaries are always signed and the kernel must at least accept ad-hoc
    signatures.
19. **No ABI census.** Nothing measures which frameworks, classes and symbols the target
    applications import, so framework priorities are guesses.
20. **The GRUB rig remains** in `Kernel/build/boot/`, GPL, though `Boot/` has replaced
    its function for test kernels.
21. **Two mig copies.** `Kernel/build/mig/` (99.231.1, flag shims) and `Tools/mig/`
    (138, patch series). One must go.
22. **`Kernel/port/` is not built.** Code that no build compiles is not tested by
    anything except the formatter.

23. **The loader hands the kernel a stale memory map.** `rv_build_boot_args` in
    `Boot/loader/main.c` records a memory map, and later the exit loop fetches a fresh
    map to call `ExitBootServices` — but the fresh map never replaces the one in
    `boot_args`. The kernel sees memory as it was before the final allocations. Test
    kernels do not notice; XNU's allocator will. Fix: fetch the final map into the
    `boot_args` buffer itself (sized with slack in advance) and exit boot services with
    that map's key.
24. **CI does not run the boot smokes.** `Tests/boot/smoke*.sh` run only when someone runs
    them; the workflow installs no QEMU. A boot regression can merge green.

## Not covered by any document

Things a Mac application depends on that no design document mentions. Each needs at
least a paragraph in the owning document before its wave starts.

| Topic | Why an application cares | Owning document |
|---|---|---|
| Installing apps from `.dmg` and `.pkg` | Most Mac software ships as a disk image or an installer package. DiskImages is closed; the UDIF format must be specified from observation. `.pkg` is a xar archive, and xar is open | 08, 15 §15 |
| Fonts | Apps ask for the system font by name (`.AppleSystemUIFont`, Helvetica, Menlo). Apple's fonts cannot ship; we need metric-compatible, permissively licensed replacements and a name-mapping table | 08 |
| Preferences | `NSUserDefaults` goes through `cfprefsd`, a closed daemon | 08, 15 §11 |
| LaunchServices and UTIs | Opening documents, `open(1)`, app registration, the Dock and Finder all go through it; closed | 08 |
| Keychain and Security | Many apps touch the keychain at launch, Electron apps among them; `securityd` is closed, parts of Security are open | 08, 15 §8 |
| Networking above sockets | `NSURLSession`/CFNetwork and Network.framework are closed; TLS needs a permissive library and a certificate trust store | 08 |
| Pasteboard, drag and drop, Services | Every text field uses the pasteboard; it is a daemon plus a framework API | 07, 08 |
| Text input and keyboards | Keyboard layouts and input methods live in HIToolbox's Text Input Sources; closed | 07, 08 |
| Accessibility | The AX API is used by automation tools and many utilities | 08 |
| Privacy prompts (TCC) and quarantine | Apps call TCC for camera, microphone and files; downloaded apps carry `com.apple.quarantine` | security design (F5) |
| Distributed notifications | `distnoted` is closed; `notifyd` (Libnotify) is open | 08 |
| x86_64 apps on arm64 | Rosetta is closed and is translation, which 01-principles.md rules out as architecture. Say plainly that arm64 ravynOS runs arm64 and universal apps only | 01, 12 |
| Developer tools on ravynOS | lldb and debugserver are open in LLVM; a developer platform needs them | 08, 13 |
| The spec-room Mac | macOS's license allows it to run only on Apple hardware, virtualized or not. The spec room needs real Apple machines | 15 §13 |

## Design flaws: where the design is wrong or silent

These are problems in the documents, not missing code. Each one, left alone, would send
an implementer the wrong way.

**F1. The SDK identity fights the compatibility goal.** 13-toolchain-sdk.md gives the
SDK a `ravynos` platform identity. Mac applications carry `LC_BUILD_VERSION` with
platform macOS; dyld, the frameworks and the kernel's code-signing paths all branch on
that platform. If our own SDK stamps a different platform, apps we build differ from the
apps we must run, and every platform check becomes a fork. The fix: the ravynOS SDK
targets the macOS platform and ABI exactly, so an app built with it is a Mac app; ravynOS
is the name of the SDK and the OS, not a new Mach-O platform. That also settles the
triple question (gap 1): use `<arch>-apple-macosx<version>`.

**F2. "arm64 comes with the tree" is wrong in an important way.** XNU's arm64 code
targets Apple systems: Apple's interrupt controller (whose driver is closed), Apple timers,
the SPTM page-table monitor on newer chips. One open configuration is usable on generic
hardware: `VMAPPLE`, the board Apple uses for macOS virtual machines
(`pexpert/pexpert/arm64/VMAPPLE.h`, `pexpert/arm/pe_fiq.c`), which drives a standard
ARM GICv3 found through `/arm-io/gic` in the device tree. The arm64 plan should target
that board — boot XNU as if it were a macOS VM guest, with the loader converting QEMU's
FDT into the ADT such a guest expects — instead of writing a new pmap. `Kernel/port/arm64/`
as written heads the wrong way.

**F3. The x86 plan underestimates what `boot.efi` does.** 04-boot.md treats the hand-off
as `boot_args` plus a mode switch. Apple's loader also builds the EFI device tree, passes
the kernel's address space layout, prepares the kext set, and sets up the boot video
structure; the platform expert and ACPI kexts that consume this are closed on Apple's
side. These belong in the boot and kernel designs as named deliverables.

**F4. Kext and kernelcache delivery is undesigned.** No document says how drivers reach
the kernel. Apple's `kmutil` is closed; the open `kext_tools` in the tree is old. This
blocks every driver, including storage, which blocks booting from disk.

**F5. Security is "staged" with no design behind the stage.** AMFI, the sandbox,
`TrustCache` and code-signature validation are closed on Apple's side. Mac binaries carry
signatures, arm64 ones always. The design needs a code-signing document: parse and hash
signatures (the format is documented by Apple), accept ad-hoc signatures, and define what
enforcement means on ravynOS.

**F6. The framework inventory is far too small.** 08-frameworks.md lists about a dozen
frameworks. A typical AppKit application loads dozens, including closed ones the design
never names: LaunchServices, HIToolbox, CFNetwork, ImageIO, CoreVideo, ColorSync,
AppKit's private SkyLight dependency, XPC. Priorities must come from measuring real apps
(gap 19), not from a list written in advance.

**F7. The closed-source ladder starts at the hardest rung.** DESIGN.md's W7 ladder runs
Electron → Qt → AppKit → SwiftUI. Electron carries Chromium, which uses more of macOS
than almost any application: its own sandbox, Mach ports and XPC, IOSurface, Metal
through ANGLE, CoreText, audio and video. The ladder should start with a small native
AppKit application, then Qt (whose Cocoa backend is a thin AppKit client), then
Electron.

**F8. The dyld shared cache is missing.** macOS system libraries live only in the shared
cache on current releases, and cold launch time (budget P8, ≤ 500 ms) depends on one.
dyld's open source includes the cache format and builder; the design never mentions it.

**F9. The clean-room process has no artifacts.** DESIGN.md §7 describes a spec room and
an implementation room. With no `Docs/spec/` and no spec template, implementers have
nothing to implement against, and the port skeletons show what happens: code written
ahead of any statement of the contract.

**F10. launchd's licensing is described too narrowly.** The design says launchd is
clean-room. Apple's last open-source launchd (the `launchd-842` line, Apache-2.0) is a
permissible seed for the job model and plist handling; the modern XPC-based behavior is
what needs study. That is cheaper than starting from nothing.

**F11. Two roadmaps still describe the same waves.** DESIGN.md §10 and 12-roadmap.md
both carry wave text. DESIGN.md now points at 12 for detail, but the duplicated exit
lines will drift; DESIGN.md should keep only the tier table and the wave titles.

**F12. "Done" for a framework has no machine check.** 02-architecture.md defines done as
"real Mac binaries link and behave". Without the ABI census (gap 19) and a fixture set
(13-toolchain-sdk.md §2.4), nothing can measure that.

## What to do next, in order

The order follows dependencies, and it fixes the foundation before anything is built on
it.

1. Settle F1: switch `rv` to `<arch>-apple-macosx` triples and Mach-O linking with
   `ld64.lld`; rewrite `Apps/hello` as a Darwin static binary (gaps 1, 2).
2. Delete `Kernel/port/arm64/` and re-plan arm64 around the `VMAPPLE` board (F2).
3. Bring the kernel build under `rv`, generating config headers with XNU's own
   `SETUP/config` from the `MASTER` files, and delete the second mig (gaps 3, 4, 21).
4. Finish the link with zero stubs; add the x86 EFI device tree to the loader; boot XNU
   to its first panic under our loader; delete the GRUB rig (gaps 3, 5, 20).
5. Write the kext delivery and code-signing designs (F4, F5).
6. Start `Docs/spec/` with the syscall surface and the T0 program; add the ABI census
   tool (gaps 16, 19).
7. Then W2: Libsystem, dyld and objc4 from `Legacy/` into manifests, and the SDK (gaps 9,
   11).

Each item has a section in [15-implementation-guide.md](15-implementation-guide.md).

## Open questions

- Is `Boot/loader/` kite, or a prototype kite replaces? It is C; DESIGN.md says Rust. The
  answer decides whether its next thousand lines are an investment or a rewrite.
- Which upstream kexts in `Kernel/Extensions/` are worth auditing, and which should move
  to `Legacy/` until a wave needs them?
- Who owns the spec system? Without an owner it will stay empty, and F9 will repeat in
  every wave.
