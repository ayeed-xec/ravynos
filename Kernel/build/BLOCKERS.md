# Kernel build blockers — xnu-12377.121.6 on Linux

Updated: 2026-11-24. Tool: clang 21.1.8 (Ubuntu), bison/flex present.

## Solved this session

- mig (migcom) now builds on Linux: see `Kernel/build/mig/`. Apple's
  bootstrap_cmds-99.231.1 sources are vendored under `mig/src` (APSL,
  license file included). The 99.260.1 tag is unusable: its `handler.c`
  references undeclared identifiers (`IsCamelot`, `WriteIncludes`);
  handler.c is not part of the working build (the Xcode target does not
  compile it) and is excluded here.
- Linux port = compile-flag + include shims only, no source edits:
  `-std=gnu17` (C23 makes `bool` a keyword; migcom uses `boolean_t bool`
  as a parameter name), `-Ulinux`, `-Wno-deprecated-non-prototype`, and
  the header shims in `mig/shim/` (`__options_decl`/`__enum_decl` as
  fixed-type enum typedefs, `machine/types.h`, `Availability.h`,
  `sys/_types/*`, `i386/_types.h`).
- Verified: `mach/clock.defs` (3 routines) and `mach/mach_host.defs`
  (25 routines) generate user.c/server.c/header/sheader end to end
  (`Kernel/build/mig/test-gen/`).

## Next blocker (WP-A1, remaining): full kernel build

1. mig-generated kernel headers: xnu's build expects a full set of
   generated headers (machine/types.h and the rest of the
   `osfmk/machine` user.h mappings) before any kernel .c compiles.
   The harness to mass-generate them from the xnu .defs/.template files
   is the next work item; the per-file driver (`mig/mig`) is proven.
2. First kernel .c syntax attempt previously stopped at `debug.h`
   include resolution (osfmk/kern/startup.c); expected to change once
   the generated headers exist.
3. Kernel linking needs a Mach-O toolchain story on Linux (ld64/llvm-lld
   with Mach-O support) and the xnu KDK-style syscall metadata; the MIT
   build driver modeled on blacktop/darwin-xnu-build should be written
   only after (1) since its job is orchestration, not fixing includes.
