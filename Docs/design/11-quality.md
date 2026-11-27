# 11 — Quality: the bar, the gates, the review

> Background document. [SYSTEM-DESIGN.md](SYSTEM-DESIGN.md) is the design of record; where this document disagrees with it, SYSTEM-DESIGN.md is right. Status lives only in [STATUS.md](STATUS.md).

This document defines what "done" means for contributed code: the style bar
each language is held to, the anti-bloat rules a reviewer checks line by line,
the test and sanitizer requirements, the performance budgets the build gates
on, license hygiene, and the documentation every component owes. Read it
before your first pull request; [05-build-system.md](05-build-system.md)
describes the machinery that enforces the mechanical parts, and this document
supplies the content those gates check against.

## The bar

The working standard is simple to state and demanding to meet: code should
look like it belongs next to Apple's own sources. Most of what we build is
either Apple's code, code that interfaces with it, or a clean-room
implementation of a subsystem whose public surface Apple defined. In all
three cases the reader is a developer who knows what Apple's code looks like
— and a file that fights that expectation costs review time on every change,
forever.

This is not nostalgia for Apple's house style. It follows from the vendoring
policy in [03-kernel.md](03-kernel.md): our tree mixes vendored drops with
our patches and our own components, and the only way a reader can tell
provenance at a glance is if each part looks native to its origin. A patch
against XNU written in a foreign style is a diff that reviewers must translate
before they can judge it.

### Formatting

Formatting is mechanical and therefore not negotiable in review. Our own new
code — the RVOS overlay, `rv`, kite, the window server, tools — follows the one
project `.clang-format` at the tree root (LLVM-based, 4-space indent, 100
columns, DESIGN.md §14.1) and `rustfmt` for Rust; the format check
(`Tools/ci/check-format.sh`) fails CI on a diff. Vendored trees are excluded
from the gate and keep their upstream style: a component built from Apple's
CoreFoundation sources formats like CoreFoundation, and a patch against XNU
formats like the file it touches. A vendored component that needs a
formatter config of its own carries one in its directory, which then
overrides the root. Either way, reviewers never discuss whitespace.

For files that are neither clang-formatted (assembly, mig `.defs`, property
lists), the component's README states the convention in one line.

### Naming

Name things the way the subsystem around you names them, not the way a
general style guide says to. Concretely:

- **Kernel and IOKit work** follows IOKit convention: `IO` prefixes for
  families, CamelCase classes inheriting the surrounding machinery, C entry
  points with the subsystem's own prefix. An IOKit driver whose classes lack
  the prefixes every neighboring driver uses is rejected on sight, even
  though a "modern C++ naming" guide would approve of it.
- **CoreFoundation and Foundation work** follows CoreFoundation convention:
  `CF`/`NS` typedef naming, the `Create`/`Retain`/`Release` memory-management
  vocabulary, `k`-prefixed constants. Inventing a house style that fights the
  code around it is a defect, not a preference.
- **Our own from-scratch components** (the harness, kite, the window
  server's internals) pick one convention per component, write it in the
  component README, and keep it. These components are free to look modern;
  they are not free to look like three authors three ways.

The rule of thumb: the subsystem's native convention wins over any document
in this directory. Where a subsystem has no native convention, the component
README is the convention.

### Header-first development

Public headers are reviewed before the implementation that backs them. A
pull request that lands an implementation with a private header gets its
interface review after the fact, which is the expensive order: by then the
callers exist and the interface has users to defend it. The cheap order is a
PR (or a first commit on a branch) that contains the public header, its
documentation comments, and nothing else; the second PR contains the
implementation.

This matters more here than in an ordinary project because our public
headers are compatibility surface. A header in the sysroot is a promise to
macOS-binary compatibility and to every internal caller; walking it back is
an ABI event, not a refactor. Reviewing the header first is how that promise
gets scrutinized before it is made.

### Headers do not leak internals

A public header exposes what a caller needs and nothing about how it is
implemented. Review-checkable forms of the rule:

- No private structures, lock orderings, or allocation strategies in an
  installed header. If a caller needs to know them, the interface is wrong.
- No `#include` of a vendored internal header in an installed header;
  forward-declare or move the detail behind an opaque pointer.
- Installation prefixes (`_private`, `Internal/`) are enforced by the
  harness: a target cannot include another target's private headers, the
  same layer rule that [02-architecture.md](02-architecture.md) applies to
  directories, applied to files.

## Anti-bloat: rules a reviewer checks

These are stated as review-checkable items on purpose. A principle that
cannot be cited in a review comment does not restrain anyone; each item
below can be quoted, with a line number, in a "please change this" comment.

1. **No stubs.** The no-stubs doctrine in [01-principles.md]
   (01-principles.md) makes a build state, never a fake return value. A
   function in an `experimental` component may return
   `ENOSYS`-equivalent and say so through the build state; a function in a
   `working` component that quietly returns `0` for the unimplemented half
   is the one failure mode this project cannot tolerate, because a system
   that lies to its developers cannot be debugged. A reviewer failing a PR
   for a stub is citing 01, not being difficult.
2. **No speculative abstraction.** No plugin factories, no configuration
   surfaces, no "future" parameters without a second user in the tree today.
   The test a reviewer applies: name the second user. If the answer is
   "someone will want this," delete it. This is not an argument against
   abstraction; it is an argument against paying for abstraction before it
   exists.
3. **Functions do one thing.** A function with a boolean parameter that
   selects between two behaviors is two functions. A reviewer should be able
   to describe a function's contract in one sentence without the word "and"
   doing heavy lifting.
4. **No translation layers by copy-paste.** A file that exists to convert
   one struct to a near-identical struct, field by field, is a design smell:
   either the two types should be one, or the boundary between them is real
   and deserves a conversion written once, with a test, at the boundary.
   Copy-pasted field-by-field translation rots silently whenever either side
   gains a field.
5. **Dead code is deleted in the same PR that orphans it.** A change that
   leaves a function, a code path or a build target unreachable "in case we
   need it" is an incomplete change. Version control remembers; the tree
   should not.
6. **Comments explain why.** A comment that restates the code is noise; a
   comment that explains a constraint, a tradeoff, or a reference is load-
   bearing. A stale comment — one the diff makes false — is a review
   failure of the same severity as a stale test.
7. **No defensive boilerplate.** Validate the invariant once at the boundary
   and assert it thereafter. Checking every pointer every layer down is not
   safety; it is a tax paid by every reader, and it hides which checks are
   load-bearing. Where the invariant genuinely is "the caller must not pass
   NULL," assert it in debug builds and document it once.

None of these rules override the vendored tree. Apple's sources carry their
own dead code and their own comment style; per [03-kernel.md](03-kernel.md)
we do not edit vendored files at all, let alone tidy them.

## Tests

The test requirements scale with how wrong a silent failure can go.

**Unit tests for anything that parses or formats.** A component that reads a
format (APFS structures, Mach-O, plists, the boot log, a wire protocol) or
writes one ships unit tests over its parser and formatter in the same PR as
the code. These run host-side under `rv test <component>`
([05-build-system.md](05-build-system.md)) on every build, and they run on
the corpus, below. The reasoning: format code fails quietly — a wrong field
offset reads garbage, not an error — and it fails at the worst possible
place, in front of a disk image or a binary that no longer parses.

**Kernel tests run in QEMU in CI.** Kernel-side changes run under QEMU on
every commit, not on demand. The pattern [04-boot.md](04-boot.md) uses for
the loader — a test kernel whose only job is to check its preconditions,
print a structured record to serial, and stop — generalizes: kernel tests
assert on the serial log, so a test needs no display, no network and no
interactive debugger. Our build fleet has no GPU and no target hardware, so
QEMU is where kernel tests live; the gap between QEMU and hardware is real
(05 says so) and is covered by contributors with machines, not pretended
away.

**Conformance corpora where one exists.** Two are load-bearing:

- **APFS volumes made by Apple's tools.** [06-filesystem.md]
  (06-filesystem.md) defines the corpus: named volume states produced by
  Apple's own tools on a macOS host, each with the assertions our
  implementation must satisfy. Our APFS code is judged against volumes the
  reference implementation made, not against volumes we made.
- **The stock-binary compatibility corpora.** [09-compat.md](09-compat.md)
  owns the Linux and FreeBSD binaries the ELF loader and ABI environments
  must handle; [13-toolchain-sdk.md](13-toolchain-sdk.md) §2.4 owns the
  macOS-built fixture set dyld and the frameworks must run. A change to dyld, the ELF loader, or the
  Mach-O image loader runs the corpus; "it compiles" is not evidence about
  binary compatibility.

A new subsystem that has a corpus available builds one early. The corpus is
test infrastructure and is version-pinned like any other vendor input.

**Failing CI trumps everything.** A red main branch outranks every other
task, including feature work, including the person who broke it moving on to
something else. The rule has teeth because of the next section: the gates in
CI are the only place our budgets, sanitizer runs and license audit execute,
so a red branch left red makes every one of them invisible. Reverting the
offending change is always an acceptable fix; debugging it on a branch is a
privilege, not a right.

## Performance budgets as CI gates

[01-principles.md](01-principles.md) states the philosophy — performance is
a budget, not an aspiration — and [05-build-system.md](05-build-system.md)
reserves the mechanism: the harness measures what it builds, compares
against the budget table, and fails the build with old and new numbers in
the error. This document owns the table.

Two kinds of number live here, and they must not be confused. DESIGN.md §5.1
sets **targets** — P1 to P10, the numbers the project commits to reaching,
such as a Mach IPC round trip of at most 2 µs. A **gate** is different: it
fails the build when a change regresses more than 5 % against the baseline
recorded at the wave where the budget arms. A target with no measurement
yet is a commitment; a gate needs a measurement, so its baseline is recorded
when the subsystem lands and the gate arms at that moment. Neither is
invented by reviewers.

The table, with wave numbers per [12-roadmap.md](12-roadmap.md):

| Budget | Target (DESIGN.md §5.1) | Method | Gate arms |
|---|---|---|---|
| P2 Boot to shell, QEMU | ≤ 10 s, trend only | serial-log timestamps, same VM configuration every run | W1 |
| P9 Full kernel build, 8 cores | ≤ 5 min | harness timing report | W1 |
| P10 Per-file compile p95 | ≤ 1 s | harness per-file timings | W1 |
| P3 Null syscall | ≤ 300 ns | `Tests/perf/syscall_null`, p50/p99 | W2 (relative in QEMU) |
| P4 Mach IPC round trip | ≤ 2 µs | fixed ping-pong between two processes through a bootstrap lookup | W2 (relative in QEMU) |
| P5 Context switch | ≤ 1 µs | `Tests/perf/ctx_switch` | W2 (relative in QEMU) |
| P6 Input to photon | ≤ 16.6 ms at 60 Hz | instrumented window server checkpoints (07-graphics.md) | W4 |
| P7 Frame pacing | < 0.5 % dropped over 60 s | scripted composition load | W4 |
| P1 Power to first app frame, bare metal | ≤ 3 s | bare-metal NVMe rig | W4 |
| P8 Cold launch, small app | ≤ 500 ms | process start to first frame | W6 |
| Kernel image and section sizes | none yet | per-component section-size report | W1, baseline only |
| Bootloader footprint | none yet | kite binary size at link time | Boot track's first exit, baseline only |

QEMU's TCG distorts absolute latency, so P3–P5 gate as ratios against the QEMU
baseline and are confirmed in absolute terms on bare metal at W4.

Three properties of a good budget follow from the table and are worth
stating because they discipline the entries we add later:

- The method names the machine. "Fast" is not a budget; "median of N runs on
  the reference configuration" is. QEMU numbers and bare-metal numbers are
  different budgets and are labeled as such, because a QEMU regression that
  reflects the host's load average is a different beast from one that
  reflects the code.
- The gate arms with the subsystem, not after. A budget bolted on at the
  end of a subsystem's life measures the accidents that already happened.
- A budget regression is a build failure with numbers in the message, not a
  dashboard. If the gate cannot explain the regression in its error output,
  the gate is not done.

## Memory and concurrency safety

We take from the toolchain what the toolchain gives, and we spend review
attention where the toolchain cannot see.

**Sanitizers run in CI.** ASan and UBSan builds run for userland components
on the build host, per component, with failures failing the build like any
other test. Sanitizer coverage is tracked per component: a component that
has never run under sanitizers is a component whose tests have not run
under sanitizers, which is a different statement, and the manifest records
which it is. The kernel does not get ASan the way userland does; XNU's own
debug facilities plus QEMU-run tests carry that side, and we do not pretend
the coverage is equivalent.

**Fuzzing for parsers.** Every parser from the unit-test section gets a
fuzzer (libFuzzer against the host-side build), and the fuzzers run
continuously, not per-commit. Corpus entries that find bugs join the
regression set. Format code is where hostile input lives — a filesystem
mounts media anyone can write, a loader runs binaries anyone can hand it —
so format code is where fuzzing pays.

**The review checklist.** A reviewer ticks these on any PR touching memory
or concurrency:

1. Every allocation has one obvious owner, and the free path is in the same
   function or documented in the header.
2. Buffer sizes come from the same source as the buffer, not from a
   constant that happens to match today.
3. User-controlled lengths are checked against the destination before the
   copy, not inferred from the copy's return value.
4. Every lock has a stated scope in the component README's concurrency
   notes; a function that acquires two locks names the order.
5. A code path that sleeps holds no spinlock (kernel) or no non-reentrant
   lock (userland).
6. Callbacks are documented as synchronous or asynchronous at the
   interface, and a caller that frees memory a callback might touch can
   point to the line that prevents it.
7. Every `dispatch_async` / deferred-work path can answer "what if the
   object is released first."
8. Integer arithmetic on sizes uses the component's overflow-checked
   helpers, not bare `a * b`.
9. Data shared with an interrupt context (kernel) or a signal context
   (userland) is written with the primitives designed for it.
10. A failure path releases everything the success path acquired — checked
   by reading the error branches, not by trusting the happy path's shape.
11. TOCTOU: anything validated before a syscall or IPC round-trip is
   re-examined after it.
12. New global state has a comment naming the thread (or queue) that owns
   it.
13. Timeouts exist on every wait that can hang; a hang in a daemon is a
   restart, a hang in the kernel is a panic, and neither should be a
   surprise the first time.
14. The change runs under sanitizers in CI (or the manifest says why it
   cannot yet).

Fourteen items is a long list, and the honest note is that experienced
reviewers run it in seconds because most of it is pattern recognition. The
list exists for the reviewer seeing the codebase for the first time, which
is the reviewer we should optimize for, because that is who every new
contributor is on their first PR.

## License hygiene as a mechanical gate

The policy in [01-principles.md](01-principles.md) — no GPL, LGPL or AGPL anywhere in the tree or
linked into the build; APSL, BSD, MIT, Apache-2.0, Zlib, ISC and public
domain acceptable — is enforced as a build gate, not a convention.
[05-build-system.md](05-build-system.md) runs the audit as a build step:

- Every file is fingerprinted against a license database; an unknown or
  missing license fails the build.
- Vendored files keep their upstream headers verbatim; a stripped header
  fails the audit.
- The allowlist is explicit per directory, so an import into the wrong
  directory fails even when the license itself is acceptable.

**Host tools are gated too.** A license audit reads files; the programs that run to produce
them are checked by `Tools/ci/check-host-tools.sh`, which fails when any script under
`Boot/`, `Tools/`, `Tests/` or `.github/`, or the harness source, invokes a GPL program: `gcc`,
`bison`, GNU `patch`, GNU `m4`, `make`, `objcopy`, `nm`, `readelf`, `ar`, `ld`. What
replaces them is ours or LLVM's: `rvpatch` applies the patch series, byacc builds the
parser, the lexer ships pregenerated, clang compiles, `ld.lld` links, `llvm-ar` archives.
QEMU is the one GPL program left in the loop, and it lives only in the test harness.

Reviewers do not adjudicate licenses in PRs; the gate does it faster and
more uniformly. What reviewers do check is provenance: where the code came
from, and whether the commit message says so.

**Clean-room evidence for RE-derived work.** For the subsystems we fill by
studying Apple's binaries — the window server protocol, Metal, APFS — the
license gate is necessary but not sufficient; what protects the project is
evidence that the implementation is ours. The rule:

- Each RE-derived subsystem has a **specification document** in its
  directory, written from observed behavior (public documentation, sample
  code, behavioral study of shipped binaries), describing the contract the implementation
  satisfies. [06-filesystem.md](06-filesystem.md) shows the pattern with its
  reference-volume corpus; [07-graphics.md](07-graphics.md) applies it to
  the window server's protocol.
- Code in that subsystem is **reviewed against the spec, not against the
  binary**. A reviewer who wants to check a behavior checks the spec's
  statement of it; if the spec is silent, the reviewer asks for a spec
  change first, not a fresh look at the binary at review time.
- Nobody who has read a GPL-licensed implementation of the same format
  touches our implementation of it — the two-party discipline
  [06-filesystem.md](06-filesystem.md) states for APFS, applied everywhere.
- Nothing derived from Apple's compiled code enters the tree. Study notes
  describe behavior; they do not contain transcription.

The evidence chain is what lets the project answer an accusation with
artifacts instead of assurances. It costs one document per subsystem; the
alternative risks the tree.

## Documentation per component

Every component directory carries a README that answers three questions,
in this order, in the first three paragraphs:

1. **What is this?** One paragraph a newcomer can read before anything
   else, naming the layer ([02-architecture.md](02-architecture.md)) it
   occupies and the document that owns its design.
2. **What does it talk to?** The component's dependencies and peers: which
   processes it messages, which sysroots it builds into, which headers it
   exports. The manifest is the machine-checkable version; the README is
   the human version and they must agree.
3. **What does it not do yet?** The honest list. A component in an
   `experimental` build state says what is missing; a component missing a
   feature it will eventually need says that too. This section is the
   no-stubs doctrine's documentation side: the build state declares what
   works, and the README declares what does not.

Beyond the README, documentation requirements are modest by design:
interface headers carry their contracts (the header-first section above),
and a subsystem with a non-obvious format or protocol carries its spec
document. We do not require per-function docs, design documents that
duplicate this directory, or wikis. The test a component's documentation
must pass: a contributor can build it, test it, and know its limits without
asking anyone.

## Open questions

- Sanitizer coverage for the kernel is much weaker than for userland. Do we
  build a kernel sanitizer configuration XNU can build with, or accept
  the gap and lean on QEMU tests? [03-kernel.md](03-kernel.md) owns the
  build side; the quality side needs an owner too.
- The budget table's "reference hardware" is undefined until the project
  buys or designates machines. QEMU budgets are armed from day one, but
  bare-metal numbers need a named machine, and the table should say which.
- The review checklist is written for code; documentation-only and
  manifest-only changes need a shorter version, and nobody has written it.
- Who maintains the license fingerprint database, and what is the appeals
  path when a vendored file's unusual formatting false-positives? 05 marks
  this unwritten; the audit gate makes it urgent rather than theoretical.
- Fuzzing runs "continuously," which implies infrastructure beyond per-commit
  CI (a target, a corpus store, a triage owner). The budget for that
  infrastructure is not yet allocated.
- Both stock-binary corpora have owners but no contents yet. Until the first
  pinned entries land, the gates that reference them cannot arm.
- The build states this document uses (`working`, `experimental`) are now
  specified in 05-build-system.md's manifest schema but not yet parsed by
  `rv`; until they are, the no-stubs states are enforced by review only.
