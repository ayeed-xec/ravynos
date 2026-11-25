# WP-A2a kernel compile driver — syntax-check chain

`compile.sh` runs `clang -fsyntax-only` on kernel `.c` files with the
full include story. Verified clean (0 errors) on `osfmk/kern/startup.c`.

## The recipe

- `export-headers.sh` first (once): flattens 1020 headers from the
  component Makefiles into `export-hdrs/`.
- Include order matters, earliest first:
  1. `config-include/` (kernel_config.h force-included, `san/`
     forwarders, `kernel_base_types.h`)
  2. `xnu/bsd/machine` and `xnu/bsd/i386` (real arch headers)
  3. `mig/gen` (generated Mach headers)
  4. flat `export-hdrs/` (can hold arch-collided files, e.g. the ARM
     `types.h` — the real dirs must shadow it)
  5. the xnu tree dirs (osfmk, pexpert, iokit, libkern, bsd,
     EXTERNAL_HEADERS)
  6. `mig/shim/` LAST: it intentionally shadows a handful of headers
     (`i386/_types.h`, `machine/types.h`, `TrustCache/API.h`) for
     files that would otherwise not resolve, but must never win over
     the real tree for general headers.
- Defines: KERNEL family + `MACH_KERNEL_PRIVATE` +
  `BSD_KERNEL_PRIVATE` (kernel .c files need `thread_urgency_t` etc.
  from the private branches) + `CONFIG_KERNEL` + arch.
- `-include stddef.h`: xnu gets `size_t` from SDK clang modules
  (`USE_CLANG_TYPES`); without modules the kernel `string.h` uses
  `size_t` before any typedef is visible.
- `-target x86_64-apple-darwin` so `#include_next` and compiler
  builtins match Apple's layout; no `-nostdinc` (breaks `stdarg.h`).

## TrustCache/API.h

Not shipped in Apple's public xnu tarball. Clean-room stand-in at
`mig/shim/TrustCache/API.h` (opaque runtime types + the enums/
constants `bsd/sys/trust_caches.h` references). Extend it as more
files compile.

## Not yet passing

`bsd/kern/bsd_init.c` (20 errors) and `osfmk/kern/zalloc.c` (13):
next wave is `CONFIG_ZONE_MAP_MIN` (more generated config values),
`semaphore_consume_ref_t` (mig gen header vs real types), and missing
prototypes (`ledger_credit`, `__nosan_strlcpy`). See BLOCKERS.md.
