<!-- SPDX-License-Identifier: BSD-2-Clause -->
<!-- Copyright (c) 2026 The ravynOS Authors -->

# xnuhook

The build hook of the `xnu` component (SYSTEM-DESIGN.md S05). It follows the
S01 hook contract, version 1: `xnuhook --request <request.json>` writes
`<out>/plan.json` and runs nothing itself.

State: incomplete. The plan holds one step today:

- **Parsers (ADR-17).** `libkern/c++/OSUnserialize.y` and
  `libkern/c++/OSUnserializeXML.y` are regenerated with byacc, each with the
  symbol prefix its header comment names (`-p OSUnserialize`,
  `-p OSUnserializeXML`). The Bison copies upstream ships are pruned from
  `Kernel/xnu` (VENDORS `prune`); the hook refuses to plan if one reappears.

Not planned yet, all W1 work:

- configure (doconf's MASTER selection, `unifdef` and attribute expansion);
- `xnu-config` per component;
- the `makesyscalls.sh`, `create-syscalls.pl` and `compile-syscalls.pl`
  generators, rewritten in Rust;
- mig over the kernel `.defs`;
- `version.c`, export lists and symbol sets;
- the kernel-header fix-ups the grammars' `to build` comments apply to
  Bison output (dropping the `<stdio.h>` include), which need the kernel
  compile to check against;
- compile, link and install.

`rv` does not run hooks yet, so nothing consumes the plan outside the tests.

Test: `cargo test --locked` with the stage-1 byacc on `PATH` (or `BYACC` set);
it fails, rather than skipping, when no byacc is found.
