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

## WP-PORT-arm64-baseline (2026-11-26)

Created `Kernel/RVOS/platform/arm64/` — the arm64 port skeleton lives OUTSIDE
`Kernel/xnu` (lane D), mirroring the API names/signatures of
`osfmk/arm/pmap.h` / `osfmk/vm/pmap.h` without copying Apple code:

- `asm.h` — `RV_ARM64_MSR`/`RV_ARM64_MRS` macros plus `rv_arm64_isb`,
  `rv_arm64_dsb_sy`, `rv_arm64_dmb_sy/_ish/_ishst` static inline primitives.
- `cpu.h` — `struct rv_cpu_data` stub (per-cpu number, TTBR0/VBAR holders,
  active pmap/thread pointers), `rv_cpu_data[RV_ARM64_MAX_CPUS]`.
- `pmap.h` — ARMv8 stage-1 constants: 4-level 48-bit VA (T0SZ=16, 512
  entries/table, 4K granule), block/table/page descriptor type bits, AP/SH/AF/
  PXN/UXN/NG bit definitions, ATTRINDX values (0=normal WB, 1=device nGnRE,
  2=normal NC) and the MAIR encodings they imply; typedefs matching the
  upstream `pmap_t`/`vm_map_address_t`/`ppnum_t` names.
- `pmap.c` — typed, empty-bodied implementations of `pmap_bootstrap`,
  `pmap_init`, `pmap_map`, `pmap_map_bd`, `pmap_map_block`,
  `pmap_enter_options_addr`, `pmap_remove`, `pmap_protect`,
  `pmap_page_protect`, `pmap_switch`, `pmap_clear_user_ttb`, `kvtophys`,
  `phystokv`, `pmap_find_phys`, `pmap_valid_address` — signatures taken from
  the arm64 upstream declarations (clean-room; bodies intentionally empty).
- `exception.h` / `exception.S` — VBAR-aligned vector table
  (`rv_arm64_vectors`, 16 entries x 128 bytes = 2048 bytes, `.balign 2048`),
  each entry pushes the general frame (x0-x30 + ELR/SPSR/ESR, struct
  `rv_arm64_trap_frame`, 272 bytes) and branches to extern C
  `arm64_exception_entry`; `rv_arm64_vbar_el1_set()` programs VBAR_EL1.

Compile verification (clang 21.1.8, every file exit 0):
`clang --target=arm64-ravynos -ffreestanding -fno-builtin -nostdinc -Wall
-Wextra -c` → `pmap.c` OK, `exception.S` OK; header-conformance TU
(asm.h + cpu.h + pmap.h + exception.h) OK; `ld.lld -m aarch64elf` links the
objects OK. `tests/boot/smoke-arm64.sh` exit 0 afterwards (edk2/QEMU virt
boot, symbolicated panic backtrace assertions intact, script untouched).

Honest open list (the skeleton does none of this yet):
1. TTBR0/TTBR1 setup: `pmap` currently has only an L0 pointer; nothing
   allocates or installs page-table pages, and `pmap_switch` never writes
   TTBR0_EL1 (TTBR1/high-half split undefined).
2. MAIR_EL1: ATTRINDX values are defined but MAIR_EL1 is never programmed;
   the NORMAL/DEVICE attribute indices must match the eventual MAIR layout.
3. TCR_EL1 programming: T0SZ/T1SZ=16, 4KB granule, cacheability/SHAREABILITY
   fields not written anywhere; MMU enable path (SCTLR_EL1.M) not attempted.
4. `pmap_init` must actually do: allocate the kernel L0-L2 tables, bootstrap
   `pmap_kernel`, wire TCR/MAIR, and publish the kernel pmap via
   `pmap_switch` — currently all no-ops.
5. Exception entry: `arm64_exception_entry` C side, exception-class dispatch
   (ESR.EC), vector selection for lower ELs (SP0/AArch32 rows are copies of
   the SP1 row in the stub), and frame unwinding on return.

## WP-PORT-rv64-baseline (2026-11-26)

Created `Kernel/RVOS/platform/riscv64/`, the first riscv64 kernel port
skeleton outside the vendored xnu tree (xnu untouched):

- `asm.h`: csrr/csrw/csrs/csrc/csrrw macro primitives plus `fence.i`
  and `sfence.vma` barriers.
- `cpu.h`: `struct riscv64_cpu_data` stub (hartid, cpu number,
  current pmap, trap frame, kernel stack top) with a master cpu_data.
- `pmap.h` + `pmap.c`: Sv39 two-stage address-space constants
  (4KB pages, 3 levels, 512 PTEs/level, 39-bit VA, PTE V/R/W/X/U/G/A/D,
  satp/ASID and hgatp/VMID mode fields, `PMAP_STAGE_USER`/`_KERNEL`)
  and a typed-but-honest `pmap_init` prototype set: `riscv64_pmap_init`
  fills the struct and computes satp, `riscv64_pmap_activate` writes
  satp + sfence.vma, `riscv64_pmap_satp`/`riscv64_pmap_empty` are
  helpers. No page-table walkers, no mapping/allocator yet.
- `sbi.h` + `sbi.c`: ecall wrapper (a0/a1 ret, a6 fid, a7 ext) with
  `riscv64_sbi_console_putchar` (legacy ext 0x01), `riscv64_sbi_shutdown`
  (SRST 0x53525354) and `riscv64_sbi_get_spec_version` (base ext 0x10).
- `trap.h` + `trap.S`: `riscv64_trap_vector` is `.p2align 2` (stvec
  requires 4-byte alignment), saves the 31-register integer frame
  (x1-x31, 256-byte slot frame with sp at offset 240), loads
  scause/sepc/stval into a0-a2 and the frame into a3, calls extern C
  `riscv64_trap_entry`, restores and srets. No C handler yet.

Compile verification (all into /tmp, all exit 0, no warnings with
`-Wall -Wextra`):

- `clang --target=riscv64-ravynos -ffreestanding -fno-builtin -nostdinc -c`
  on `pmap.c`, `sbi.c` (also verified with the rv harness flag set:
  `-fno-pic -mcmodel=medany -fno-stack-protector -fno-stack-check
  -mno-relax -msmall-data-limit=0 -O2`).
- same driver on `trap.S`; with `-mno-relax` the vector symbol sits at
  offset 0 (4-byte aligned); without it the alignment pad is an
  `R_RISCV_ALIGN` relaxation hole that `ld.lld` resolves at link time
  (verified via `readelf -r`/link smoke), which is why the harness links
  riscv64 with `-mno-relax`.
- headers (`asm.h`, `cpu.h`, `pmap.h`, `sbi.h`, `trap.h`) compile clean
  via a throwaway TU with the same flags.
- all C files pass repo `.clang-format` (`--dry-run --Werror`).

Nothing is wired into `tools/rv` yet and `tests/boot/smoke-riscv64.sh`
is unchanged; it still exits 0 (edk2 boot of the PE testkernel on QEMU
virt, symbolicated `probe-reached` backtrace asserted).

Open list (what a real pmap_init/trap handler still needs):

1. Physical page allocator + Sv39 page-table walker (map/unmap,
   permission bits, A/D handling) so `pmap_init` becomes real; needs a
   memory map source (edk2/DTS) and a boot-time free-list.
2. Per-hart `cpu_data` from `mhartid` (master stub currently hardwires
   hart 0) and ASID/VMID allocation for the two-stage design.
3. C `riscv64_trap_entry` implementation: scause decode (interrupts,
   ecall from VS/U, page faults), trap-frame save of `sscratch`-swapped
   kernel stacks, and timer enablement via SBI SET_TIMER.
4. stvec wiring at boot (currently nothing points `stvec` at
   `riscv64_trap_vector`) and m-mode trap delegation in SBI/edk2 path.
5. Console/panic path on top of `riscv64_sbi_console_putchar` so the
   port can boot far enough to replace the edk2 testkernel.

## WP-A3-triples (2026-11-26)

compile.sh takes ARCH=x86_64|arm64|riscv64; per-arch target, arch defines
and machine include dirs; every failing file is counted, not just the first.

- x86_64: exit 0, all 17 files (10 osfmk incl. startup.c, 7 bsd headers).
  Fixes this round: osfmk/i386 must come AFTER export-hdrs on the include
  path (string.h otherwise resolves to the i386 variant on every arch);
  libkern/version.h is stamped at run time; skywalk include path is
  skywalk/packet/os_packet.h; new shim Kernel/build/driver/shim/skywalk/
  os_nexus.h forwards <skywalk/os_nexus.h> to bsd/skywalk/nexus/os_nexus.h
  (upstream path mismatch, xnu untouched).
- arm64: exit 17, all 17 files fail at the same class: osfmk/mach/arm/
  vm_param.h:42 "processor not supported" (Apple's guard enumerates arm/
  arm64 CPUs only), then MAX_CPUS/MAX_CPU_CLUSTERS/CPMU_PMC_COUNT undeclared
  (arm64 machine_config not in our config-include) and an ipc_port size
  static assert keyed to the x86_64 layout. Next: config-include arm64
  machine_config values and a mach/arm/vm_param shim; no include-order
  problem remains — the arm64 dirs already win the include race.
- riscv64: exit 17, all 17 files fail at bsd/sys/cdefs.h:1231 and
  bsd/machine/types.h:39 "architecture not supported" (upstream enumerates
  i386/x86_64/arm/arm64 only), then uintptr_t/int8_t... unknown (types.h
  bailed before defining them). Next: a config-include cdefs/types shim for
  riscv64 that defines the __riscv branch; after that the same arm64-class
  config work applies.
