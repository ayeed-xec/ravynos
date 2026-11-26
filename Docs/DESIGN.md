# System Design & Wave Roadmap (Roadmap 4.0)

**Status:** Authoritative. Supersedes `Docs/ROADMAP.md` (Roadmap 3.0), which has been
removed along with its P0–P2 work packages.
**Date:** 2026-11-25
**Scope:** The full OS: boot, kernel, drivers, userspace, frameworks, UI, installer — on
x86_64 and arm64, Mac and non-Mac hardware, with later RISC-V/other ports.

---

## 0. Mission and non-negotiables

Build a native macOS-compatible operating system that runs real Mac applications, is
architected to Apple-parity standards, and is fundable as a demonstration.

Four constraints bind every decision in this document. None is waivable by an implementer;
only a dated decision record in `.mlai/DECISIONS.md` can change one.

1. **License purity.** No GPL, LGPL or AGPL code, and no GPL-derived tooling, anywhere in
   the tree — including build tools, scripts, and generated artifacts. Every vendored
   component carries a provenance record. Our own code is licensed so the whole OS can be
   redistributed under one coherent license set (§6).
2. **Measured performance.** "Ultra-low latency" is a set of numeric budgets with named
   benchmarks (§5), not an adjective. Every subsystem ships with a benchmark; CI fails on
   regression. No performance claim is accepted without a before/after measurement.
3. **Functional understanding before code.** For anything we implement against observed
   Apple behavior (including closed-source apps), a written functional spec exists first,
   produced under the clean-room protocol (§7). We study *observable behavior*, document
   it, and implement from the document — never from anything derived from Apple's
   compiled code.
4. **xnu is vendored, not modified.** The kernel of record is Apple's xnu-12377.121.6,
   kept pristine so future versions drop in cleanly (§8). Everything we add lives outside
   the vendored tree as shims, overlays, or a maintained patch series.

Everything else — framework choices, wave ordering, lane structure — is engineering
judgment and may be revised by the wave owner with a note in the wave log.

---

## 1. What "parity with Apple" means here: the compat tiers

Parity is scoped as tiers with testable exits. A tier is done when its exit criterion
passes on the reference rig, not when its code is written. This is what makes the project
falsifiable — for contributors, investors, and ourselves.

| Tier | Definition | Testable exit | Wave |
|------|-----------|---------------|------|
| **T0** | Exec format: we load and run a static Mach-O | A static Mach-O built with the macOS SDK (`clang -static`) runs on our kernel; correct exit status, argv, env | W1 |
| **T1** | Dynamic linkage: dyld + objc runtime work | A trivial *dynamic* Objective-C app (Foundation only, SDK-built) links via dyld,objc4 and runs to completion | W2 |
| **T2** | POSIX/BSD surface: real userland lives on us | launchd starts from disk; our shell + a ported set of BSD userland tools run a full boot-to-login cycle with no stub syscalls on the hot path | W3 |
| **T3** | Graphics: window server + AppKit slice | An SDK-built AppKit app shows a window, responds to mouse/keyboard at ≥ 60 fps with the W3 latency budget (§5) | W4 |
| **T4** | Frameworks width: the 20-app set | A fixed set of 20 real-world open-source Mac apps (chosen in W4, licenses permitting) launch and function at the level their AppKit usage requires | W6 |
| **T5** | Closed-source Mac apps | A designated commercial app (POC candidate: an Electron- or Qt-based app, then a native AppKit app) installs and runs | W7 |

Rules that follow from the tiers:

- **Depth before width.** No framework work starts before the tier below it exits. Upstream
  ravynOS's core failure mode — hand-writing giant framework surfaces with unresolved
  symbols, on an unfinished ABI — is explicitly banned here. A component that cannot be
  linked without `-undefined dynamic_lookup` or stub symbols does not ship.
- **The test app is the exit criterion.** Each tier names the binary that proves it, before
  work on that tier starts. Adding to the 20-app set after T4 work begins requires a wave
  decision.
- **Compile progress is not compatibility progress.** A milestone report says what *ran*,
  not what *compiled*.

---

## 2. System architecture

### 2.1 Layer map

```
+---------------------------------------------------------------------+
| Distro layer:   Installer · ISO · OS update · System prefs          |
+---------------------------------------------------------------------+
| App layer:      AppKit/UIKit-slice apps · SysApps · 20-app set      |
+---------------------------------------------------------------------+
| Frameworks:     AppKit · CoreGraphics · CoreText · Foundation       |
|                 (clean-room, spec-driven; reuse of in-tree BSD/MIT  |
|                  ravynOS framework code where the audit passes)     |
+---------------------------------------------------------------------+
| Core runtime:   dyld · objc4 · Libsystem · libdispatch · XPC        |
+---------------------------------------------------------------------+
| Userland core:  launchd · WindowServer · IOKit user lib             |
+---------------------------------------------------------------------+
| Kernel:         xnu-12377.121.6 (vendored, pristine)                |
|                 + RVOS overlay: platform ports, config, kext shim   |
+---------------------------------------------------------------------+
| Driver layer:   IOKit families (IONetworkingFamily, ...)            |
|                 + FreeBSDPort shim (FreeBSD 11.4 drop-in drivers)   |
|                 + HAL (platform abstraction, §2.4)                  |
+---------------------------------------------------------------------+
| Platform:       x86_64 QEMU/OVMF · x86_64 bare metal · arm64 QEMU   |
|                 virt · arm64 Apple Silicon (our own stage-1 loader)      |
+---------------------------------------------------------------------+
```

Each layer communicates with its neighbors through a **frozen interface contract**
(headers + ABI + documented behavior), listed in §9. Contracts are frozen at wave start so
parallel lanes never block on each other's internals.

### 2.2 Boot architecture

One boot chain shape, four platform backends:

```
firmware → RVOS loader (Mach-O/img4-capable) → guest_kernel (Mach-O) → launchd
```

- **x86_64 QEMU (dev rig):** GRUB 2.12 multiboot2 → 64-bit loader → Mach-O at
  0x100000000. Works today (BIOS and q35/OVMF paths in `Kernel/build/boot/`).
- **x86_64 bare metal (W4 target):** OVMF/UEFI on commodity PCs; loader gains disk/NVMe
  and a real filesystem read path; kexts linked or prelinked.
- **arm64 QEMU virt (W5):** U-Boot → our arm64 loader entry → xnu arm64.
- **Apple Silicon (W5/W7):** the Asahi *pattern*, followed in method and not in code: a
  stage-1 loader enrolled through Apple's official boot policy from 1TR recoveryOS,
  presented as the kernelcache slot iBoot expects — i.e. a Mach-O wrapped in img4. Our
  kernel already *is* a Mach-O, and our Phase-1 loader/patch experience (`patch_macho.py`,
  `verify.py`) transfers directly. **License note (updated 2026-11-26):** m1n1's `main`
  branch relicensed MIT → GPLv2 in the Asahi relicense announced 2026-11-15, so m1n1
  source is now forbidden input under our license rule; the stage-1 loader is our own
  clean-room implementation of the publicly documented boot-policy method, and Asahi
  sources may not be read or adapted for it. The method itself is not encumbered.
  Hardware bring-up order per platform: UART/NVMe/PCIe → boot framebuffer →
  full display later. This is the same order the HAL (§2.4) assumes.

Boot-time budget is a performance gate, not a nicety: **≤ 3 s power-to-first-app-frame on
bare-metal NVMe**; the QEMU dev rig targets ≤ 10 s and only trends matter there.

### 2.3 Kernel architecture

- **Vendored xnu-12377.121.6** (Darwin 25.5 era) is the kernel of record, commit
  `fde4c10515`, at `Kernel/xnu/`. It carries complete `osfmk/arm`, `osfmk/arm64`,
  `osfmk/i386`, `osfmk/x86_64` and `pexpert/{arm,i386}` trees: Apple maintains the
  portability layer, we build slices of it (§8).
- **Everything we add lives outside the vendored tree:**
  - `Kernel/build/` — the build harness (§8.4 of the harness design; compile driver,
    export-headers flattening, own APSL-licensed mig, config-include stand-ins, link +
    patch + verify chain, boot rig).
  - `Kernel/build/config-include/` — committable stand-ins for build-time generated
    config headers (`kernel_config.h`, `mach_kdp.h`, `mach_ldebug.h`, `san/` forwarders).
  - `Kernel/Extensions/FreeBSDPort/` — the driver shim.
  - `Kernel/RVOS/` (new, W1) — the overlay tree for the small amount of genuinely ours
    kernel code (platform init, DMI/SMBIOS quirks, the scheduler guard equivalents for
    the new kernel) plus the quilt-style patch series against xnu (§8.2).
- **Old-kernel phase-1 patches do not carry over.** The scheduler-guard and `sh2` __TEXT
  RWX patches applied to xnu-7195 were workarounds for that tree; the new kernel's
  equivalents are designed, not ported.
- **Link path:** objects compile one-tranche-at-a-time today (139 clean; 97-symbol
  workplan in `Kernel/build/link/REAL-KERNEL-UNDEFS.txt`); W1 completes the full link with
  zero stub symbols (§10, Wave 1).

### 2.4 Driver architecture and the HAL

- **Policy: FreeBSD is a driver source, not a second OS.** FreeBSD 11.4 (pre-iflib)
  drop-in drivers compile against xnu KPIs through the `FreeBSDPort` shim
  (`sglist_seg` matched to 11.4, `BUS_READ/WRITE_IVAR` dispatched through the driver's
  `device_method_t` by name). The iflib port for modern FreeBSD is deferred to a dedicated
  phase after W4 — recorded in `Kernel/build/HARDENING.md`.
- **HAL for portability.** Every platform-specific fact (uart, interrupt controller,
  timer, framebuffer, DMA) is reached through a small HAL interface
  (`Kernel/RVOS/platform/<arch>-<board>/`), never poked from driver or kernel logic
  directly. Rule: **a new board port = new HAL directory + device configuration, zero
  edits elsewhere.** This is the mechanism that makes "any hardware, Mac and non-Mac"
  a build-graph property rather than a porting project per device.
- First drivers: virtio-net (vtnet, in progress), virtio-blk, virtio-gpu, then real
  hardware: AHCI, NVMe, xHCI, Intel/AMD framebuffer (KMS-style via IOKit graphics family),
  common Ethernet/WiFi through the same shim.

### 2.5 Core runtime architecture (T1 — the compatibility heart)

The runtime layer decides whether Mac apps run. It is specified as a contract set, each
with a functional spec written before implementation (§7):

| Contract | Source | Our implementation |
|---|---|---|
| dyld ABI (dyld info, fixups, initializers) | Public headers + dyld source (APSL) | Port of Apple's open dyld where license permits, else clean impl against the spec |
| objc4 runtime | Apple open source (APSL) | Vendor and build on Linux/host toolchain; class registry, message dispatch, categories, non-fragile ABI |
| Libsystem / libc | Apple open source (APSL) + BSD | Vendor APSL parts; fill gaps with BSD |
| Mach IPC / XPC surface | xnu + public docs | xnu native; XPC as clean-room spec impl |
| launchd behavior | Public docs + observed behavior | Clean-room reimpl (upstream's `old-launchd` audited; only reused if license and quality pass) |

The gate for this layer is T1's exit: an SDK-built dynamic Objective-C app runs. That
single test exercises dyld fixups, objc dispatch, Foundation basics and the syscall
surface end to end.

### 2.6 Frameworks and UI architecture

- **Strategy: spec-driven clean-room implementations, reusing licensed code where the
  audit passes.** The clone already carries ravynOS's own framework trees (AppKit,
  CoreGraphics, CoreText, Foundation, CoreFoundation, ...). ravynOS's stated practice is
  clean-room-from-public-docs with FreeBSD/MIT release — consistent with our rule — but
  every file we reuse passes the license audit (§6.3) and a quality gate; everything else
  is reimplemented from the functional spec.
- **UI stack:** `WindowServer` (in `CoreServices/`) is the composition server: per-window
  buffers, GPU-accelerated composition (virtio-gpu / bare-metal KMS path), input routing
  with a **hard input-to-photon budget** (§5). The `first-window` demo apps in this repo's
  `apps/` are the T3 test harness.
- **Zero-copy path:** window buffers are IOSurface-like shared-memory surfaces; the
  compositor swaps, it does not blit, on the hot path.

### 2.7 Distro architecture

`ISO/`, `SysApps/Installer`, `installravynos/` become the W8 deliverable: a bootable ISO,
a disk installer, an OS-update path (A/B slot swap so a failed update cannot brick the
machine — mandatory for bare-metal credibility), and a Mac VM demo package (the funding
artifact). Filesystem layout matches macOS conventions (`/Library`, `/System`, app
bundles) so app installers and NSBundle lookups behave as apps expect.

---

## 3. How things functionally work: the spec system

Every subsystem we implement gets a **functional spec** in `Docs/spec/<subsystem>.md`,
written before or alongside the first implementation, answering: what does this do, what
are its observable behaviors, what are the data structures and their invariants, what are
the performance-relevant paths. Specs cite public sources (Apple docs, headers, open-source
Darwin releases) — this is how the project accumulates the deep functional understanding
the user asked for, and how any contributor (human or agent) gets productive in a lane
without reading the whole kernel.

Spec lifecycle: `draft` → `implemented` (spec cites the code) → `verified` (spec cites the
test that proves each behavior claim). A subsystem without a verified spec is not done.

---

## 4. Language and toolchain choices

- Core set, matching ravynOS community practice and keeping the contributor pool wide:
  **C, C++, Objective-C, Objective-C++** for kernel, drivers and frameworks; **Python 3**
  for build tooling and test harnesses; **shell** only for thin orchestration
  (`build.sh` entry points); Swift considered only after the runtime can host it (post-W6).
  SwiftUI is a committed milestone, not a maybe: the Swift runtime hosts post-W6, and a
  stock SwiftUI app runs out of the box at Wave 8 (see the wave exits).
- Toolchain: clang/LLVM (already validated at clang 21.1.8 against xnu-12377), ld64.lld
  for Mach-O linking, our own mig. No GCC for kernel code (xnu headers assume clang).
- Host build on Linux is the supported path (that is the team's rig); macOS host builds
  are a convenience, not a requirement.

---

## 5. Performance engineering: budgets, measurement, enforcement

"Ultra-low latency" is defined here as a budget table plus a measurement regime. Design
decisions that trade latency away must name the budget they spend.

### 5.1 Latency and throughput budgets

| # | Metric | Budget | Measured by | Wave enforced |
|---|--------|--------|-------------|---------------|
| P1 | Boot: power → first app frame, bare-metal NVMe | ≤ 3 s | `Tests/perf/boot_time` on the bare-metal rig; QEMU trend only | W4 |
| P2 | Boot: power → shell prompt, QEMU dev rig | ≤ 10 s (trend) | `boot/build.sh` + `boot/qemu-boot.sh` timer | W1 |
| P3 | Null syscall round trip (getpid-class) | ≤ 300 ns | `Tests/perf/syscall_null` (rdtsc, warm cache, p50/p99) | W2 |
| P4 | Mach IPC round trip, same host, small message | ≤ 2 µs | `Tests/perf/mach_ipc_rt` | W2 |
| P5 | Thread context switch (same core) | ≤ 1 µs | `Tests/perf/ctx_switch` | W2 |
| P6 | WindowServer input → photon (input event to first pixel change) | ≤ 16.6 ms @ 60 Hz; stretch ≤ 8 ms @ 120 Hz | `Tests/perf/input_photon` with instrumented WindowServer | W4 |
| P7 | UI frame pacing: dropped frames under composition load | < 0.5 % over 60 s scripted test | `Tests/perf/frame_pacing` | W4 |
| P8 | Cold app launch, small SDK-built app | ≤ 500 ms | `Tests/perf/app_launch` | W6 |
| P9 | Kernel build: full batch, 8 cores | ≤ 5 min | build harness timing report | W1 |
| P10 | Build: per-file compile p95 | ≤ 1 s | build harness per-file timings | W1 |

QEMU numbers are indicative only (TCG distorts absolute latency); gates P3–P5 run in QEMU
as **relative regression gates** (CI baseline ratio), and absolute numbers are confirmed
on bare metal at W4.

### 5.2 Design rules that buy latency

These are the architectural choices made *now* so latency doesn't have to be won back
later:

- **Zero-copy everywhere the contract allows:** shared-memory window surfaces (§2.6),
  Mach OOL messages for large payloads, send-once rights instead of reply polling.
- **No blocking on the hot path:** WindowServer compositor and input routing run
  non-blocking; input batches at most one frame.
- **Lock discipline:** xnu's low-level locks are already wait-free where it matters; new
  kernel code follows xnu's own concurrency idioms rather than inventing wrappers.
- **No abstraction tax on hot paths:** the HAL (§2.4) is devirtualized via per-target
  constant configuration, not function-pointer tables, in code proven hot by measurement.
- **Allocation:** per-subsystem zone allocation follows xnu's allocator; userspace uses
  libdispatch pools. `malloc` in loop paths is a review flag.

### 5.3 Measurement regime

- Every budget has a named benchmark binary in `Tests/perf/` with a machine-readable
  output (JSON: p50, p99, samples, host, commit).
- Baselines are recorded per wave in `Tests/perf/baselines/` with the command used
  (the same discipline used for the 2026-11-25 baseline: 0.26 s small kern file, 0.81 s
  `vm_map.c`, 7.26 s → target for batch mode).
- CI gate: any budget regresses > 5 % vs. the wave baseline → build fails, with the diff
  printed. New subsystems without benchmarks cannot merge (CI blocks on missing budget
  coverage for files under `CoreServices/`, `Kernel/RVOS/`, `Libraries/`).

---

## 6. Licensing: the purity system

### 6.1 License verdict table

| Verdict | Licenses | Rule |
|---|---|---|
| **Accepted** | APSL 2.0 (Apple open source: xnu, dyld, objc4, Libsystem, mig), BSD-2/3/4, MIT, ISC, Apache-2.0, Zlib, ICU | Use freely; provenance record mandatory |
| **Review required** | MPL-2.0, EPL, CDDL, Python-2.0-style attribution stacks | Weak copyleft or attribution complexity; lane lead + license audit approval; file-level isolation required |
| **Forbidden** | GPL, LGPL, AGPL — any version, any artifact of derivation | Not in the tree, not in build tooling, not in generated output. Includes darling and its forks (standing decision 2026-11-24) |

Apple's APSL 2.0 is OSI-approved and is the license Apple itself ships Darwin under;
the kernel staying APSL is both unavoidable and correct. **Our** original code is released
under **BSD-3-Clause** (kernel, drivers, shims — matches the BSD heritage and keeps the
whole OS redistributable) and **MIT** (build tools, scripts, test harnesses). This mirrors
ravynOS community practice (their FAQ: clean-room originals "released under the FreeBSD
license or the MIT license") and yields an OS whose combined license story is: APSL
kernel + BSD/MIT everything ours. A final `LICENSES.md` at W8 states the whole picture
for distribution.

### 6.2 Provenance discipline

- Every vendored component gets a `Kernel/_PROVENANCE/`-style record: upstream URL, exact
  tag/commit, license, local modifications. The pattern already exists for xnu
  (`Kernel/_PROVENANCE/`); W0 extends it to all of `Libraries/`, `Frameworks/`, `BSD/`.
- Every original file carries an SPDX header (`SPDX-License-Identifier: BSD-3-Clause`).
- CI license gate (`tools/license-audit.py`): scans SPDX tags + a filename/blocklist
  heuristic; a new file without an SPDX tag or matching a forbidden-license fingerprint
  fails the build. Content-level fingerprinting (n-gram match against known GPL sources)
  is a W1 addition so GPL-derived *rewrites* are caught too.

### 6.3 Reuse audit for the in-tree ravynOS frameworks

The clone inherited ravynOS's framework sources. Before any of that code is built into
our system (W5+), each framework passes: (a) license check — upstream states clean-room
+ BSD/MIT; (b) quality gate — compiles clean, no unresolved symbols, benchmarks where
hot-path; (c) spec coverage — a functional spec exists. Fail any of the three and that
framework is reimplemented from spec rather than patched ad infinitum.

---

## 7. Reverse engineering: the clean-room protocol

Goal: know *how things functionally work* well enough to reimplement them natively, with
zero copyright exposure and zero GPL contamination.

### 7.1 Two-room model

- **Spec room** (may look at anything *behavioral*): observes Apple's public
  documentation, public headers, open-source Darwin releases, and the *observable
  behavior* of Apple binaries (strace-class observation on a Mac VM: what syscalls a
  process makes, what files it opens, what it writes to the log — not its code). The
  spec room may use any tool including LLM assistance. Its only output is
  `Docs/spec/<subsystem>.md`: behavior contracts, state machines, wire formats,
  error-code tables, pseudocode.
- **Implementation room** (sees only the spec + licensed sources): writes the real
  implementation from the spec and public headers. Never receives spec-room raw notes
  or binaries.

With agent lanes (§9), the same person may run both rooms at different times, but the
artifacts stay separated: the spec is the wall. Anything that looks like transcribed
compiled code is rejected in review.

### 7.2 Hard rules

- **Nothing derived from Apple's compiled code enters the tree.** We never paste translated
  pseudocode of compiled Apple code, or model-reconstructed code derived from it into any
  committed file. Models used for study may produce *specs*; implementations are written
  against the spec, from scratch, and review diff-checks them against spec (not against
  any binary).
- **Observation, not extraction.** Black-box behavioral observation of a shipped binary
  (running it, tracing its system calls) is the method. This is the same evidence Apple's
  own public docs and headers give, and it is what keeps the OS legally clean.
- **Every observed behavior is cited.** A spec claim about a closed-source behavior carries
  the observation that established it ("Claude Desktop 0.x calls `mach_msg` with
  msgh_id 0x... at startup; see spec notes 2026-11-xx"). Uncited behavior claims do not
  pass review.

### 7.3 Why this is the fast path, not the slow one

It converts unknowns into a testable document the whole team can implement against in
parallel, it produces the functional understanding the project needs anyway, and it is
the only study method that leaves the license story intact for funding due diligence.

---

## 8. xnu upgrade strategy

### 8.1 Vendoring model

`Kernel/xnu/` is a pristine import of the `apple-oss-distributions` xnu tag — currently
xnu-12377.121.6 at commit `fde4c10515`. The import commit is tagged locally
(`xnu-12377.121.6-import`). **No working-tree edits to `Kernel/xnu/` are committed.** All
our kernel-adjacent work lives in `Kernel/build/`, `Kernel/RVOS/`, and
`Kernel/Extensions/`.

### 8.2 When a genuine gap forces a kernel change

Real cases will exist (a missing export, a config that can't be set externally). The
process:

1. File an RFC in `Docs/rfc/` stating the gap, the alternatives exhausted
   (config-include, shim, overlay symbol override), and the minimal in-tree change.
2. Approve at wave level; implement as a patch in the quilt series
   `Kernel/RVOS/patches/NNNN-<name>.patch` with a `MANIFEST` (purpose, upstream status).
3. Patches are written in upstream style (§8.3) and kept *small and droppable* — if
   Apple later ships the same fix in a new tag, the patch is deleted, not maintained.

### 8.3 Style inside the series

Patches read like xnu code, because a future merge depends on it: BSD KNF layout, xnu
naming, no decorative comments — a one-line `/* rvos: why */` only where the reason is
non-obvious. No diff contains formatting churn.

### 8.4 Upgrade procedure

Upgrade windows are scheduled **between waves**, never mid-wave:

1. Import the new tag as a new vendored commit; tag `xnu-<ver>-import`.
2. Regenerate: mig headers (`Kernel/build/mig/build.sh`), export-headers flattening,
   `config-include` diffs (medium/large/xlarge MASTER profile).
3. Re-apply the patch series; delete patches the new tag obsoleted; resolve the rest.
4. Full batch build; per-tranche regression (same object list, 0-error gate); kernel
   boots in QEMU; P2/P9/P10 budgets re-measured.
5. Wave log records the new kernel version and any budget movement.

Cost of an upgrade stays bounded by design: zero in-tree edits means zero merge conflicts
— only patch-series conflicts, which the MANIFEST keeps small.

---

## 9. Parallel development model (agent lanes)

The project is designed so several agents (or humans) work simultaneously without
colliding: **lanes own directories, contracts own interfaces.**

### 9.1 Standing lanes

| Lane | Owns (exclusive write) | Mission |
|---|---|---|
| **A — Kernel build** | `Kernel/build/`, `Kernel/RVOS/` | Compile/link the vendored kernel; batch build; budgets P2, P9, P10 |
| **B — Core runtime** | `Libraries/` (dyld, objc4, Libsystem) | T1; budgets P3–P5 |
| **C — UI stack** | `CoreServices/` (WindowServer, Dock), `apps/` | T3; budgets P6–P8 |
| **D — Drivers/HAL** | `Kernel/Extensions/`, `Kernel/IOKit*`, `Kernel/RVOS/platform/` | Real hardware, every board port |
| **E — Harness/CI** | `tools/`, `Tests/`, CI config, `Docs/spec/` infra | License gate, perf gate, spec system, build engine 2.0 |
| **F — Spec** | `Docs/spec/` | Functional specs, clean-room behavioral observations, tier test apps |

### 9.2 Coordination rules

- **Contracts frozen at wave start** (§2.1): the header set + ABI + behavior spec for
  each layer boundary is agreed before parallel work begins; changes after that are wave
  decisions, not lane edits.
- **One lane per directory.** Cross-lane work happens by PR to the owning lane, never by
  direct edit. No two lanes share a file — if a change seems to need two owners, the
  boundary is wrong and is fixed at the contract level.
- **Definition of Done per work package:** code + spec updated + benchmark updated +
  CI green + the tier's exit criterion still passing on the reference rig.
- **Integration cadence:** the reference rig (QEMU boot + tier test) is rebuilt from
  mainline at least daily; a lane that breaks mainline boot owns the revert.
- **Dispatch:** each wave in §10 lists its active lanes and work packages, mirroring the
  A/B/C/D dispatch-table pattern Roadmap 3.0 used, now with explicit ownership and DoD.

---

## 10. Wave roadmap

Waves replace phases. A wave **exits** when its exit criteria pass on the reference rig
and its perf gates hold; partial waves do not unlock the next one. Mapping from the old
P0–P6: W1 ≈ P2 completion, W2 ≈ P4 (runtime), W3 ≈ P2 userland, W4 ≈ P3+P6, W5 ≈ new
(arm64), W6 ≈ P4 width, W7 ≈ P5, W8 ≈ distro (new).

### Wave 0 — Foundations (no kernel work)
**Exit:** CI enforces license + style + build; harness design doc merged; provenance
records exist for every vendored tree.
- **E:** build harness 2.0 skeleton — declarative component graph + batch engine
  (design already agreed: graph of components, generic engine, ninja emission, cache
  hooks); `tools/license-audit.py` with content fingerprinting; `.clang-format` + style
  checker; CI pipeline (build matrix, license gate, perf smoke).
- **E/F:** spec system seeded (`Docs/spec/` with the contract set of §2.5).
- **F:** tier test apps chosen and built with the macOS SDK (T0 static, T1 dynamic,
  T3 AppKit window app) — the exit criteria exist as binaries before the work starts.
- **All:** provenance audit of `Libraries/`, `Frameworks/`, `BSD/` (§6.3).

### Wave 1 — Kernel links and boots (T0)
**Exit:** full kernel link with **zero stub symbols**; static SDK Mach-O runs (T0);
P2/P9/P10 budgets recorded.
- **A:** tranche the 97-symbol workplan (`REAL-KERNEL-UNDEFS.txt`) to zero via real
  definitions (kheap sections, boot-args, blocks runtime, Mach primitives); remove
  `link/stub/` from the boot path; batch compile mode for all osfmk+bsd objects.
- **E:** perf benchmarks P2/P9/P10 + baseline recording in CI.
- **F:** syscall-surface spec (what T0 exercises).

### Wave 2 — Dynamic runtime (T1)
**Exit:** SDK-built dynamic Objective-C app runs end to end; P3–P5 measured in CI.
- **B:** dyld + objc4 built and running on our kernel; Libsystem syscalls completed
  against the spec; XPC spec drafted.
- **A:** syscall surface gaps surfaced by the runtime (the only sanctioned kernel change
  source); zalloc/vm tuning for userspace workloads.
- **F:** dyld/objc behavioral specs verified against the running app.

### Wave 3 — Userland on us (T2)
**Exit:** boot-to-login from disk image: launchd, our shell, BSD userland, no hot-path
stub syscalls.
- **B:** launchd (clean-room per §2.5), XPC minimal surface.
- **E:** full-boot test rig on a disk image (not initrd), nightly trend runs.
- **A:** VFS/block path hardening; virtio-blk from lane D.

### Wave 4 — Graphics + real hardware (T3)
**Exit:** SDK AppKit app: window + input at 60 fps on bare-metal x86_64 PC; P1/P6/P7
measured; installed-to-disk boot.
- **C:** WindowServer composition (zero-copy surfaces), input stack, first-window apps.
- **D:** bare-metal: AHCI/NVMe, xHCI HID, Intel/AMD framebuffer, real NIC; installer
  pre-work.
- **E:** bare-metal perf rig; P1/P6/P7 baselines.
- **F:** IOKit matching/power-management behavior specs.

### Wave 5 — arm64 + Apple Silicon path (portability proof)
**Exit:** x86_64 and arm64 build in one CI matrix from one graph; kernel boots on arm64
QEMU virt; our own stage-1 loader prototype boots our Mach-O on Apple Silicon under Apple's
boot policy (dev-mode machine acceptable for the prototype).
- **D:** HAL interface extraction completed for both arches (the §2.4 rule proven: a
  board port touched only `platform/`).
- **A:** arm64 kernel slice build; P9/P10 re-measured per arch.
- **E:** dual-arch CI matrix + per-arch budgets.

### Wave 6 — Frameworks width (T4) — *gated on T3*
**Exit:** 20-app set launches and functions; P8 measured; Swift runtime feasibility
verdict (go/no-go for post-W6).
- **C+B:** framework audits (§6.3) then width work, framework by framework, each with
  spec + benchmark; no `-dynamic_lookup` band-aids.
- **B:** Swift runtime hosted — dyld loads a dynamically linked Swift binary on our
  kernel, and the objc4/Swift interop passes the OBJC-RUNTIME-AUDIT.md checklist. The
  go/no-go verdict is made on evidence: the SwiftUI dependency graph (SwiftUI,
  SwiftUICore, AttributeGraph and everything below) inventoried from a real SwiftUI
  app's load commands, per Docs/SWIFTUI-FEASIBILITY.md.
- **F:** per-app gap specs from the 20-app run logs.

### Wave 7 — Closed-source Mac apps (T5) — *gated on T4*
**Exit:** designated commercial app installs and runs (POC ladder: Electron app → Qt
app → native AppKit app → SwiftUI app).
- **F:** clean-room RE of the target's observable behavior (§7).
- **B/C:** runtime/appkit gaps the ladder exposes, each as spec → impl → test.
- **B:** the SwiftUI rung — SwiftUI/SwiftUICore and their private dependencies
  (AttributeGraph and below) clean-room implemented to the width the ladder app needs,
  gated on Wave 6's go verdict; AppKit hosting (NSHostingView) is the same clean-room
  rule, no emulation and no Apple binaries. Reference implementation for the inventory:
  OpenSwiftUI ([github.com/OpenSwiftUIProject/OpenSwiftUI](https://github.com/OpenSwiftUIProject/OpenSwiftUI),
  MIT — license verified 2026-11-26) shows the surface is tractable; it is study
  material and an API-shape oracle, not a drop-in binary.

### Wave 8 — Distro (funding artifact)
**Exit:** bootable ISO + installer + A/B update + Mac VM demo package; `LICENSES.md`;
final perf report (all budgets vs. baselines); **a stock SwiftUI app installs and runs
out of the box from the installed image** — the demo artifact includes it, because
SwiftUI is what new Mac apps are written in and a distro that cannot run one fails the
parity argument on the first download.
- **All lanes** converge; **E** owns release engineering.

### Sequencing logic

W0→W1→W2 are strictly serial (each unlocks the next's test rig). From W3 the graph
widens: W3∥(early D work), W4∥(W5 arm64 prototype), W6∥(W5 Apple Silicon), keeping at
most 3–4 lanes active — more parallel lanes than the contracts can freeze produces
collision churn, not speed.

### External contract version pins

A KPI contract must live years, so each driver-class contract pins to a long-support
line, not the newest stable. Re-pinning is a decision recorded here and in the owning
wave, not a drive-by bump. Checked 2026-11-26 against kernel.org and freebsd.org.

| Contract | Pin | Support window | Rationale |
|---|---|---|---|
| Linux KPI (driver shim) | **6.18.y longterm** (6.18.65 at pin time) | EOL Dec 2028 | Longterm gets fixes for years; the 7.x stable line churns weekly and is for testing new drivers, not for a contract |
| FreeBSD KPI | **15.1-RELEASE** (stable/15 branch) | Branch EOL Dec 31, 2029 | Current production line; the branch-based model guarantees the 4-year window our shims need |
| GPU/DRM subset | DRM/KMS ABI **as of 6.18** | Matches the Linux KPI pin | One kernel baseline for all three contracts keeps the shim test matrix honest |

Precedent worth recording: FreeBSD 15.1's own wireless drivers run Linux v7.x-based
drivers through its LinuxKPI shim in production — the KPI-shim approach is proven
technology, not a bet.

### Language policy

The rule: **Rust for new code that does not have to match Apple's binaries; C/C++/Objective-C
where binary compatibility with Apple requires it.** Closed-source Mac apps and drivers expect
a binary-level match, so xnu (never edited — our changes live in the quilt patch series),
objc4, the AppKit-style frameworks and IOKit driver classes stay in their original languages;
switching there gains nothing and loses compatibility.

| Layer | Language | Why |
|---|---|---|
| Bootloader (`kite`) | Rust (no-std, `uefi-rs`) | Small new surface, and a bug here stops the machine from booting |
| Vendored xnu | C, untouched (patch series only) | Apple's source must remain diffable against upstream |
| Our kernel additions | C in xnu's style | Rust inside a C kernel needs a dedicated owner per component; allow it only for self-contained parts |
| IOKit-compatible drivers | C++ (Apple's driver model); Rust for new self-contained drivers | Precedent: Asahi Linux's Apple GPU driver is Rust |
| objc4, AppKit-style frameworks | Objective-C/C++ only | Running Mac apps unchanged is the product; sanitizers + fuzzing cover the safety gap |
| WindowServer, new system services | Rust first | New code with no Apple heritage — the highest safety per line |
| `rv`, mig tooling, CLI tools, tests | Rust | Entirely our own code |

The evidence: Android's memory-safety vulnerabilities fell from 76% of the total in 2019 to
24% in 2024 as new code moved to Rust, with zero memory-safety bugs found in its Rust code —
most risk comes from new code, which is exactly what this policy routes to Rust. Every Rust
component needs a named owner before it starts (the FreeBSD Rust lesson), and the W0 license
gate scans Rust package dependencies too, because some are GPL.

---

## 11. Testing and verification

- **Reference rig:** QEMU (BIOS + OVMF q35) for x86_64, QEMU virt for arm64, bare-metal
  PC from W4. Every tier exit runs here, from a clean build, every time.
- **Test layers:** (1) per-commit: build + style + license gates; (2) per-PR: tier
  smoke (boot + tier test app) in QEMU; (3) nightly: full boot trends + perf baselines +
  fuzzing of the syscall surface (fixed-seed, reproducible); (4) per-wave: tier exit on
  the reference rig with recorded artifacts (serial log, screenshots, perf JSON).
- **Compat tests are apps, not units.** The growing SDK-built test app set under
  `Tests/apps/` is the compatibility suite; kernel unit tests exist only for our own
  overlay code.
- **Failure policy:** a broken mainline boot is reverted by the owning lane within the
  day; fixes re-land through the normal gate.

---

## 12. Risks and standing mitigations

| Risk | Mitigation |
|---|---|
| Kernel link stalls on deep symbol gaps (677-undef scale work) | Tranche workplan with weekly exit per tranche; stub removal is a wave exit, never a rolling hope |
| dyld/objc ABI drift vs. modern apps | Kernel of record is macOS-26 era precisely to avoid the Catalina trap; T1 gate uses current SDK builds, not old ones |
| GPL contamination via contributor or AI output | CI content fingerprinting (§6.2); review checklist includes provenance question; RE outputs are specs only (§7). Live example (2026-11-15): Asahi relicensed m1n1 MIT → GPLv2 — code adopted under an old license can flip, so provenance records the license at vendor time and CI re-checks it at every pin refresh |
| Competitor pace: PureDarwin's own AppKit runs a stock macOS Firefox build unmodified (Oct 2026) | Our differentiation — unmodified closed-source GUI apps on the real client protocol — is exactly what their Wayland/own-RPC stack cannot host; W6's app set and the T4 exit are the measured answer (Docs/design/01-principles.md, 07-graphics.md) |
| Apple Silicon bring-up cost | The boot-policy method is proven (Asahi) but m1n1's code is GPLv2 since 2026-11-15, so our stage-1 loader is clean-room — extra W5 effort; scope it as a prototype on a dev-mode machine before promising hardware support |
| Bus factor / contributor wall | Low barrier: no toolchain-from-source (clang from distro + our harness), spec system makes lanes independently startable, style rules make diffs reviewable |
| Perf claims outrunning reality | Budgets are CI gates with baselines; the final report shows all budgets vs. baselines, measured commands included |

---

## Appendix A. Current-state inventory (2026-11-25, branch `darwin` @ `1491d4bcac`)

Verified inventory this design builds on:

- Kernel: xnu-12377.121.6 vendored (commit `fde4c10515`); 139 objects compile clean;
  link workplan at 97 symbols (`Kernel/build/link/REAL-KERNEL-UNDEFS.txt`, zalloc.o
  tranche); `guest_kernel` is currently the stub-linked test binary (4,384 bytes);
  boot rig proven end to end (GRUB multiboot2 → loader → Mach-O at 0x100000000, BIOS and
  q35/OVMF).
- Harness: `Kernel/build/` — compile driver, export-headers flattening, own APSL mig
  (migcom builds on Linux), `config-include/` stand-ins, `patch_macho.py` + `verify.py`
  gate, stub builder.
- Drivers: `Kernel/Extensions/FreeBSDPort/` shim with FreeBSD 11.4 vtnet sources;
  empty-object root cause fixed (module records now externally linked).
- Userspace trees present and awaiting audit (§6.3): `Libraries/` (dyld, objc4,
  Libsystem), `Frameworks/` (AppKit, CoreGraphics, CoreText, Foundation, ...),
  `CoreServices/` (WindowServer, Dock, Filer), `BSD/` userland, `SysApps/`, `ISO/`.
- Docs: `Kernel/build/BLOCKERS.md`,
  `Kernel/build/HARDENING.md`, `boot/NOTES.md`.

Nothing in this appendix is a claim of running behavior beyond what the boot rig logs
show; tier exits T0–T5 are all still open.

---

## 14. Coding standard: code that reads like ours, not like generated output

Goal: a senior systems programmer browsing the tree sees consistent, idiomatic,
deliberately-written code. This is both a quality bar and a contributor signal — upstream
communities and future hires judge the project by its diffs.

### 14.1 Match the neighborhood

- **Kernel and anything touching xnu:** the style of the file you are in. xnu's `bsd/`
  follows BSD KNF (tabs for indent, braces as the file does, `/* */` comments), `osfmk/`
  follows Apple's kernel style. Never reformat, never mix styles in one diff.
- **Our own new code (RVOS overlay, tools, WindowServer):** one checked-in `.clang-format`
  (LLVM-based, 4-space, 100-col) plus `.editorconfig`; formatting is what CI enforces so
  humans never argue about it.
- **ravynOS community practice** (their FAQ/wiki): the C family preferred for
  frameworks and apps, clean-room originals, no proprietary assets, resources reached via
  NSBundle/CFBundle relative paths rather than fixed paths. We adopt the same practices.

### 14.2 Writing rules (anti-pattern list)

- **No decorative comments.** A comment states a *why* that the code cannot say
  (invariant, provenance, non-obvious constraint) — in one line. Nothing that restates
  the code, no banners, no "This function ...", no changelog-in-comments (git history
  exists). Standing preference: code that gets committed carries as few comments as the
  reviewer will accept.
- **No AI-tell prose** in code, comments, commit messages or docs: no emoji, no
  marketing adjectives ("blazing", "robust", "seamless"), no em-dash chains, no bullet
  lists where a paragraph belongs, no "Note that", no restated summaries at the end of a
  file.
- **Names from the domain:** Mach/xnu/BSD vocabulary (`zone`, `right`, `port`,
  `ipc_space`), not generic factory-speak. An Objective-C method reads like AppKit's own.
- **Small diffs:** one logical change per commit; a commit that touches > ~300 lines
  needs the wave lead's ok. No drive-by refactors inside feature diffs.

### 14.3 Commits and review

- Commit identity and push policy follow the standing decisions (author
  `Shaikh Ayeed Mohd Ali <225547338+ayeed-xec@users.noreply.github.com>`, branch
  `darwin-clean` on the fork).
- Subject line: imperative, ≤ 50 chars, names the subsystem (`vm: finish kheap tranche`).
  Body: the why, and the measurement for anything perf-related.
- **Review checklist** (lane lead, every PR): provenance question asked (§6)? benchmark
  updated if hot path (§5.3)? spec updated if behavior changed (§3)? comments pass §14.2?
  tier exit still passing on the reference rig?
