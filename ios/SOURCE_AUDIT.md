# iOS source audit

This is a source-level inventory, not a successful build report. The repository has not yet been compiled for iOS.

## Findings from the current repository

- `Minecraft.World/Minecraft.World.vcxproj` lists 715 C++ translation units and 846 headers.
- Its project configurations target ARM64EC, Durango, ORBIS, PS3, PSVita, Win32, x64, and Xbox 360. There is no iOS configuration.
- The project contains console-specific save implementations such as `ConsoleSaveFileInputStream.cpp`, `ConsoleSaveFileOutputStream.cpp`, and other `ConsoleSaveFile*.cpp` files. These must not be added blindly to an iOS target.
- Platform macros in the project include `_XBOX`, `__PS3__`, `__PSVITA__`, `WIN32`, and `_DURANGO`. Source selection and platform abstractions must be reviewed before compiling on Apple platforms.
- Both `Minecraft.Client/stdafx.h` and `Minecraft.World/stdafx.h` conditionally include platform SDKs such as `windows.h`, `d3d11.h`, DirectX headers, and Sony console SDK headers. The iOS target must define a deliberate Apple platform path and must not accidentally fall through into console-only assumptions.
- `Minecraft.Client/Windows64/Windows64_App.cpp` contains a desktop/console application subclass and a test-world startup path named `TemporaryCreateGameStart()`. It depends on `CMinecraftApp`, profile/storage managers, a fake local player join, and a `C4JThread` network-game thread. It is a useful reference for startup behavior, but is not an iOS entry point that can simply be compiled as-is.
- `Minecraft.Client/Input.cpp` implements gameplay input updates using `Minecraft`, `LocalPlayer`, game mode, and world state. It may be reusable behind a new touch-input adapter, but its platform-facing callers and event source still need tracing.
- `Minecraft.Client/glWrapper.cpp` forwards legacy-style graphics calls to `RenderManager`; it is a compatibility wrapper, not a standalone renderer. Reusing it alone will not provide Metal rendering.
- `Minecraft.Client/Minecraft.Client.vcxproj` currently has an empty file body in the GitHub repository response. Its client build configuration cannot be used as a reliable source list until that is checked and repaired.
- No `.xcodeproj` or `.xcworkspace` has been added yet. Therefore, an IPA build cannot succeed today; the workflow reports this explicitly instead of packaging a fake app.

## Porting implications

1. Do not try to compile all 715 world translation units as one first step. Trace the app entry point and dependency chain from the desktop startup path into the game loop, world creation, and renderer.
2. Separate portable game/world logic from filesystem, threading, audio, input, and console-save code.
3. Do not assume the Windows renderer can be reused on iOS. A real iOS rendering backend and a shared boundary from game rendering calls are required.
4. Add an Xcode app target only after a minimal source set and its required dependencies have been identified.
5. Keep the unsigned IPA workflow, but treat its artifact as a packaging/build result, not proof that the game is playable. An unsigned IPA is not ready for ordinary installation on an iPhone without a signing/sideloading path.

## Next source-level milestone

Trace the client startup path into the game loop, world creation, and rendering, then map every dependency reached by that path. Use that dependency map to select a small, real C++ source set for the first Xcode target. The first workflow run is expected to stop at the Xcode-project discovery step until an actual iOS project and shared scheme are committed; that failure is intentional and prevents a false IPA artifact.
