# Kernel boot/link hardening checklist (WP-A5)

Scope: our own boot path — `boot/loader/{stub.S,stub.c}` loads a static Mach-O
kernel image at 0x100000000, `link/patch_macho.py` rewrites LC_UNIXTHREAD and
drops dynamic-linking load commands, `link/verify.py` gates the result.
Priorities: P1 = before first P2 kernel boot, P2 = before P2 release, P3 = hygiene.

## P1

- Enforce W^X in the link gate: `verify.py` now fails any segment whose maxprot
  or initprot is write+execute (the old RWX __TEXT bug class); keep this gate in
  CI. (Done in this change.)
- Set EFER.NXE in `stub.S` (currently `or $0x100, %eax` sets LME only, line 64)
  and add the NX bit (bit 63) to every non-execute PDE (`mov $0x83, %eax` at
  lines 74/93 builds P|RW|2MB pages with no NX). Without NXE the whole 2MB-page
  identity map — kernel text, data, stack — is executable, so the loader hands
  the kernel an all-RWX address space no matter what the Mach-O says.
- Copy Mach-O segments with per-segment page permissions in the loader
  (`stub.c` copies file bytes but never reads initprot/maxprot): map __TEXT
  r-x, __DATA rw-, and reject write+execute segments at load time, so the
  on-disk gate cannot be bypassed by booting an unverified image.

## P2

- KASLR in the loader: vmaddr is a fixed 0x100000000 (link.sh `-no_pie`,
  patch_macho clears MH_PIE by design). Feasible with current plumbing: pick a
  page-aligned slide at load time, add it to every LC_SEGMENT_64 vmaddr and the
  LC_UNIXTHREAD rip/rsp copied by `stub.c`, and have xnu relocate via its own
  `kernel_vm_address_slide`. Keep the loader as the single slide authority.
- Tear down the loader identity map once the kernel VM is up: stub.S maps all
  of physical memory 1:1 at PML3 slot 0 and in the high map; xnu should switch
  to its own page tables and stop using the bootstrap CR3 before enabling user
  fault handling, or that RWX window persists.
- Remove debug/KDP from release: verified good today — `config-include/mach_kdp.h`
  sets `MACH_KDP 0` and `CONFIG_KDP_INTERACTIVE_DEBUGGING 0`, `mach_ldebug.h`
  sets `MACH_LDEBUG 0`. Pin these in a release-config check so a debug kernel
  config cannot silently re-enable KDP (debug ports let a debugger attach to
  the running kernel).
- Review weak CONFIG_* defaults in `config-include/kernel_config.h`:
  `MPTCP 1` (kernel_config.h:43) widens kernel network attack surface (Apple ships
  it optional) — default it to 0 until the P2 kernel needs it; `CONFIG_DTRACE 1`
  and `CONFIG_CPU_COUNTERS 1` are read-mostly but should be gated off in
  release kernels; keep `CONFIG_MACF 1` (required later for AMFI/sandbox).
- Wire trust-cache / code-signing validation before the first kext load:
  `mig/shim/TrustCache/API.h` is the clean-room stand-in; the boot kernel
  itself is unsigned static Mach-O (patch_macho drops LC_CODE_SIGNATURE,
  verify forbids it — fine for the static image), but no loaded extension
  should be trusted until the trust-cache check is real.
- Stack canaries: the kernel link (`link/link.sh`) passes no
  `-fstack-protector`; enable `-fstack-protector-strong` plus the xnu
  `__stack_chk_guard` setup (set the guard from RNG early in boot) for the P2
  kernel. Keep `-fno-stack-protector` in the freestanding loader stub.
- Keep sanitizers out of release: `config-include/san/` (kasan, kcov) shims
  exist for development kernels only; assert release configs never define the
  corresponding CONFIG_SAN options.

## P3

- Run `link/verify.py` as a CI gate on every linked kernel image, not just
  locally via link.sh.
- Pin toolchain versions (clang, byacc, flex, ld64.lld) with checksums in
  BLOCKERS.md; GPL policy is already clean (byacc/public-domain yaccpar, no
  bison, no darling code) — add a CI grep for GPL markers to keep it that way.
- Loader robustness: `stub.c` walks the module's load commands without bounds
  checks against `mod_end` (ncmds/cmdsize are attacker-controlled only if the
  boot image itself is untrusted, but validate anyway) and `verify.py` reports
  `__LINKEDIT` as `--x` (ld64.lld artifact; harmless for a static image, note
  only).
