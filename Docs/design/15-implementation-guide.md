# 15 — Implementation guide

This is the working plan for each piece the design calls for and the tree does not have
yet: what to build, how to build it, what to read first, and the test that says it is
done. It is written for whoever picks up a work package, human or agent, and assumes
they have read [01-principles.md](01-principles.md) and the gap list in
[14-status-and-gaps.md](14-status-and-gaps.md). Gap numbers (G1…) and flaw numbers
(F1…) refer to that document. The subsystem documents still own the reasoning; this one
owns the steps.

## The compatibility contract

Everything below serves one sentence: a Mac application, as downloaded, runs on ravynOS
without modification. Concretely, that application is a Mach-O file that expects:

| It expects | Which means we must provide |
|---|---|
| `LC_BUILD_VERSION` platform macOS, a minimum OS version | a loader and dyld that accept the macOS platform and a version at or above the app's minimum |
| install names such as `/usr/lib/libSystem.B.dylib` and `/System/Library/Frameworks/AppKit.framework/Versions/C/AppKit` | real dylibs at exactly those paths |
| exported symbols and Objective-C classes by name | the same names, with compatible signatures and object layouts |
| system calls and Mach traps through libSystem | XNU itself, which gives us these for free |
| behavior: return values, notification order, event timing | clean-room implementations of every closed framework, specified from observation |
| a code signature, on arm64 always | a kernel and loader that read signatures and accept at least ad-hoc ones |

The kernel row is the reason the project runs Apple's XNU: the system-call ABI is not
something we reimplement. The behavior row is where nearly all the work is.

Our own SDK targets this same contract. An application built with the ravynOS SDK is a
Mac application: macOS platform, Apple install names, Mach-O. That is what lets one
compatibility surface serve both the apps we build and the apps we only run (F1).

## 1. Toolchain and target triple (G1, G10)

**What.** Every component builds as Mach-O for the macOS platform.

**How.**

- Map harness targets to clang triples in one table inside `rv`: `x86_64-ravynos` →
  `x86_64-apple-macosx<min>`, `arm64-ravynos` → `arm64-apple-macosx<min>`. Keep the
  harness names; they are labels, not triples. Pick `<min>` to match the ABI generation
  we target and record it in `VENDORS` beside the xnu pin.
- Link with `ld64.lld` (`-fuse-ld=lld` with a Darwin triple selects it) and pass
  `-platform_version macos <min> <sdk>`. Kernel and kext links keep their own flags in
  `Kernel/build/` until the kernel joins `rv`.
- riscv64 has no Darwin triple in LLVM. Until our stage-1 LLVM carries one (a small
  permanent patch adding a Darwin-family OS for riscv64 with Mach-O as its object format),
  riscv64 user code cannot be Mach-O. Say so in the riscv64 track rather than emitting
  ELF and calling it done.
- Add a CI step that runs `llvm-objdump --macho -p` on every linked output and fails
  on anything that is not Mach-O with `LC_BUILD_VERSION` platform 1 (macOS).

**Read first.** `EXTERNAL_HEADERS/mach-o/loader.h` in the vendored xnu
([upstream](https://github.com/apple-oss-distributions/xnu/blob/main/EXTERNAL_HEADERS/mach-o/loader.h));
[lld's Mach-O port](https://lld.llvm.org/MachO/index.html);
[13-toolchain-sdk.md](13-toolchain-sdk.md) §2.1 for the stage-1 plan.

**Done when** `rv build` on x86_64 and arm64 produces Mach-O for every component and the
CI format check passes.

## 2. T0 — a Darwin static program (G2)

**What.** Replace `Apps/hello` with a static Mach-O that runs on XNU.

**How.**

- No libSystem yet, so the program makes system calls directly. On x86_64: number
  `0x2000000 | n` in `rax`, arguments in `rdi`, `rsi`, `rdx`, `r10`, `r8`, `r9`,
  instruction `syscall`; the carry flag reports an error. On arm64: number in `x16`,
  arguments in `x0`–`x5`, instruction `svc #0x80`; the carry flag again reports an
  error. Numbers come from `bsd/kern/syscalls.master` in the vendored xnu (`exit` is 1,
  `write` is 4).
- Link `-static -e _start`. Check with `llvm-otool -l` that the binary has
  `LC_UNIXTHREAD` (or `LC_MAIN` with no dynamic loader expected) and no
  `LC_LOAD_DYLINKER`. If `ld64.lld` cannot produce the static form XNU accepts, the
  stage-1 toolchain builds Apple's `ld64` from the `ld64` and `cctools` sources, which is
  what the design lists anyway.
- The test writes argv and environment to stdout and exits with a known code; the boot
  smoke asserts all three.

**Read first.** `bsd/kern/syscalls.master`; `bsd/kern/mach_loader.c` (how XNU loads a
Mach-O and what it rejects); `osfmk/i386/bsd_i386.c` and `osfmk/arm64/sleh.c` for the
trap entry.

**Done when** the T0 criterion in [12-roadmap.md](12-roadmap.md) W1 passes in QEMU under
our loader.

## 3. The kernel build under `rv` (G3, G4, G21)

**What.** `rv build kernel` produces a linked XNU with zero stub symbols, per
architecture.

**How.**

- Generate configuration, do not hand-write it. XNU ships its own configuration tool
  (`SETUP/config`) and `config/MASTER*` option files. Build `config` as a host tool and
  run it to produce `kernel_config.h`, the per-subsystem option headers and the file
  lists. The hand-written headers in `Kernel/build/config-include/` then become a diff to
  review once, and disappear.
- Use XNU's file lists (`*/conf/files`, `files.<arch>`) to drive compilation. The
  tranche lists in `Kernel/build/` were a way to make progress file by file; the real
  build compiles what `config` selects.
- Wrap it in a kernel manifest with a build hook (05-build-system.md allows one, with a
  high bar; the kernel meets it). The hook runs the generated compile and link steps
  through ninja like every other component.
- Keep one mig: `Tools/mig/` (138, patch series). Regenerate `Kernel/build/mig/gen/`
  with it, compare, commit the result as the fixture, delete `Kernel/build/mig/`.
- Close the link with real definitions. The 97 symbols for `zalloc.o` are almost all
  defined elsewhere in XNU; they are missing because the tranche did not compile those
  files, not because they need writing. Anything that truly lacks a definition (a
  closed-source dependency) gets a patch in `Kernel/patches/` with a `why:`/`fate:`
  header, never a stub.

**Read first.** `xnu/README.md` and `makedefs/` in the vendored tree;
[darwin-xnu-build](https://github.com/blacktop/darwin-xnu-build) (MIT) for how others
drive XNU's build outside Xcode; DESIGN.md §8 for the patch rules.

**Done when** `rv build kernel -t x86_64-ravynos` links with no stub object and the
symbol report shows zero undefined symbols; P9 and P10 are recorded.

## 4. Booting XNU on x86_64 (G5, G7, F3)

**What.** `Boot/loader/` boots the real kernel, not only test kernels.

**How.**

- Build the EFI device tree XNU's x86 platform expert reads and pass it in
  `boot_args.deviceTreeP`/`deviceTreeLength`. The format is XNU's flattened device tree
  (`pexpert/gen/device_tree.c` parses it; `pexpert/pexpert/device_tree.h` defines it),
  not FDT. Required nodes are found by reading what `pexpert/i386/pe_init.c` and
  `osfmk/i386/AT386/model_dep.c` look up: start with `/chosen`, `/chosen/memory-map`,
  `/efi`, `/efi/platform` and the properties they query, and add each node only when a
  boot log shows XNU asking for it.
- Fill the `boot_args` video fields from UEFI GOP so XNU's early console draws.
- Keep the 64→32 entry switch the loader already performs for Mach-O kernels.
- Platform expert and ACPI: build and audit `Kernel/Extensions/IOACPIFamily`,
  `AppleAPIC` and `AppleI386PCI` first; they are the upstream answers to Apple's closed
  x86 platform kexts. Anything they lack is specified from behavior (section 13) and
  written as our own kext.
- Delete `Kernel/build/boot/` (the GRUB rig) in the same change that first boots XNU
  through `Boot/`.

**Read first.** `pexpert/pexpert/i386/boot.h`; `pexpert/i386/pe_init.c`;
[PureDarwin xnu-loader](https://github.com/PureDarwin/xnu-loader) as a reference for
which nodes a working loader provides; the
[UEFI specification](https://uefi.org/specifications) for GOP and the memory map.

**Done when** XNU, loaded by `Boot/loader/`, reaches `kernel_bootstrap` and prints its
banner on serial in QEMU with OVMF.

## 5. Booting XNU on arm64 as a VMAPPLE guest (G6, F2)

**What.** XNU arm64 boots under QEMU `virt` by being given the platform it already
supports for macOS virtual machines.

**How.**

- Build XNU's arm64 kernel for the `VMAPPLE` board (`pexpert/pexpert/arm64/VMAPPLE.h`,
  `board_config.h`) with the page-table monitors off: the classic pmap in
  `osfmk/arm/pmap/` with `XNU_MONITOR` disabled, not the SPTM path in
  `osfmk/arm64/sptm/`, which needs Apple's closed monitor.
- In the loader, read QEMU's FDT and write an Apple Device Tree in XNU's format with the
  nodes the VMAPPLE code looks up: `/arm-io/gic` with GICD and GICR `reg` values
  (`pexpert/arm/pe_fiq.c` shows the exact layout it expects), the PL011 UART, memory,
  CPUs, and the timer. Pass it with an arm64 `boot_args` from
  `pexpert/pexpert/arm64/boot.h`, entered at EL1 with the MMU off.
- Delete `Kernel/port/arm64/`. Port-layer code for arm64 appears only where VMAPPLE
  lacks something QEMU `virt` has, and each piece is named in a spec first.
- Apple Silicon hardware is a separate, later target (W5's stage-1 loader item); this
  path is for QEMU and generic arm64 machines.

**Read first.** `pexpert/arm/pe_init.c`, `pe_fiq.c`, `pe_identify_machine.c`;
`osfmk/arm64/start.s`; [QEMU virt machine](https://www.qemu.org/docs/master/system/arm/virt.html);
[GICv3 architecture specification](https://developer.arm.com/documentation/ihi0069/latest);
[Devicetree specification](https://www.devicetree.org/specifications/) for the FDT side.

**Done when** the arm64 kernel prints its banner under QEMU `virt` through our loader.

## 6. riscv64 (riscv64 track)

XNU has no riscv64 support, so this is a new architecture port: `osfmk/riscv64`,
`pexpert/riscv64`, a platform expert over SBI and an FDT-derived device tree. The
skeleton in `Kernel/port/riscv64/` (SBI calls, Sv39 `satp`, trap entry) is a reasonable
start. Before it grows, it needs two things: a build that compiles it, and the Mach-O
triple for riscv64 (section 1). The estimate in [12-roadmap.md](12-roadmap.md) stands.

**Read first.** [RISC-V privileged specification](https://github.com/riscv/riscv-isa-manual);
[SBI specification](https://github.com/riscv-non-isa/riscv-sbi-doc); `osfmk/arm64` as
the structural template.

## 7. Kexts and the kernelcache (F4)

**What.** A way to get drivers into the kernel, on both architectures.

**How.** Build a kernelcache: the kernel plus prelinked kexts in one Mach-O, the form
XNU's arm64 boot requires and x86 accepts. Write `rv kernelcache` as our own tool: read
each kext's `Info.plist` and Mach-O, resolve symbols against the kernel's exported
symbol sets (`config/*.exports`), lay out segments, and emit the
`MH_FILESET` kernelcache format XNU parses. Runtime loading of extra kexts on x86 can
come later; boot needs only the prelinked set.

**Read first.** `libkern/kxld/` and `libkern/c++/OSKext.cpp` in xnu (how the kernel
consumes kexts); `EXTERNAL_HEADERS/mach-o/fixup-chains.h` and the `MH_FILESET` handling in
`mach_loader.c`; `Kernel/kext_tools/` for the old open-source tooling.

**Done when** the x86_64 boot loads a kernelcache containing the HFS and storage kexts and
mounts an HFS+ root (needed by W3).

## 8. Code signing (G18, F5)

**What.** Read, hash and accept code signatures the way Mac binaries expect.

**How.** Parse the code directory in `LC_CODE_SIGNATURE`, compute page hashes, and accept
ad-hoc signatures. Our tools sign everything we build ad-hoc, as Apple's linker does on
arm64. Enforcement policy — which binaries must carry which signatures — is a separate
spec, written before AMFI-equivalent code exists. Nothing here involves Apple's keys or
certificates.

**Read first.** Apple's technotes
[TN3126: Inside Code Signing: Hashes](https://developer.apple.com/documentation/technotes/tn3126-inside-code-signing-hashes)
and [TN3127: Inside Code Signing: Requirements](https://developer.apple.com/documentation/technotes/tn3127-inside-code-signing-requirements);
`bsd/kern/ubc_subr.c` and `osfmk/kern/cs_blobs.h` in xnu;
[Apple Platform Security](https://support.apple.com/guide/security/welcome/web).

## 9. The SDK (G9, F1)

**What.** A ravynOS SDK with the same shape as Apple's macOS SDK, generated by `rv build
sdk`, containing nothing of Apple's proprietary SDK.

**Layout.**

```
ravynOS.sdk/
├── SDKSettings.json            name, version, platform macosx, deployment targets
├── usr/include/                headers exported by Libc, libdispatch, objc4, xnu ...
├── usr/lib/libSystem.B.tbd     text stubs, one per dylib we build
├── usr/lib/swift/              Swift overlay stubs, when the Swift runtime exists
└── System/Library/Frameworks/
    └── AppKit.framework/
        ├── Headers/            our headers for the public API
        ├── Modules/module.modulemap
        └── AppKit.tbd          install name /System/Library/Frameworks/AppKit.framework/Versions/C/AppKit
```

**How.**

- Headers come only from components' `exports.headers`. A framework's public headers are
  written by us from Apple's public documentation and the needs of the ABI census
  (section 12); they must declare the same names and layouts, and carry our license.
- `.tbd` files are generated from the dylibs we built, with LLVM's TextAPI tooling
  (`llvm-readtapi`), so the SDK cannot claim a symbol the system does not export. The
  install name and current/compatibility versions in each stub must equal Apple's for the
  same library, or Mac binaries will not bind.
- `SDKSettings.json` declares platform `macosx` and the deployment-target range we
  support; the display name says ravynOS. The SDK identifies itself honestly and targets
  the macOS ABI.
- A thin `rv sdk` wrapper prints the SDK path and runs clang with `-isysroot` and the
  right triple, the way `xcrun` does on a Mac, so third-party build systems work.

**Read first.** [13-toolchain-sdk.md](13-toolchain-sdk.md) §2.3;
[llvm-readtapi](https://llvm.org/docs/CommandGuide/llvm-readtapi.html); Apple's
documentation of
[framework bundle structure](https://developer.apple.com/library/archive/documentation/MacOSX/Conceptual/BPFrameworks/Concepts/FrameworkAnatomy.html).

**Done when** the T1 test app builds against the SDK alone (no `Legacy/`, no host
headers) and runs.

## 10. Libsystem, dyld and objc4 — T1 (G11, F8)

**What.** The user-space runtime every Mac binary loads first.

**How.**

- Libsystem is a set of Apple open-source projects assembled into one umbrella:
  `libsystem_kernel` (built from xnu's `libsyscall/`), `libsystem_c` (Libc),
  `libsystem_pthread`, `libsystem_platform`, `libsystem_malloc`, `libdispatch`,
  `libsystem_info` (Libinfo), `libsystem_notify`, `libsystem_blocks` (libclosure),
  `libsystem_m`, `libunwind` and others. Each becomes a component under `Foundation/`
  with a manifest and its upstream tag in `VENDORS`. `libxpc` is closed; T1 does not
  need it, launchd does (section 11).
- dyld comes from Apple's `dyld` project. Build it static, install at `/usr/lib/dyld`.
  Its source includes the shared-cache format and builder: produce a shared cache for
  our system libraries once they exist, because launch time depends on it (P8).
- objc4 next, then CoreFoundation (Apple's CF project, the C layer).
- Migrate each from `Legacy/` only after its DESIGN.md §6.3 audit, and update it to the
  tag that matches the ABI generation we target.

**Read first.** [Libsystem](https://github.com/apple-oss-distributions/Libsystem),
[dyld](https://github.com/apple-oss-distributions/dyld),
[objc4](https://github.com/apple-oss-distributions/objc4),
[Libc](https://github.com/apple-oss-distributions/Libc),
[libpthread](https://github.com/apple-oss-distributions/libpthread),
[libdispatch](https://github.com/apple-oss-distributions/libdispatch),
[CF](https://github.com/apple-oss-distributions/CF); the
[Objective-C runtime reference](https://developer.apple.com/documentation/objectivec/objective-c-runtime).

**Done when** T1 passes: a dynamic Objective-C program built against our SDK loads
through our dyld and runs.

## 11. launchd and userland — T2 (G12, F10)

**What.** PID 1, job control, and the bootstrap namespace every Mac program looks up
services in.

**How.** Seed from Apple's last open-source launchd (the `launchd-842` line, Apache-2.0)
for the plist job model, then specify modern behavior from observation: the XPC
service model, `launchctl` subcommands applications and installers call, and the
bootstrap-port lookups real apps make. libxpc's wire format and API behavior are a spec
item before they are code. The BSD userland (shell, coreutils equivalents) comes from
Apple's open-source releases where they exist and from FreeBSD otherwise.

**Read first.** [launchd source](https://github.com/apple-oss-distributions/launchd);
Apple's guide to
[creating launchd jobs](https://developer.apple.com/library/archive/documentation/MacOSX/Conceptual/BPSystemStartup/Chapters/CreatingLaunchdJobs.html);
the public [XPC documentation](https://developer.apple.com/documentation/xpc).

**Done when** T2 passes: boot from an HFS+ disk image to a login prompt.

## 12. The ABI census (G19, F6, F12)

**What.** A tool that tells us, for any Mac application, exactly what it needs from us.

**How.** `Tools/abi-census` reads an application bundle and walks every Mach-O inside it:
`LC_LOAD_DYLIB` and friends for libraries, the bind and chained-fixup tables for imported
symbols, `__objc_classrefs` and selector references for Objective-C classes and methods.
It writes one JSON record per app and merges them into a demand table: for each library,
which symbols and classes how many apps import. That table orders all framework work in
W4–W7, and "done" for a framework becomes measurable: every symbol the target apps import
exists and its behavior spec is verified. The tool reads third-party applications only;
it never reads Apple's libraries.

**Read first.** `mach-o/loader.h` and `mach-o/fixup-chains.h` in xnu's `EXTERNAL_HEADERS`;
[objc4](https://github.com/apple-oss-distributions/objc4)'s `objc-runtime-new.h` for
the Objective-C metadata layout.

**Done when** the census runs over the W6 app set and the fixture set from
13-toolchain-sdk.md §2.4 and its output is committed under `Tests/apps/census/`.

## 13. Studying closed components

Most of what makes macOS run Mac applications is closed: the window server, AppKit,
CoreGraphics, Metal, LaunchServices, AMFI. The method for each is the same, and it is the
method of DESIGN.md §7 made concrete.

**What we may use.** Apple's public documentation and headers; Apple's open-source code;
third-party applications' own binaries (to learn what they import); and the *observable
behavior* of Apple's software running on a Mac — what it returns, what it writes, what
it sends over IPC, what it does to hardware registers. What we may not use is anything
derived from Apple's compiled code: no disassembly or decompilation of Apple binaries
feeds a spec or a commit.

**How a spec gets written.**

1. Write the test first, on a Mac. A small program calls the API under study with chosen
   inputs and records outputs: return values, errors, notifications in order, files
   touched (`fs_usage`), Mach messages sent (`log stream` and interposing our own test
   process's calls), timing.
2. Write the spec in `Docs/spec/<subsystem>.md`: the behavior, the state machine, the
   wire format where there is one, each claim citing the test run that showed it.
3. Run the same test on ravynOS against our implementation. The test is the
   compatibility check, forever.
4. Where a closed driver talks to hardware, observe the hardware side instead of the
   software: that is the Asahi method, below.

**What Asahi Linux shows.** Asahi brought Linux to Apple Silicon Macs with no
documentation from Apple, and their method is the best public model of this kind of
work. Their bootloader, m1n1, can run macOS itself as a guest under a small hypervisor
on the real machine. macOS drives the real hardware, and every access it makes to device
registers is trapped and logged; Python tooling decodes the logs into readable traces.
The team learns how a device works by watching what Apple's driver does to it, then
writes a Linux driver from those observations. For the GPU, Alyssa Rosenzweig worked out
the shader instruction set by compiling shaders through Metal on macOS and studying the
output, and worked out the command interface the same way — enough to submit her own
work to the GPU before any kernel driver existed. Asahi Lina then traced the GPU
firmware's interface with the hypervisor and wrote the kernel driver in Rust. Rosenzweig
describes the discipline as observing the proprietary driver's output and looking for
patterns, and filling the gaps the driver never exercises by informed guessing and
testing.

Three lessons carry over. Observe at a boundary you are allowed to watch: hardware
registers, IPC messages, API results — never the code. Build the observation tooling
first; it is worth more than any single finding. And keep the test that established each
fact, because it becomes the regression test for the implementation.

One caution specific to us: m1n1 changed from MIT to GPLv2 from version 2.0.0, so its
code is forbidden input for our Apple Silicon loader (01-principles.md). The method is
free to use; the code is not.

**Read first.** [Tales of the M1 GPU](https://asahilinux.org/2022/11/tales-of-the-m1-gpu/);
[Dissecting the Apple M1 GPU, part IV](https://rosenzweig.io/blog/asahi-gpu-part-4.html);
[Reverse Engineering the M1, Black Hat USA 2021](https://i.blackhat.com/USA21/Wednesday-Handouts/us-21-Reverse-Engineering-The-M1.pdf);
[m1n1](https://github.com/AsahiLinux/m1n1) (read its README for the license history;
do not read its code for our loader).

## 14. Window server, CoreGraphics and AppKit — T3

**What.** One AppKit window on screen, driven by input, at 60 frames per second.

**How.**

- IOSurface first: allocation, cross-process sharing by Mach port, mapping. It is small,
  public in its API, and everything else moves pixels through it.
- The window server (Rust, per DESIGN.md's language policy) owns displays, a compositor
  and input routing, and talks to clients over Mach messages. Its client library is the
  private framework applications reach through CoreGraphics and AppKit — on macOS that is
  SkyLight, with the `CGS*`/`SLS*` functions. The census (section 12) lists which of
  those functions real apps call; implement those, with specs, and no more.
- CoreGraphics: drawing contexts, paths, images, color, fonts handed to CoreText. Start
  with the calls the T3 test app and the census demand.
- AppKit: audit the in-tree seed under `Legacy/Frameworks/AppKit` (DESIGN.md §6.3); if it
  passes, rebind its backend (`WindowServer.backend/`) to our window server's client
  library; if it fails, implement from spec. The event loop is `NSApplication` over a
  CFRunLoop with a Mach port source from the window server, as on macOS.

**Read first.** [07-graphics.md](07-graphics.md);
[AppKit](https://developer.apple.com/documentation/appkit),
[Core Graphics](https://developer.apple.com/documentation/coregraphics),
[IOSurface](https://developer.apple.com/documentation/iosurface) and
[Core Animation](https://developer.apple.com/documentation/quartzcore) documentation.

**Done when** T3 passes and P6/P7 are measured.

## 15. Frameworks width and closed apps — T4, T5 (F7)

Order framework work by the census, not by intuition. Order the closed-source ladder from
smallest to largest surface: a small native AppKit app, then a Qt app (Qt's macOS backend
is an AppKit client), then SwiftUI, then Electron, whose Chromium core touches more of
macOS than almost anything else. Every gap an app exposes goes through section 13 before
it becomes code.

**Read first.** [08-frameworks.md](08-frameworks.md); [../SWIFTUI-FEASIBILITY.md](../SWIFTUI-FEASIBILITY.md);
[OpenSwiftUI](https://github.com/OpenSwiftUIProject/OpenSwiftUI) as an API-shape
reference.

## 16. Metal

The plan in [07-graphics.md](07-graphics.md) and the Metal track in
[12-roadmap.md](12-roadmap.md) stand. Two additions. The API layer is checked against
[metal-cpp](https://developer.apple.com/metal/cpp) and the
[Metal Shading Language specification](https://developer.apple.com/metal/Metal-Shading-Language-Specification.pdf).
And the AIR/metallib formats are learned the section-13 way: compile known shaders with
Apple's public toolchain on a Mac and study the output files, which are our test inputs,
not Apple's code.

## Reference index

| Topic | Source |
|---|---|
| Apple open source, by project | https://github.com/apple-oss-distributions |
| Release index | https://opensource.apple.com/releases/ |
| Mach-O format | `Kernel/xnu/EXTERNAL_HEADERS/mach-o/` |
| XNU internals notes | `Kernel/xnu/doc/` |
| APFS format | https://developer.apple.com/support/apple-file-system/Apple-File-System-Reference.pdf |
| Code signing | TN3126, TN3127 (linked in section 8) |
| UEFI | https://uefi.org/specifications |
| Arm GICv3 | https://developer.arm.com/documentation/ihi0069/latest |
| RISC-V | https://github.com/riscv/riscv-isa-manual, https://github.com/riscv-non-isa/riscv-sbi-doc |
| Metal | https://developer.apple.com/metal/ |
| Behavioral study, by example | Asahi Linux links in section 13 |

## Open questions

- Which minimum macOS version does the SDK target? It decides which symbols and load
  commands our dyld must understand, and it should match the newest Mac software we
  intend to run, not the oldest.
- Can `ld64.lld` produce every Mach-O form we need (static executables, kernelcache
  filesets), or does the stage-1 toolchain need Apple's `ld64` from day one?
- Who keeps a Mac for the spec room? Section 13 needs one for every closed framework, and
  CI must never depend on it.
