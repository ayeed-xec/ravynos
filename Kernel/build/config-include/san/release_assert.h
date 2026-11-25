/*
 * Release-config gate (WP-A5, HARDENING.md P2): a release kernel build
 * force-includes this header first so a debug-only option cannot silently
 * come back on. Kept under san/ because the sanitizer options are the main
 * thing it pins.
 *
 * Usage (release build driver):
 *   clang ... -include san/release_assert.h ...
 * Development kernels must not use it: they legitimately enable these.
 */
#ifndef _RELEASE_CONFIG_ASSERT_H
#define _RELEASE_CONFIG_ASSERT_H

#ifdef MACH_KDP
#if MACH_KDP
#error "release kernel: MACH_KDP must be 0 (debug port would be reachable)"
#endif
#endif

#ifdef CONFIG_KDP_INTERACTIVE_DEBUGGING
#if CONFIG_KDP_INTERACTIVE_DEBUGGING
#error "release kernel: CONFIG_KDP_INTERACTIVE_DEBUGGING must be 0"
#endif
#endif

#ifdef MACH_LDEBUG
#if MACH_LDEBUG
#error "release kernel: MACH_LDEBUG must be 0"
#endif
#endif

#ifdef CONFIG_DTRACE
#if CONFIG_DTRACE
#error "release kernel: CONFIG_DTRACE must be 0 (instrumentation is dev-only)"
#endif
#endif

#ifdef CONFIG_CPU_COUNTERS
#if CONFIG_CPU_COUNTERS
#error "release kernel: CONFIG_CPU_COUNTERS must be 0 (dev-only counters)"
#endif
#endif

#ifdef MPTCP
#if MPTCP
#error "release kernel: MPTCP must be 0 (optional Apple feature, attack surface)"
#endif
#endif

#ifdef CONFIG_SAN
#error "release kernel: CONFIG_SAN must not be defined (KASAN/KCOV are dev-only)"
#endif

#endif /* _RELEASE_CONFIG_ASSERT_H */
