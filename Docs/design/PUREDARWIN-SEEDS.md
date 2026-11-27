# PureDarwin as a seed source

PureDarwin (github.com/PureDarwin/PureDarwin) builds an operating system on the same XNU
line we vendor (xnu-12377) and boots it on PCs and arm64 machines. This document lists
which parts of it ravynOS may take, which it should only study, and which it must avoid,
and why. It applies [DECISIONS.md](DECISIONS.md) ADR-27. Subsystem numbers refer to
[SYSTEM-DESIGN.md](SYSTEM-DESIGN.md).

The survey covered PureDarwin at commit `0c1990c8`, plus its external `xnu-loader`,
`kc-tools` and `iig-tools` repositories.

## 1. Rules

- **License per file.** The license of a PureDarwin file is the one its own header
  states; the project's NOTICE says so explicitly. Files under "MPL-2.0 OR
  BSD-2-Clause" are taken under BSD-2-Clause. Files under MPL-2.0 alone, GPL, LGPL,
  CDDL, CC-BY-SA, or with no license header are not taken.
- **Files with no header** are treated as unlicensed until their authors state a
  license (section 6).
- **GPL and LGPL files** are not copied, and engineers who write the matching ravynOS
  code do not read them (section 5).
- **Taking code** means vendoring it like any upstream: the untouched tree in
  `<component>/upstream/`, changes as patches, and a `_PROVENANCE` file naming
  PureDarwin, the commit, the files taken and their licenses. Each `_PROVENANCE` also
  states "based on PureDarwin", as PureDarwin's NOTICE asks of derived work.
- **PureDarwin edits XNU in place**, without a patch series. Changes we want are
  re-cut, by topic, as patches in `Kernel/patches/<series>/` with `why:`/`fate:`
  headers, and reviewed one hunk at a time. Edits to Apple's files are APSL-2.0.
- **Architecture is not taken.** PureDarwin's Nix build, its Wayland-based display path
  and its stubbed services do not fit our design (ADR-07, SYSTEM-DESIGN section 2.4).

## 2. Kernel, boot and platform

| Component | Verdict | Serves |
|---|---|---|
| XNU changes: x86 CPU vendor (AMD, Hygon) | take, as patches | S06 |
| XNU changes: TSC sources (AMD P-state, TCG) | take, as patches | S06 |
| XNU changes: boot (FB above 4 GB, stolen memory) | take, as patches | S04, S06 |
| XNU changes: root mount, volume group | take, as patches | S05, S19 |
| XNU changes: periodic TSC resync | avoid | — |
| XNU changes: build, CMake, builtin stand-ins | study | S05 |
| XNU changes: arm64 boards, PAC emulation | study | S07 |
| XNU changes: riscv64 port | study; license unclear | S08 |
| uACPI (MIT, from upstream) | take | S06 |
| ACPI platform expert, APSL parts | take | S06 |
| ACPI platform expert, other files | study or avoid | S06 |
| `kc-tools` kernelcache builder (BSD-3) | take after header check | S09 |
| `xnu-loader` (BSD-3, GNU-EFI) | study | S04 |
| `iig-tools` (BSD-3) | study | S10 |

Notes:

- The ACPI platform expert subclasses `IOACPIPlatformExpert` and uses uACPI for MADT
  CPU enumeration, ACPI S5 and reset, and the power button. It does not publish ACPI
  namespace nubs; ours does. `PDACPIEC.cpp`, `PDACPII2C.cpp`, `PDEFINVRAM.*` and
  `PDEFIVariables.*` are MPL-2.0 only; `PDACPIKernel.cpp`, `PDACPIGlue.cpp` and
  `PDACPIPower.cpp` have no header.
- `kc-tools` builds `MH_FILESET` kernelcaches, links kexts, and converts relocations to
  kernelcache chained fixups for x86_64, arm64 and riscv64. Its repository license is
  BSD-3-Clause; its files carry no per-file headers.
- `xnu-loader` builds `boot_args`, the XNU device tree with `/efi/platform`, and an
  FDT-to-Apple-device-tree conversion for arm64. It depends on GNU-EFI and has a second
  contributor whose grant must be confirmed; it is a reference for S04, not a seed.

## 3. Drivers

| Component | Verdict | Serves |
|---|---|---|
| `fbsdkpi`: FreeBSD and Linux driver layers | take, restructured | S11 |
| FreeBSD em, igc, rge drivers | take | S11 |
| Wi-Fi: iwlwifi, rtw89 (BSD side), iwn, rtwn, run | take | S11 |
| virtio block, net, GPU, input (BSD-2 side) | take | S10 |
| `RavynAHCIPort`, `RavynXHCIPort` (MIT) | take | S10 |
| `PDRealtek8111` (BSD) | take | S10 |
| Apple IOKit families with PD edits | take from Apple; diff edits | S06, S10 |
| IOUSBFamily reconstruction, NVMe, GOP FB | study; no headers | S10 |
| arm64 platform expert, ECAM PCI | study; no headers | S07 |
| ext4 file system | study; no header | S19 |
| NVIDIA loader (`PDNVLoader`) and its blob | avoid | — |
| DRM drivers fetched at build time | avoid | — |
| corecrypto fork (MPL-2.0) | avoid (ADR-12) | — |
| PowerVR firmware blob | avoid | — |

The FreeBSD layer is the largest single saving: a working in-kernel newbus, mbuf and
iflib environment with real network drivers. Our S11 design pins FreeBSD 15.1 and puts
shared pieces in `kpi-core`, so the code is restructured as it comes in.

## 4. User space and frameworks

| Component | Verdict | Serves |
|---|---|---|
| CoreText (dual-licensed rewrite) | take | S23 |
| CoreGraphics and Onyx2D, MIT and dual files | take | S23 |
| Carbon: TIS, UCKeyTranslate, theme metrics | take | S25, S26 |
| AudioToolbox, CoreAudio, CoreMedia, CoreMIDI | take | S27 |
| `coreaudiod` | take | S27 |
| CVPixelBuffer | take | S23 |
| IOSurface library and `iosurfaced` | take as seed | S22 |
| libsandbox (SBPL compiler) | take | S12 |
| FSEvents.c (dual) | take | S18, S19 |
| Userspace apps: Finder, Dock, Terminal … | study or take | S32 |
| AppKit: MIT files with Firefox-driven fixes | take per file | S25 |
| QuartzCore (GL readback renderer) | study | S24 |
| Wayland client shim (`rpc_wayland.c`) | study | S22 |
| OpenGL/CGL | study; no header | S28 |
| AVFoundation, ScreenCaptureKit and others | study as API lists | S27, S31 |
| CoreFoundation (swift-corelibs base) | avoid as base; study | S15 |
| Foundation (mostly MPL-2.0 only) | avoid | S15 |
| CoreServices library (MPL-2.0 only) | avoid | S25 |
| HIServices (MPL-2.0 only) | avoid | S25 |
| libSystem, dyld, objc4, launchd versions | study only (macOS 11–12 era) | S13–S17 |

Notes:

- PureDarwin's AppKit descends from the same Cocotron and ravynOS code as our
  `Legacy/Frameworks/AppKit`, with several hundred lines of fixes per core class
  driven by Firefox. Its MIT files are a better seed than ours for those classes.
- Several of PureDarwin's frameworks return fake success, for example an empty window
  list from `CGWindowListCopyWindowInfo` and requirement checks in Security that always
  pass. These parts are not taken; our implementations follow SYSTEM-DESIGN section
  2.4.
- Worth upstream review: dyld read the `dlerror` thread-specific key 0 as
  `pthread_self`; PureDarwin's fix is a candidate patch for S14.

## 5. Must not be copied or read by implementers

- `src/Libraries/XPC/libxpc/xpc_error.c` (Darling, GPL-3.0).
- `src/ThirdParty/neuswc/protocol/server-decoration.xml` (LGPL-2.1).
- `src/Libraries/Security/apple/OSX/libsecurity_asn1/lib/*` (NSS, MPL-1.1/GPL/LGPL).
- `tools/cctools/as/*`, `tools/cctools/include/coff/*`, `include/elf/dwarf2.h`,
  `include/opcode/arm.h`, `otool/arm_disasm.c` (GPL).
- `tools/dtrace_ctf/*` (CDDL, LGPL libdwarf), `src/Userspace/dtrace` (CDDL).
- `src/Userspace/fbdoom/*` (GPL).
- `cmake/circular.cmake` (GPL-3).
- Trenta icons under `src/Userspace/*/Resources` (CC-BY-SA-4.0).

## 6. Questions for PureDarwin's authors

Sent before any affected file is taken:

1. The license of files with no header, in particular: `PDACPIKernel.cpp`,
   `PDACPIGlue.cpp`, `PDACPIPower.cpp`, the arm64 platform expert, `IONVMEFamily`,
   `IOGOPFramebuffer`, `PDHyperV`, the ext4 driver, the IOUSBFamily reconstruction, the
   new riscv64 files in XNU, `CGL.c`, the QuartzCore files, and the AppKit additions
   (`NSStoryboard.m`, `NSAccessibility.m`, `NSNibArchiveUnarchiver.m`).
2. Whether the MPL-2.0-only files (Foundation, CoreServices, HIServices, the EFI NVRAM,
   embedded controller, I2C and USB networking drivers) can be offered under
   "MPL-2.0 OR BSD-2-Clause" like the rest of the project.
3. Per-file SPDX headers for `kc-tools` and `xnu-loader`, and confirmation of the second
   contributor's grant in `xnu-loader`.
4. The Firefox configuration that runs: build, architecture, preferences (WebRender and
   GPU process), and how it is installed and launched.
