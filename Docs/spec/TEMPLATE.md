# <Subsystem>: <topic>

One paragraph: what behavior this spec covers and which subsystem in
[../design/SYSTEM-DESIGN.md](../design/SYSTEM-DESIGN.md) implements it.

## Sources

Every source this spec relies on. Allowed: Apple's public documentation and headers,
Apple's open-source code, third-party applications' own binaries (for what they import),
and probe runs on Apple hardware. Not allowed: anything derived from Apple's compiled
code.

| Id | Source |
|---|---|
| O1 | probe `Tests/probes/<name>`, macOS <version>, <machine>, output `<file>` |
| D1 | <documentation URL> |

## Behavior

Each claim is one numbered statement with the source that established it.

1. `<API>` returns … when … (O1)
2. …

## State and formats

State machines, message formats, file formats, with field offsets and sizes where
relevant.

## Tests

The probes that become ravynOS tests, and what each asserts.

## Open points

Behavior not yet observed, and the probe that would settle it.
