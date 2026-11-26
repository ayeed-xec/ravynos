# ravynOS system design

This is the design documentation for the operating system itself: what it is, how each
subsystem works, and the order we build it in. It replaces the older design and roadmap
documents (Roadmap 3.0 and the phase plans, now removed); this set is the single source
of truth.

The target is the macOS 27.2.1 generation (build 26B131, Darwin 27.2, kernel
xnu-13432.41.4), assembled from Apple's own open-source releases plus our own native
implementations of everything Apple keeps closed. No stubs, no emulation as architecture,
no GPL code anywhere in the tree.

## The documents

| # | Document | What it covers |
|---|----------|----------------|
| 1 | [01-principles.md](01-principles.md) | Mission, the three pillars (performance, modularity, native design), the no-stubs doctrine, licensing, how we differ from PureDarwin, host/arch matrix |
| 2 | [02-architecture.md](02-architecture.md) | The full stack, project structure and dependency rules, runtime process architecture |
| 3 | [03-kernel.md](03-kernel.md) | Vendored XNU, building on Linux, per-architecture ports, driver model, in-kernel Linux/FreeBSD driver environment, compute and memory |
| 4 | [04-boot.md](04-boot.md) | Our UEFI bootloader: features, per-architecture boot paths, the hand-off contract |
| 5 | [05-build-system.md](05-build-system.md) | The `rv` harness: manifests, hermetic cross-builds, license gate, caches, single-component workflows |
| 6 | [06-filesystem.md](06-filesystem.md) | APFS as the root filesystem and our clean-room implementation plan, secondary filesystems, encryption |
| 7 | [07-graphics.md](07-graphics.md) | WindowServer design, the Metal implementation plan, GPU backends |
| 8 | [08-frameworks.md](08-frameworks.md) | Framework strategy by sourcing tier, coverage inventory, MLX, the Xcode/toolchain story |
| 9 | [09-compat.md](09-compat.md) | Native Linux and FreeBSD compatibility for binaries and kernel drivers (NVIDIA case study), ML workloads |
| 10 | [10-mobility.md](10-mobility.md) | The iOS-alternative face: phone stack, SoC bring-up playbook, scope honesty |
| 11 | [11-quality.md](11-quality.md) | Implementation guidelines: code standards, anti-bloat rules, tests, performance budgets as CI gates, review checklist |
| 12 | [12-roadmap.md](12-roadmap.md) | Work waves W0-W10 with exit criteria, dependencies and parallel lanes |

Diagrams live in [diagrams/](diagrams/), one SVG per figure, referenced from the documents.

## Reading order

If you read only two, read 01 and 02: they define the constraints every other document
assumes. Contributors should then read 11 before writing code, and the document that owns
the subsystem they are working on. 12 tells you where the project stands and what can start
in parallel.

## Status

These documents are design, not records: subsystems described here do not exist yet, and
each document's "Open questions" section is the honest list of what is not settled. The
roadmap's exit criteria are how a claim of progress gets checked.
