# Verified facts for the design docs (checked 2026-11-26)

Use these numbers and links in the docs. Do not invent versions, dates or capabilities.
If you need a fact that is not here, either omit it or mark it clearly as an estimate.

## Target system
- Target release: macOS 27.2.1, build 26B131 ("Golden Gate"), released 2026-11-09.
  https://developer.apple.com/news/releases/?id=11092026d
- Corresponding kernel: Darwin 27.2.0, XNU xnu-13432.41.4~25.
  https://macosdb.com/macos/release/27.2.1-26B131/
- Our vendored kernel in this clone is xnu-12377.121.6 (Darwin 25.5 generation). The design
  should specify upgrading the vendored source to the xnu-13432 line when Apple publishes it.
- Apple's open-source downloads now live on GitHub under github.com/apple and the release
  index at https://opensource.apple.com/releases/ (components also listed per release:
  https://opensource.apple.com/projects/).

## Metal (what is open, what is not)
- Apple has NOT released the Metal implementation. What exists:
  - metal-cpp: header-only C++ interface for Metal, Apache-2.0, github.com/apple/metal-cpp.
    Covers the full Metal 4 API surface through macOS 27 / iOS 27 (Apple11 GPU family).
    https://developer.apple.com/metal/cpp
  - Public docs: https://developer.apple.com/metal/ (Metal 4, MetalFX, MPS, tools).
  - Metal sample code library: https://developer.apple.com/metal/sample-code/ and
    https://developer.apple.com/documentation/samplecode
- Conclusion for the design: a working Metal stack (API surface, shading language, compiler
  backend, GPU drivers) must be written by us. The public headers define the ABI we must
  satisfy; the implementation is clean-room. Shaders, AIR/metallib formats and the compiler
  are the hard parts; say so plainly.

## ML / compute
- MLX: Apple's open-source array framework for Apple silicon (unified memory), MIT license,
  https://opensource.apple.com/projects/mlx — this is the realistic path for running/training
  models on our system (MLX has C++/C/Python/Swift bindings and PyTorch backend integration
  exists for Mac per developer.apple.com/metal).

## ravynOS filesystem decision
- ravynOS moved to the Darwin base (xnu) in Discussion #529 (2025-10-29,
  https://github.com/ravynsoft/ravynos/discussions/529). XNU expects APFS for the boot volume;
  the project gave up ZFS as root for that reason and chose APFS. HFS+ read and other
  filesystems (FAT, NTFS, ext4) remain useful as secondary filesystems.
- Today's install script still sets up ZFS on the old FreeBSD base — that is the old path.

## PureDarwin (what they do today — read carefully, do not copy)
Repo: https://github.com/PureDarwin/PureDarwin (main). Site: https://puredarwin.org/
- Kernel: Apple's XNU 12377 (reports Darwin 26.5, build 25F74); dev branch upstream-xnu-12377,
  main frozen.
- Build: Nix flakes (`nix build .#image-minimal`); calling cmake directly is unsupported;
  their own open-source SDK, no Apple SDK needed. macOS hosts build some components.
- Desktop: Wayland compositor ("Argon" on their neuswc/neuwld) plus X11; own AppKit,
  Foundation, CoreFoundation, CoreGraphics, CoreText, QuartzCore, OpenGL under
  src/Frameworks (BSD-licensed mySTEP lineage). Stock macOS Firefox ran unmodified since
  Oct 2026.
- GPU: AMD/Intel/nouveau through an in-kernel FreeBSD/Linux driver compatibility layer
  ("fbsdkpi"); Vulkan through Mesa; NVIDIA Maxwell/Pascal/Volta through NVIDIA's own 580
  driver via their PDNLoader.
- Linux programs: "elftrans" runs unmodified Linux binaries on XNU, including NVIDIA
  userspace (nvidia-smi, Vulkan).
- Drivers/filesystems: xHCI USB3, AHCI, HD audio, Wi-Fi, read-write ext4, HFS+, APFS.
- Hardware: x86_64 PCs; arm64 under QEMU plus Jetson TX1, ThinkPad X13s, Orange Pi Zero 3/4;
  iPhone 6s under the Inferno emulator; Apple M1 bring-up in progress.
- Known weaknesses we can do better on (form your own view, evidence over opinion):
  Nix-only build is opaque and non-modular; the compositor is Wayland/X11-based rather than a
  native WindowServer protocol; their AppKit talks to their own RPC protocol, so stock Mac
  GUI apps still do not run against it; no iOS story; no Windows build host; the kernel is
  pinned to the 12377 line.

## PureDarwin/xnu-loader (reference for our bootloader)
https://github.com/PureDarwin/xnu-loader — a GNU-EFI UEFI application (BOOTX64.EFI) that
reads the XNU Mach-O (or an lc-tools kernelcache) from the ESP, builds the boot_args struct
XNU's pstart expects, exits EFI boot services, copies segments to physical addresses, and
jumps to the kernel entry through a 64->32-bit mode switch. Built with Nix or CMake +
gnu-efi. Author's own note: "I made it to work, not to be clean." It is QEMU-oriented;
driver coverage (filesystems, storage) is minimal. Use it as a reference for boot_args and
the hand-off sequence, and design ours properly (modular, BSD/MIT, real driver model,
x86_64 + arm64 + riscv).

## Other references
- PureDarwin/efi-emulation and PureDarwin/gershwin-desktop: their EFI emulation and desktop
  experiments. Study for scope; our design must stand on its own architecture.
- darling (github.com/darlinghq/darling): reference for Mac-binary loading on Linux and
  xnu-on-Linux lessons. Contains GPL code in places — study ideas, import nothing GPL.
- dipa-studio/ios-sweet: xnu-8796 (iOS 16 generation) ported to a Redmi Note 10 Pro
  (SM7150/Snapdragon 732G) booting unmodified iOS 16.7.16 userland (launchd, backboardd,
  SpringBoard) to the home screen as of 2026-10-18. CPU rendering only (no GPU driver),
  ~20-30 fps, security largely off (permissive AMFI/sandbox), no Wi-Fi/BT/audio yet.
  Proof that Apple's open-source XNU + IOHIDFamily + libpthread + hfs can host real iOS
  userland when the SoC bring-up exists. Use as evidence for our iOS-alternative design;
  we are not porting their patches.
- apple/corecrypto: Apple's crypto library, source available under its own APSL-style
  license (github.com/apple/corecrypto) — study; license check per-file before any import.

## License policy (hard rule)
- No GPL, LGPL, or AGPL code anywhere in the tree, and no GPL-linked tooling in the build.
- Acceptable: APSL (Apple releases), BSD, MIT, Apache-2.0, Zlib, ISC, public domain.
- Every imported file keeps its upstream license header; a per-directory LICENSE audit is a
  build gate, not a convention.
- Behavioral study of Apple's shipped binaries is how we fill closed-source gaps
  (Metal, WindowServer, etc.): observe, specify, then write our own implementation
  clean-room. Never copy anything derived from Apple's compiled code into the tree.

## Machine reality
- Our build machines today: x86_64 Linux, 8 CPUs, 16 GB RAM, no GPU. Cross-compilation must
  work on modest hosts; arm64/riscv targets are cross-compiled or QEMU-emulated for tests.
