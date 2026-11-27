# Objective-C runtime: license and ABI audit

> Superseded by [design/DECISIONS.md](design/DECISIONS.md) ADR-05: ravynOS uses Apple's objc4. Kept for reference.

Research date: 2026-11-24. Verdict: conditional acceptance of MIT-licensed libobjc2 source, subject to a permissive-only dependency/toolchain audit; no claim of macOS 26.x binary compatibility. The research session had no access to the ravynOS clone or its actual runtime build; integrated here.

## Correct upstream and license

The `llvm-swift/libobjc2` GitHub repository returned HTTP 404. This does not establish its historical ownership or license. This audit instead pins GNUstep libobjc2 at `762f9d7ed2a518cbd8ab13c3ace3176de2db08f0`. Its COPYING grants the MIT permission to use, modify and distribute, with copyright/permission notice retention and warranty disclaimer. [R1]

The README says early work included GCC runtime code and therefore was GPL, then states: "Since then, all of the GCC code has been removed, leaving the remaining files all MIT licensed, and allowing the entire work to be MIT licensed." It also says the libcxxrt exception header was permitted under MIT. The asserted current "BSD with optional GPLv3 backends" description is not supported by these sources. Historical GPL ancestry is not evidence that today's optional modes reintroduce GPL code. [R2]

Project policy (user decision, 2026-11-24): no GPL or GPL-derived components in shipped artifacts. If the policy also prohibits GPL tools used only during compilation, apply that broader rule to the build host separately. This is an engineering license review, not legal advice or an audit of every downstream package.

## Build-mode verdict

The inspected CMake configuration has OLDABI_COMPAT, LEGACY_COMPAT, TYPE_DEPENDENT_DISPATCH, ENABLE_OBJCXX, EMBEDDED_BLOCKS_RUNTIME, shared/static and other options. No inspected option is documented as selecting a GPL backend. Enabling compatibility with the GCC *ABI* does not mean linking GCC's *runtime*. [R2-R3]

- **Candidate baseline:** modern GNUstep ABI; OLDABI_COMPAT=OFF, LEGACY_COMPAT=OFF, ENABLE_OBJCXX=ON, EMBEDDED_BLOCKS_RUNTIME=ON; Clang plus a separately reviewed permissive C++ library, ABI library and unwinder. Keeping Objective-C++ enabled avoids silently dropping that capability. These are proposed settings, not a verified ravynOS build command.
- **Shared versus static:** neither choice changes libobjc2's MIT license. Audit all dynamic dependencies and all objects incorporated into static outputs; static linking is not a license exemption.
- **Legacy/typed dispatch:** no identified GPL license switch; enable only when needed for the target code. Disabling OLDABI_COMPAT removes older ABI support, not copyleft obligations. Typed-selector behavior also needs semantic compatibility tests.
- **Dependency gate:** CMake finds tsl-robin-map or fetches v1.4.0, whose LICENSE is MIT. Record the actually resolved version, including preinstalled packages. Inspect generated link lines and final dependencies for libstdc++, libgcc, external Blocks runtimes and all other components. Under the project's permissive-only policy, a GPL library with a runtime exception still requires explicit policy approval rather than being silently accepted. Do not assume choosing Clang selects a permissive standard library/unwinder. [R3-R4]
- **STRICT_APPLE_COMPATIBILITY:** CMake describes this as always defining BOOL as signed char. It is not a switch for the entire Apple ABI and should not be enabled as a substitute for per-architecture verification. [R3]

Conditional source approval is defensible; unconditional approval of an uninspected binary or every build configuration is not. Preserve MIT notices and produce a dependency/license inventory before distribution.

## ABI gap against macOS 26.x-era frameworks

The README promises GCC, GNUstep 1.9 and GNUstep 2.0 ABIs, not Apple's current Mach-O metadata ABI. The availability of similarly named APIs does not prove binary interchangeability. Apple's inspected objc4 source is a comparison reference, not proof that its commit exactly matches a particular macOS 26.x release. [R2, R5]

| Area | Evidence and remaining requirement |
| --- | --- |
| ARC | GNUstep documents ARC APIs. Apple framework acceptance additionally needs correct retain/release, autorelease return-value handshakes, weak zeroing, Blocks lifetime and Swift/Objective-C ownership interoperation. API presence is not a passed test. |
| Non-fragile ABI | GNUstep explicitly supports its own non-fragile ABIs. Apple class_ro/class_rw metadata, ivar offsets, image registration, categories and protocols require compatible decoding/registration; do not pass GNUstep structures directly to Apple binaries. |
| Modern objc_msgSend | libobjc2 has architecture-specific messenger assembly. Validate target calling conventions, nil/forwarding/super sends, architecture-applicable return conventions, selector representation and dispatch/cache expectations. Matching an exported name is insufficient. [R6] |
| Relative method lists | Apple objc4 documents small method lists and relative entries, including shared-cache-specific selector treatment. GNUstep's inspected method structure uses ordinary implementation, selector and type fields. An Apple-metadata adapter or equivalent support must be demonstrated; this comparison is not an exhaustive proof that every possible adapter is absent. [R5, R7] |
| Modern loader/runtime integration | Inventory chained fixups, shared-cache assumptions, tagged pointers, class initialization, exceptions and, for relevant arm64e images, pointer authentication. Exact needs depend on the selected binaries and architecture. |

## Acceptance evidence still required

1. Pin the ravynOS runtime revision, build flags, dependency revisions and target macOS images/architecture.
2. Inventory undefined symbols and metadata formats from real target binaries; test only relevant calling conventions rather than assuming x86_64 and arm64e are interchangeable.
3. Run image registration, inheritance/ivar-offset, category/protocol, relative-list, message-send/forwarding, ARC/weak/autorelease and exception/Blocks probes.
4. Run the actual target framework's initialization and a representative UI flow, checking ownership failures and shutdown as well as launch.

None of these native compatibility tests ran in the research session. Recommendation: accept the pinned MIT source for further evaluation, reject a blanket macOS 26.x compatibility claim, and make permissive dependency review plus application-level execution the release gates.

## Sources

[R1]: https://github.com/gnustep/libobjc2/blob/762f9d7ed2a518cbd8ab13c3ace3176de2db08f0/COPYING
[R2]: https://github.com/gnustep/libobjc2/blob/762f9d7ed2a518cbd8ab13c3ace3176de2db08f0/README.md
[R3]: https://github.com/gnustep/libobjc2/blob/762f9d7ed2a518cbd8ab13c3ace3176de2db08f0/CMakeLists.txt
[R4]: https://github.com/Tessil/robin-map/blob/v1.4.0/LICENSE
[R5]: https://github.com/apple-oss-distributions/objc4/blob/fb265098298302243cd7eeaa1f63f0ba7786dd9a/runtime/objc-runtime-new.h
[R6]: https://github.com/gnustep/libobjc2/blob/762f9d7ed2a518cbd8ab13c3ace3176de2db08f0/objc_msgSend.x86-64.S
[R7]: https://github.com/gnustep/libobjc2/blob/762f9d7ed2a518cbd8ab13c3ace3176de2db08f0/method.h

Links identify the reviewed revisions; this document contains no copied runtime implementation.
