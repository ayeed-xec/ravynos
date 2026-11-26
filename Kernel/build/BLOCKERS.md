# Kernel build blockers — xnu-12377.121.6 on Linux

Updated: 2026-11-26. Tool: clang 21.1.8 + lld (Ubuntu), byacc; the lexer is pregenerated
and committed, so neither flex nor its GPL `m4` dependency runs at build time.

## Closed 2026-11-25 (evening round)

- `bsd/kern/bsd_init.c`: 0 errors. Two root causes fixed:
  1. `thread_call_t` was undeclared in bsd compiles — bsd/net headers use it
     but nothing in the include chain provides it (kern/thread.h exposes it
     only under MACH_KERNEL_PRIVATE). Stand-in typedef added to
     `config-include/kernel_base_types.h` (the clang-modules equivalent).
  2. `#define TCP_SYN_COOKIE_DISABLED(tp)` used `tp->t_mpflags & TMPF_MPTCP_TRUE`
     unguarded while the `t_mpflags` member and TMPF_* defines sit behind
     `#if MPTCP` (tcp_var.h:741, 2007). Now preprocessor-gated on MPTCP
     (tcp_var.h:923). Same class of fix in mptcp_var.h: `tptomptp()` touches
     `tp->t_mptcb` unguarded; now `#if MPTCP` (mptcp_var.h:460).
- `osfmk/kern/zalloc.c` was already 0 errors before this round (link driver
  regression, exit 0).
- Added to `kernel_config.h`: TRAFFIC_MGT=1 (MASTER:71, gates
  CLEAR_IAJ_STATE/iaj_rcv_ts), MACH_BSD=1 made explicit.
- Still failing: none of the audited files. Next WP-A1 step: object-compile
  (`compile.sh -c`) of the same files, then the mig-generated headers build.

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
- GPL audit fix (2026-11-25): parser regenerated with byacc
  (public-domain yaccpar) instead of `bison -y`, whose skeleton
  carried the GPLv3 notice. `mig/generated/y.tab.c`/`.h` are now
  byacc output; shim 64-bit typedefs aliased to `__INT64_TYPE__`
  on Linux so glibc and the Apple headers agree.
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
  `san/` forwarders (`kasan.h`, `kcov.h`, `kcov_data.h`,
  `kcov_ksancov.h`, `kcov_ksancov_data.h`, `kcov_stksz.h`,
  `kcov_stksz_data.h`). Put `config-include` first on the include path
  and force-include `kernel_config.h`.
- Export-headers step (WP-A2a): `Kernel/build/driver/export-headers.sh`
  walks the component Makefiles and flattens 1020 headers into
  `driver/export-hdrs/` (incl. `debug.h`). Caveat: the flat dir can
  collide arch-specific files (`types.h` exports the ARM variant);
  real arch dirs go EARLIER on the include path than `export-hdrs`.
- `libkern/version.h` is build-generated (`newvers.pl` stamps
  `xnuVersion` into the template). A copy stamped with `12377.121.6`
  is committed at `driver/export-hdrs/libkern/version.h`; regenerate
  after kernel version bumps. Do NOT run newvers.pl on the in-tree
  template — it rewrites the file in place.
- WP-A2a syntax-check driver: `Kernel/build/driver/compile.sh`
  (clang -fsyntax-only, `-target x86_64-apple-darwin`, MACH/BSD
  KERNEL_PRIVATE, `-include stddef.h` because `size_t` in xnu comes
  from clang modules, which we do not use).

## Next blocker: full kernel .c compile (WP-A3)

`osfmk/kern/startup.c` now passes `compile.sh` CLEAN (0 errors) with
the full chain: export-hdrs + config-include (incl.
`kernel_base_types.h` for `u_int`/`uint_t`) + clean-room
`mig/shim/TrustCache/API.h` + generated `libkern/version.h`.

Remaining, in order:

1. `CONFIG_ZONE_MAP_MIN` and friends: more build-time config values
   that the real MASTER build derives; add to `config-include`'s
   `kernel_config.h` as files demand them.
2. `semaphore_consume_ref_t` unknown in mig-generated
   `mach/task_server.h`: mig gen headers reference semaphore typedefs
   that need `mach/semaphore.h` on the path (include-order or shim).
3. Missing prototypes in `zalloc.c` (`ledger_credit`,
   `__nosan_strlcpy`) and an int/pointer mismatch at zalloc.c:8217:
   real header/decl work, batch by batch.
4. `bsd/kern/bsd_init.c`: 20 errors of the same classes.
5. Then write the MIT build driver (WP-A2b/c link+boot story already
   proven, see `Kernel/build/link/` and `Kernel/build/boot/`).
