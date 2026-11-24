# SwiftUI feasibility for ravynOS

Research date: 2026-11-24. Scope: architecture and source review, not a successful native execution test. Research was carried out in a parallel session without access to the ravynOS clone and integrated here. Assume the project requires native application compatibility and permissive-only shipped code; confirm these policy boundaries before integration.

## Verdict

Recommend a clean-room implementation in narrowly tested increments as the distributable direction. Loading user-supplied Apple frameworks is potentially the shortest route to a fidelity experiment, but only after legal approval and substantial loader/runtime work. It is not a freely redistributable solution. HTML rendering is a source-porting alternative, not native binary compatibility. No option is demonstrated working on ravynOS by this audit.

## What a SwiftUI application depends on

Do not confuse source imports, SDK linker stubs, and an executable's actual load commands. A SwiftUI application ordinarily uses SwiftUI.framework and the Swift runtime, including `/usr/lib/swift/libswiftCore.dylib`. The inspected macOS 26.5 SDK mirror explicitly reexports SwiftUICore from SwiftUI; its libswiftCore stub supplies that install name. SwiftUICore is therefore part of the compatibility surface, even when an application only imports SwiftUI. [S1-S3]

AttributeGraph appears in SwiftUI exported symbol signatures and has a private-framework linker stub. This is evidence of a graph-related ABI surface, not proof that every application directly loads AttributeGraph. AppKit hosting is exposed through NSHostingView. QuartzCore/Core Animation belongs in the rendering dependency investigation, but the exact dependency edges and loading conditions were not established from a running application here. Foundation, CoreGraphics, observation, concurrency, text, events and accessibility also need inventory; implementing these five named libraries alone is not a completeness claim. [S1, S4-S5]

The SDK files cited below are third-party mirrors, not Apple compatibility guarantees or redistribution grants. Linker stubs describe exports/reexports, not the complete transitive dependency graph. Inspect a legally obtained target application's Mach-O load commands and the matching OS/framework build before fixing that graph. Swift's stable ABI does not supply SwiftUI's implementation or make an arbitrary non-Apple runtime compatible. [S6]

## Options and tradeoffs

1. **Clean-room ABI-compatible reimplementation.** Reimplement the exact symbols, calling conventions, Swift metadata/protocol witnesses, ownership, framework behavior and rendering services required by a pinned application set. Source compatibility alone is insufficient for existing binaries. Highest engineering cost, but the strongest direction for a distributable native system under the assumed policy. Independently written code and documented provenance are essential; clean-room process does not itself settle all legal questions. OpenSwiftUI illustrates source-compatible work, not proof of drop-in binary compatibility. [S7]
2. **Load user-supplied Apple binaries.** Reuse matching SwiftUI, SwiftUICore and private dependencies to reduce framework reimplementation for a restricted experiment. This still requires compatible Mach-O/dyld handling, Objective-C and Swift runtimes, system APIs and graphics services; it is not a framework-copy shortcut. "Fastest demo" is a hypothesis conditional on those prerequisites. Apple's macOS license restricts redistribution and use on non-Apple systems; Xcode/SDK terms impose further restrictions. User possession does not establish permission. Do not bundle, download or extract these binaries as part of this proposal. The user must decide whether to pursue this route after qualified legal review. [S8-S9]
3. **SwiftUI-to-HTML source port.** Recompile or adapt application source for a compatible web renderer such as Tokamak. It is not an automatic exporter for arbitrary compiled SwiftUI applications. Useful for a limited UI demonstration; native AppKit behavior, accessibility, text layout and API coverage require separate validation. It does not meet the native binary objective. [S10]

## Recommended acceptance gates

- Pin an application, architecture, deployment target and OS/framework build; record file hashes and acquisition authority.
- Establish the actual recursive load graph and missing symbols; distinguish hard, weak and runtime-loaded dependencies.
- Validate Objective-C metadata/ARC and Swift metadata, generics and concurrency before claiming framework compatibility. See OBJC-RUNTIME-AUDIT.md.
- Run a real window, text layout, state update, input, resize, accessibility and shutdown scenario; record failures as well as successes.
- Keep proprietary-binary experiments separate from distributable artifacts and require explicit permission before starting them.

Not verified: any native SwiftUI binary on ravynOS, the complete macOS 26.x dependency graph, or permission to run Apple's binaries there. No implementation or timing estimate is warranted from this research alone.

## Sources

[S1]: https://raw.githubusercontent.com/alexey-lysiuk/macos-sdk/896cd40df984b847d486723edce50e247385617e/MacOSX26.5.sdk/System/Library/Frameworks/SwiftUI.framework/Versions/A/SwiftUI.tbd
[S2]: https://raw.githubusercontent.com/alexey-lysiuk/macos-sdk/896cd40df984b847d486723edce50e247385617e/MacOSX26.5.sdk/System/Library/Frameworks/SwiftUICore.framework/Versions/A/SwiftUICore.tbd
[S3]: https://raw.githubusercontent.com/alexey-lysiuk/macos-sdk/896cd40df984b847d486723edce50e247385617e/MacOSX26.5.sdk/usr/lib/swift/libswiftCore.tbd
[S4]: https://raw.githubusercontent.com/alexey-lysiuk/macos-sdk/896cd40df984b847d486723edce50e247385617e/MacOSX26.5.sdk/System/Library/PrivateFrameworks/AttributeGraph.framework/Versions/A/AttributeGraph.tbd
[S5]: https://developer.apple.com/documentation/swiftui/nshostingview
[S6]: https://www.swift.org/blog/abi-stability-and-more/
[S7]: https://github.com/OpenSwiftUIProject/OpenSwiftUI/blob/7d0df61393f31dc8a4ffac9d635ce33af1b5e890/README.md
[S8]: https://www.apple.com/legal/sla/docs/macOSTahoe.pdf
[S9]: https://www.apple.com/legal/sla/docs/xcode.pdf
[S10]: https://github.com/TokamakUI/Tokamak/blob/e0d8e9db462938610337327ade58161aa3b698ac/README.md

All were read over HTTPS; Apple's license PDFs were text-extracted. No Apple binaries are included.
