# iOS source audit

This is a source-level inventory, not a successful build report. The repository has not yet been compiled for iOS.

## Findings from the current repository

- `Minecraft.World/Minecraft.World.vcxproj` lists 715 C++ translation units and 846 headers.
- Its project configurations target ARM64EC, Durango, ORBIS, PS3, PSVita, Win32, x64, and Xbox 360. There is no iOS configuration.
- The project contains console-specific save implementations such as `ConsoleSaveFileInputStream.cpp`, `ConsoleSaveFileOutputStream.cpp`, and other `ConsoleSaveFile*.cpp` files. These must not be added blindly to an iOS target.
- Platform macros in the project include `_XBOX`, `__PS3__`, `__PSVITA__`, `WIN32`, and `_DURANGO`. Source selection and platform abstractions must be reviewed before compiling on Apple platforms.
- `Minecraft.Client/Minecraft.Client.vcxproj` currently has an empty file body in the GitHub repository response. Its client build configuration cannot be used as a reliable source list until that is checked and repaired.
- No `.xcodeproj` or `.xcworkspace` has been added yet. Therefore, an IPA build cannot succeed today; the workflow reports this explicitly instead of packaging a fake app.

## Porting implications

1. Do not try to compile all 715 world translation units as one first step. Identify the actual app entry point, client dependencies, platform services, and renderer dependencies first.
2. Separate portable game/world logic from filesystem, threading, audio, input, and console-save code.
3. Do not assume the Windows renderer can be reused on iOS. A real iOS rendering backend and a shared boundary from game rendering calls are required.
4. Add an Xcode app target only after a minimal source set and its required dependencies have been identified.
5. Keep the unsigned IPA workflow, but treat its artifact as a packaging/build result, not proof that the game is playable. An unsigned IPA is not ready for ordinary installation on an iPhone without a signing/sideloading path.

## Next source-level milestone

Trace the client startup path into world creation and rendering, then map every dependency reached by that path. Use that dependency map to select a small, real C++ source set for the first Xcode target.
