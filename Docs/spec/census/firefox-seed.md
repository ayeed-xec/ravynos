# Census seed: Firefox

The libraries and API that stock macOS Firefox (version 158 era) calls, as exercised by
PureDarwin's Firefox test programs (`tests/cg-ff`, `tests/appkit-ff`,
`tests/frameworks-ff`, `tests/ffprobe` at commit `0c1990c8`). It seeds the ABI census
(SYSTEM-DESIGN section 5.4) until `Tools/abi-census` runs over a Firefox build itself;
the census output replaces this list.

## Libraries loaded

AppKit, ApplicationServices, AudioToolbox, AudioUnit, AVFoundation, Accessibility,
AddressBook, AuthenticationServices, Carbon, CoreAudio, CoreFoundation, CoreGraphics,
CoreLocation, CoreMedia, CoreMIDI, CoreServices, CoreText, CoreVideo, Foundation, GSS,
ImageIO, IOBluetooth, IOKit, IOSurface, LocalAuthentication, MediaPlayer, OpenGL,
QuartzCore, ScreenCaptureKit, Security, ServiceManagement, SystemConfiguration,
UserNotifications, UniformTypeIdentifiers, VideoToolbox, Vision,
`/usr/lib/libcups.2.dylib`, `/usr/lib/libbsm.0.dylib`, libsandbox.

## API by framework

- **CoreGraphics:** bitmap contexts (`CGBitmapContextCreate`, `…WithData`, `GetData`,
  `GetBytesPerRow`, `GetHeight`); `CGContextSetBaseCTM`; transparency layers;
  save/restore state; fills; subpixel font positioning; PDF contexts with URLs and
  destinations; `CGPDFDocument`; `CGFont` tables, variations, glyph advances and boxes;
  `CGColor` and `CGColorSpace` (sRGB, ICC data); data providers and consumers; rect
  dictionaries; `CGWindowListCopyWindowInfo`, `CGWindowListCreateImage`,
  `CGWindowLevelForKey`; `CGMainDisplayID`, `CGDisplayScreenSize`, `CGDisplayIsBuiltin`,
  `CGDisplayCreateImage`, `CGDisplayStreamCreate`;
  `CGEventSourceSecondsSinceLastEventType`; SPI `_CGSDefaultConnection`,
  `CGSSetDenyWindowServerConnections`, `CGSShutdownServerConnections`.
- **Carbon / HIToolbox:** `TISCopyCurrentKeyboardLayoutInputSource`,
  `TISCopyCurrentKeyboardInputSource`, `TISCopyCurrentASCIICapable*`,
  `TISCreateInputSourceList`, `TISGetInputSourceProperty`, `TISSelectInputSource`,
  `TISCopyInputMethodKeyboardLayoutOverride`, `UCKeyTranslate`, `LMGetKbdType`,
  `KBGetLayoutType`, `GetCurrentEventKeyModifiers`, `GetThemeMetric`,
  `TSM*DocumentProperty`, `Enable/DisableSecureEventInput`, `RegisterEventHotKey`.
- **HIServices:** `GetCurrentProcess`, `GetProcessInformation`, `TransformProcessType`,
  `SetFrontProcess`, `PasteboardCreate`, `PasteboardCopyPasteLocation`, PrintCore
  `PMSession*` and `PMGetAdjustedPaperRect`, `UAZoomEnabled`, `AXTextMarker*`.
- **CoreServices:** `FSFindFolder`, `FSPathMakeRef`, `FSRefMakePath`, `FSCompareFSRefs`,
  `LSCopyDefaultApplicationURLForURL`, `LSCopyDefaultHandlerForURLScheme`,
  `LSSetDefaultHandlerForURLScheme`, `LSSharedFileList*` (recent documents),
  `MDItemCreate`, `MDItemCopyAttribute` (`kMDItemWhereFroms`), `UTTypeConformsTo`,
  `UTTypeCreatePreferredIdentifierForTag`, `UTTypeCopyPreferredTagWithClass`,
  `UTTypeIsDynamic`, `CFPreferencesCopyAppValue`, `CFPreferencesAppSynchronize`.
- **AppKit:** `NSApplication` (activation policy, `nextEventMatchingMask:`,
  `postEvent:`, `sendEvent:`), `NSWindow`, `NSView`, `NSEvent` factories, drag and drop
  (`beginDraggingSessionWithItems:`, `NSDraggingItem`, `registerForDraggedTypes:`),
  `NSPasteboard`, `NSCursor`, `NSImage imageWithSize:flipped:drawingHandler:`,
  `NSSavePanel`, `NSRunningApplication`, `NSWorkspace`, `NSScroller` styles,
  `NSColor colorUsingColorSpaceName:`, `NSMenu`; private `NSThemeFrame` title-bar
  methods (`_titlebarHeight`, `_closeButtonOrigin`).
- **Foundation:** constant string binding, `NSEqualRects`, `NSIsEmptyRect`,
  `NSPointInRect`, `NSProcessInfo thermalState`, `NSProgress`, `NSURLSession`,
  `NSMutableURLRequest`, `NSURLQueryItem`, `NSAppleEventDescriptor`,
  `NSXPCConnection`, `NSXPCInterface`, `NSUserActivity`, `NSUserNotificationAction`,
  `NSLayoutConstraint`, `NSSocketPortNameServer`, toll-free bridging of `NSDate`,
  `NSURL`, `NSSet`.
- **CoreAudio / AudioToolbox:** `AudioObjectGetPropertyData(Size)` (default output
  device, sample rate, stream configuration, name), `AudioComponent*`,
  `AudioUnitSetProperty`, `AudioUnitInitialize`, `AudioOutputUnitStart/Stop`,
  `AudioConverter*`, `AudioGetCurrentHostTime`, `AudioConvertHostTimeToNanos`.
- **CoreMedia / CoreVideo / VideoToolbox:** `CMBlockBuffer*`, `CMSampleBuffer*`,
  `CMVideoFormatDescription*`, `CMTimebase*`, `CMClockGetHostTimeClock`;
  `CVPixelBuffer*` (including IOSurface-backed), `CVBufferSetAttachment`,
  `CVDisplayLink*`; `VTDecompressionSessionCreate`, `VTCompressionSessionCreate`,
  `VTIsHardwareDecodeSupported`.
- **IOKit:** `IOPMAssertionCreateWithName`, `IOPMAssertionRelease`,
  `IOPSNotificationCreateRunLoopSource`, `IODeregisterForSystemPower`.
- **Security:** `SecCodeCopyGuestWithAttributes`, `SecCodeCheckValidity`,
  `SecStaticCode*`, `SecRequirementCreateWithString`, `SecCertificateCopyPublicKey`,
  `SecTrustSettingsCopyTrustSettings`.
- **Other frameworks:** AuthenticationServices web-browser passkey and single-sign-on
  providers; ScreenCaptureKit content picker and stream frame info; `SMAppService`,
  `SMJobBless`; Accessibility custom content; `LAContext`; `CLLocationManager`; Vision
  text recognition; `AVCaptureDevice`, `AVSpeechSynthesizer`, `AVURLAsset`,
  `AVAssetReader`, `AVSampleBufferDisplayLayer`; `MPNowPlayingInfoCenter`,
  `MPRemoteCommandCenter`; `IOBluetoothDevice`; CoreMIDI client, port and send;
  `cupsGetDests`; `audit_token_to_pid`.

## Runtime behavior Firefox depends on

- Native layers: `CALayer`s with IOSurface contents under the `NSView`'s layer
  (Firefox's NativeLayerCA), composited by the window server (S22, S24).
- `CVDisplayLink` callbacks paced by the display.
- `sandbox_init_with_parameters` succeeding for content processes (S12).
- Networking through Firefox's own NSS, so `NSURLSession` network support is not on
  Firefox's path.
