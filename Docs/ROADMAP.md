# ravynOS Roadmap 3.0 — xnu-native, UI-first

**Decision (2026-11-24):** one kernel, xnu, native, no emulation. The UI stack and
running real Mac UI apps — SwiftUI apps and other closed-source Mac apps — are the
top priority and every phase below serves that. FreeBSD is a driver source, not a
second OS. QEMU/TCG is only a dev rig; the shipping model is bare metal.

**Tooling license rule (user decision 2026-11-24):** no GPL code or GPL-derived
tooling enters the project — that excludes darling and its forks. Permitted
sources: Apple's own APSL releases, BSD/MIT-licensed harnesses, and our own
clean-room code.

**North star:** a closed-source Mac app the user already owns launches on ravynOS
and renders its window through our WindowServer.

**Kernel of record:** xnu-12377.121.6 (Darwin 25.5, macOS 26.x era), imported from
github.com/apple-oss-distributions/xnu tag `xnu-12377.121.6`, commit `fde4c10515`.
This replaces the old xnu-7195 (Darwin 19.6) tree, so the kernel source generation
matches the macOS era the target apps are built against.

---

## Where we are

| Phase | State | Exit criterion |
|---|---|---|
| P0 Baseline & compat matrix | DONE | matrix tracked, test apps chosen |
| P1 Foundation | DONE | static Mach-O guest exec runs multi-command shell on our kernel; host probes against ravynOS Foundation (commits `cab9ae7fc`, `5c5b83c9b`, `ebf9387e4a`) |
| P2 Kernel bring-up, new xnu | **STARTING** | xnu-12377 kernel binary boots in QEMU q35/OVMF, shell runs on it |
| P3 UI stack on xnu | not started | first-window app renders through WindowServer on the xnu VM |
| P4 dyld + real frameworks | not started | a dynamically-linked AppKit app launches |
| P5 Closed-source Mac apps | not started | Claude Desktop POC; SwiftUI feasibility verdict |
| P6 Bare metal | not started | same kexts boot on real hardware with real NIC |

---

## P2 — Latest xnu builds and boots (kernel track)

Goal: xnu-12377.121.6 compiled into a bootable kernel collection, booting in the
existing q35/OVMF rig, with the guest shell (sh2) running on it.

**WP-A1 — Linux build harness.** The Apple tree builds with Apple's makedefs on a
macOS host. On Linux we need three things, none of which may come from GPL
projects:
  1. **mig** — Apple's own mig, APSL-licensed. Apple stopped publishing standalone
     mig tarballs on opensource.apple.com (verified 404s on 2026-11-24), so the
     source must come from Apple's mig inside another APSL release channel
     (PureDarwin's non-GPL mirrors, or re-publishing from Apple's public svn/
     GitHub mirrors). A 2026-11-24 host probe showed migcom's source compiles on
     Linux with a small header shim; the only open issue is the include-path
     setup, not the code itself.
  2. **Generated headers** — `machine/types.h`, `mach/clock.h` etc. are mig- or
     script-generated; generate them once WP-A1's mig exists. Never hand-write
     them.
  3. **Build driver** — reference harness: `blacktop/darwin-xnu-build` (MIT;
     verified on GitHub 2026-11-24 to build macOS 26.x xnu and report x86_64
     boots). Use its scripts as reference to write our own MIT-licensed
     `Kernel/build/` driver pointed at our in-tree xnu.
  - Entry: none. Exit: a `guest_kernel` Mach-O produced on this host, version
    string 12377.121.6; blockers written to `Kernel/build/BLOCKERS.md`.

**WP-A2 — Boot the new kernel.** Use the existing recipe
(`/tmp/rvosvm/boot-sweep.sh`, q35/OVMF/Penryn/tcg). Expect OVMF to want a kernel
collection or an EFI-loaded Mach-O; adapt the Phase 1 injection tooling to the new
binary layout.
  - Entry: WP-A1 exit. Exit: serial log shows new kernel banner + sh2 banner;
    sh2 survives a 3-command session (same bar as Phase 1).
  - Note: the Phase 1 binary patches (scheduler NULL-guard, sh2 __TEXT RWX) were
    fixes for the OLD kernel; the new kernel ships the guard upstream
    (`ebf9387e4a` lineage). Do not carry the patches over — rebuild from source
    with the same intent (RELEASE or DEVELOPMENT config).

**WP-A3 — Performance on the new kernel.** tcg thread=multi, `-smp 2`, RELEASE
config, and strip the debug serial verbosity. No KVM here; bare-metal perf is P6.
  - Entry: WP-A2 exit. Exit: boot-to-shell time halved vs the P2 baseline (measure
    with the boot-sweep timestamps).

**WP-A4 — Drivers rebased onto new xnu.** The imported IONetworkingFamily-188 and
the FreeBSDPort vtnet shim were staged against the old tree; re-check their
headers against xnu-12377's KPIs (mbuf KPI and kpi_interface.h are still the same
family, versions differ) and continue the `if_vtnet` port there.
  - Entry: WP-A2 exit. Exit: vtnet kext links; DHCP lease inside the VM.

---

## P3 — UI stack native on xnu (top priority track)

Goal: our WindowServer + AppKit backend run on the xnu VM, and the first-window
app renders through them. Everything here is clean-room and in-tree already
(CoreGraphics/WSRPC.m, CoreServices/WindowServer,
Frameworks/AppKit/WindowServer.backend).

**WP-B1 — Mach-port IPC on the guest.** WindowServer's message.h contract moves
to xnu's real Mach IPC; the guest has no launchd yet, so WindowServer advertises
via a bootstrap port set by init.
  - Entry: WP-A2. Exit: two guest processes exchange a 4-byte ping/pong over the
    message.h contract; verified by a probe binary + serial log.

**WP-B2 — WindowServer as a guest daemon.** Build the WindowServer for the new
kernel's userland constraints (static, no dyld yet), render to a framebuffer
device (add a simple gfx kext or use OVMF's framebuffer via IOKit).
  - Entry: WP-B1. Exit: a window with title bar + button bitmap appears on the
    QEMU framebuffer; screenshot captured from the VNC display.

**WP-B3 — first-window on xnu.** Port the `apps/first-window` flow (already
verified on GNUstep under Xvfb on the host) into the guest.
  - Entry: WP-B2. Exit: "Clicked 0 times" + "Click me" render and increment in
    the xnu VM.

## P4 — dyld and real framework binaries (unlocks closed apps)

**WP-C1 — dyld for the guest.** The guest today runs static Mach-O only. Port/
build dyld (open-source in Apple's dyld releases, APSL) against the new xnu.
  - Exit: a dynamically-linked hello-world (linked against our libc equivalent)
    launches via dyld.

**WP-C2 — objc4-compatible runtime.** The in-tree libobjc2 (BSD/GPLv3 dual? —
verify its license mode before relying on it; if GPLv3 mode applies, replace with
a clean-room or BSD objc runtime) must present the ABI the target-era frameworks
expect (ARC, non-fragile, modern objc_msgSend entry points).
  - Exit: a real framework binary loads and calls through objc_msgSend with the
    modern ABI. License verdict recorded in Docs/ first.

**WP-C3 — CoreFoundation and Foundation ABI surface.** ravynOS already
reimplements these; the audit here is ABI-level (struct layouts, toll-free
bridging, CFTypeID stability) so binaries linked against Apple's frameworks can
satisfy their imports.
  - Exit: compat-matrix rows for CF/FS updated with ABI-verdict columns.

## P5 — Closed-source Mac apps (top priority track)

**WP-D1 — Claude Desktop POC, gates G0–G6** (from Docs/XNU-PLAN.md): static
Mach-O (done in P1) → first window on WindowServer (P3) → dyld (P4) → dynamic
AppKit app → Mach bootstrap/IOKit userland → Chromium headless → the .app
itself. Each gate de-risks one Electron layer.

**WP-D2 — SwiftUI feasibility probe (research, before any build work).** A
SwiftUI app links: SwiftUI.framework, SwiftUICore, the Swift runtime
(libswiftCore, closed), AttributeGraph, plus AppKit/CoreAnimation underneath.
This WP produces a written verdict with three options and a recommendation:
  1. ABI-compatible reimplementation of the SwiftUI surface (largest, cleanest);
  2. load Apple's framework binaries supplied by the user (fastest to a demo;
     legal posture must be decided by the user — Apple framework binaries are
     not freely redistributable);
  3. SwiftUI Export → HTML/other path for specific apps (not native; listed for
     completeness).
  - Exit: decision doc in Docs/, approved by the user, before committing effort.

**WP-D3 — AppKit shim depth for real apps.** Extend AppKit reimplementation to
the subset Electron/Chromium and typical apps touch (NSApplication, NSWindow,
NSView hierarchy, events, pasteboard, fonts).
  - Exit: compat-matrix AppKit rows green for the POC app's imports.

## P6 — Bare metal

Same kexts, real machines: NIC (vtnet → real e1000/atlantic), AHCI/NVMe, HID,
GPU framebuffer first (no accelerated GPU early). Funded by the POC demo.

---

## Parallel-agent dispatch plan

Four work packages can run concurrently; each owns disjoint files in the clone
at `/vercel/sandbox/ravynos` (branch `darwin`). Commit identity and push rules
per .mlai/CONVENTIONS.md.

| Agent | Owns | Entry | First deliverable (1 session) |
|---|---|---|---|
| A — kernel build | `Kernel/build/`, `Kernel/xnu/` (build-only edits) | now | WP-A1: mig obtained from an APSL source + harness runs, produces a kernel binary, blockers written to `Kernel/build/BLOCKERS.md` |
| B — guest UI | `CoreServices/WindowServer/`, `Frameworks/AppKit/WindowServer.backend/` | now | WP-B1: Mach-IPC ping/pong probe compiles + host syntax-checks; contract doc updated |
| C — runtime research | `Docs/` only + compat matrix | now | WP-D2 verdict draft + WP-C2 objc runtime license/ABI audit notes (read-only on code) |
| D — drivers | `Kernel/Extensions/FreeBSDPort/`, `FreeBSDPort/vtnet/` | after WP-A2 | re-check shim headers vs new KPIs; controller subclass skeleton |

Integration rules: A's kernel binary is the dependency gate for B3/D's on-device
runs; B and C produce no kernel edits; each agent ends its session with a
compile-check (`clang -fsyntax-only` for AppKit C-level files, harness build for
kernel) and a one-paragraph report with exit statuses.

## Assumptions

- "Latest xnu" = newest tag on apple-oss-distributions (12377.121.6, dated
  2026-06-17). If the user wants the absolute newest never-released tip, that
  does not exist publicly.
- SwiftUI priority is interpreted as: get the UI stack + closed-app path working
  (P2→P5 gates), and decide the SwiftUI strategy at WP-D2 before building it.
- Roadmap authority lives in the clone (`Docs/ROADMAP.md`); the repo copy is a
  mirror for the user's file tree.
