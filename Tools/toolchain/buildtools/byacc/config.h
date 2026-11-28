/* SPDX-License-Identifier: BSD-2-Clause */
/* Copyright (c) 2026 The ravynOS Authors */

/*
 * The config.h byacc's autoconf script would write for a POSIX host with
 * clang. byacc builds without configure or make (SYSTEM-DESIGN.md 2.2), so
 * the answers are fixed here; every host we build on (Linux, macOS) has these.
 */
#ifndef RAVYNOS_BYACC_CONFIG_H
#define RAVYNOS_BYACC_CONFIG_H

#define HAVE_FCNTL_H 1
#define HAVE_GETOPT 1
#define HAVE_GETOPT_HEADER 1
#define HAVE_INTTYPES_H 1
#define HAVE_MKSTEMP 1
#define HAVE_STDINT_H 1
#define HAVE_STDLIB_H 1
#define HAVE_STRING_H 1
#define HAVE_SYS_STAT_H 1
#define HAVE_SYS_TYPES_H 1
#define HAVE_UNISTD_H 1
#define HAVE_VSNPRINTF 1
#define MIXEDCASE_FILENAMES 1
#define STDC_HEADERS 1

#define GCC_NORETURN __attribute__((noreturn))
#define GCC_UNUSED __attribute__((unused))
#define GCC_PRINTF 1
#define GCC_PRINTFLIKE(fmt, var) __attribute__((format(printf, fmt, var)))

#endif
