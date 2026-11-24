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
- Mass generation works: `mig/gen/` holds the generated headers and
  user/server .c files for the osfmk `.defs` set the kernel build
  needs (86 headers). Generated with the vendored migcom via
  `mig/build.sh`; regenerate in place after kernel source updates.
- Config-include dir: `Kernel/build/config-include/` provides the
  committable generated-config headers the real kernel build creates
  at build time (`kernel_config.h` with CONFIG_* defines matching the
  default x86_64 MASTER options, `mach_kdp.h`, `mach_ldebug.h`) plus
  `san/` forwarders to the in-tree `san/memory/kasan.h` and
  `san/coverage/kcov_data.h`. Put `config-include` first on the
  include path and force-include `kernel_config.h`.
- Verified syntax check (clang -fsyntax-only, shims + gen +
  config-include on the path) passes with 0 errors for:
  `kern/coalition.h`, `kern/thread.h`, `kern/task.h`, `kern/zalloc.h`,
  `osfmk/kern/locks.h`, `kern/processor.h`, `kern/timer.h`,
  `mach/mach_vm.h`, `mach/mach_host.h`. This resolves the two
  earlier stops: `struct run_queue` (behind
  CONFIG_SCHED_TIMESHARE_CORE) and the coalition typedef (behind
  CONFIG_COALITIONS) now resolve.

## Next blocker (WP-A1, remaining): full kernel build

1. Header include order: xnu kernel headers are not self-contained
   (e.g. `kern/sched.h` expects `kern_types.h`/timer_call types to be
   included first; `pexpert.h` needs `-I iokit`). The kernel .c
   compile order must follow xnu's own Makefile inclusion order, not
   standalone header checks. Include paths needed so far:
   xnu, osfmk, osfmk/i386, pexpert, iokit, libkern, libkern/libc,
   bsd, EXTERNAL_HEADERS, build/config-include, build/mig/shim,
   build/mig/gen.
2. First kernel .c syntax attempt previously stopped at `debug.h`
   include resolution (osfmk/kern/startup.c); retest now that the
   generated headers and config-include exist.
3. Kernel linking needs a Mach-O toolchain story on Linux (ld64/llvm-lld
   with Mach-O support) and the xnu KDK-style syscall metadata; the MIT
   build driver modeled on blacktop/darwin-xnu-build should be written
   only after (2) since its job is orchestration, not fixing includes.
