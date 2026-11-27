# ravynOS system design

ravynOS runs unmodified Mac applications on Apple's open-source XNU kernel, with our own
implementations of everything Apple keeps closed and an SDK shaped like Apple's.

## Start here

| Document | Read it for |
|---|---|
| [SYSTEM-DESIGN.md](SYSTEM-DESIGN.md) | The design of record: rules, architecture, and every subsystem with how it works, how to build it and when it is done |
| [ROADMAP.md](ROADMAP.md) | Waves, parallel lanes and work packages: what is built when, and by which lane |
| [STATUS.md](STATUS.md) | Where the code stands: bugs and gaps, numbered |
| [DECISIONS.md](DECISIONS.md) | Why each major choice was made |
| [../spec/TEMPLATE.md](../spec/TEMPLATE.md) | The template for specifying closed behavior |

Implementing agents: read SYSTEM-DESIGN.md sections 0–2, then your subsystem, then its
entries in STATUS.md. Nothing else is required.

## Background documents

These explain the reasoning behind the design in more depth. Where they disagree with
SYSTEM-DESIGN.md, SYSTEM-DESIGN.md is right. [../DESIGN.md](../DESIGN.md) is the earlier
project-wide plan, kept for its reasoning.

| # | Document | What it covers |
|---|----------|----------------|
| 1 | [01-principles.md](01-principles.md) | Mission, the three pillars (performance, modularity, native design), the no-stubs doctrine, licensing, how we differ from PureDarwin, host/arch matrix |
| 2 | [02-architecture.md](02-architecture.md) | The full stack, project structure and dependency rules, runtime process architecture |
| 3 | [03-kernel.md](03-kernel.md) | Vendored XNU, building on Linux, per-architecture ports, driver model, in-kernel Linux/FreeBSD driver environment, compute and memory |
| 4 | [04-boot.md](04-boot.md) | Our UEFI bootloader: features, per-architecture boot paths, the hand-off contract |
| 5 | [05-build-system.md](05-build-system.md) | The `rv` harness: manifests, hermetic cross-builds, license gate and tooling boundary, caches, single-component workflows |
| 6 | [06-filesystem.md](06-filesystem.md) | APFS as the root filesystem and our clean-room implementation plan, secondary filesystems, encryption |
| 7 | [07-graphics.md](07-graphics.md) | WindowServer design, the Metal implementation plan, GPU backends |
| 8 | [08-frameworks.md](08-frameworks.md) | Framework strategy by sourcing tier, coverage inventory, MLX, the Xcode/toolchain story |
| 9 | [09-compat.md](09-compat.md) | Native Linux and FreeBSD compatibility for binaries and kernel drivers (NVIDIA case study), ML workloads |
| 10 | [10-mobility.md](10-mobility.md) | The iOS-alternative face: phone stack, SoC bring-up playbook, scope honesty |
| 11 | [11-quality.md](11-quality.md) | Implementation guidelines: code standards, anti-bloat rules, tests, performance budgets as CI gates, review checklist |
| 12 | [12-roadmap.md](12-roadmap.md) | Earlier wave plan, superseded by ROADMAP.md; the reasoning behind each wave's hard part |
| 13 | [13-toolchain-sdk.md](13-toolchain-sdk.md) | Native toolchain bootstrap, the SDK as a build output, the legal boundary between Apple open source and Apple proprietary artifacts |


Diagrams live in [diagrams/](diagrams/).
