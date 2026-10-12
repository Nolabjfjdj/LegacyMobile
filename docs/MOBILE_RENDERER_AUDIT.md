# Mobile renderer dependency audit

## Result

The current renderer cannot be linked into an Android or iOS application as-is. This is a source/dependency blocker, not merely a missing Gradle or Xcode project.

## Evidence in the repository

- `Minecraft.Client/Windows64/4JLibs/inc/4J_Render.h` exposes a `C4JRender` interface that directly mentions Direct3D 11 types, including `ID3D11Device`, `IDXGISwapChain`, `D3D11_RECT`, `ID3D11Buffer`, and `ID3D11ShaderResourceView`.
- The same interface contains a broad rendering contract: frame lifecycle, vertex submission, command buffers, textures, blend/depth/fog/lighting state, viewport handling, and screen capture. A mobile backend must implement the behavior the game actually uses; a no-op backend would not produce a playable game.
- `Minecraft.Client/glWrapper.cpp` routes legacy GL-style calls into `RenderManager`. It is an adapter to that interface, not a standalone OpenGL ES renderer. In particular, `Display::update()` and `Display::swapBuffers()` are empty in this file.
- The repository tree contains platform-specific prebuilt renderer libraries under `Minecraft.Client/Windows64/4JLibs/libs`, `Minecraft.Client/Xbox/4JLibs/libs`, `Minecraft.Client/Durango/4JLibs/libs`, `Minecraft.Client/PS3/4JLibs/libs`, `Minecraft.Client/PSVita/4JLibs/libs`, and `Minecraft.Client/Orbis/4JLibs/Libs`. These artifacts are built for their respective platforms and cannot be assumed linkable by Android NDK or iOS toolchains.
- The source audit scans C/C++/Objective-C++ implementation files under `Minecraft.Client` for definitions of `C4JRender::...`. Latest recorded audit (commit `96907564a161ef2cac4f32a6d70fe4e0ba2a34d5`, workflow run [38296663250](https://github.com/Nolabjfjdj/LegacyMobile/actions/runs/38296663250)) found 92 declarations in the header, zero visible portable definitions, and zero visible platform-specific definitions matching that class/method pattern. This means the implementation is not available as matching source definitions in the scanned tree; the prebuilt libraries are not source code and the audit does not inspect their object symbols.
- The input interface in `4J_Input.h` is centered on controller actions, remapping, and pad state. Touch must be translated into the existing game actions or supported by a mobile input implementation.

## What the audit does and does not prove

The result does not prove that no renderer implementation exists anywhere in the original development history or external build environment. It shows that the current repository checkout does not expose a matching renderer implementation as source that can simply be compiled for iOS. The platform libraries may contain implementations, but their binaries are not a portable substitute for source and must not be treated as iOS-compatible without verifying their format and ABI.

## Recommended implementation order

1. Establish the actual renderer library link dependencies for each existing platform target, including the platform-specific project configuration and any library/object metadata available in the repository.
2. Confirm whether a usable renderer implementation source can be recovered from the project's own source/history. If it exists only in prebuilt platform libraries, do not try to link those libraries on mobile.
3. Define a platform-neutral renderer contract based on the subset of `C4JRender` actually called by the game, while preserving current desktop/console interfaces.
4. Implement and test a real mobile renderer backend. OpenGL ES is a candidate because the game already passes through a GL-like wrapper, but the wrapper alone does not remove the Direct3D-shaped backend contract. Metal is the native Apple option if the backend is implemented directly for iOS.
5. Only after a minimal native C++ target can compile and render a test frame should Android Gradle/NDK and iOS/Xcode packaging be wired to build distributable apps.

## Validation status

- Public renderer interfaces and platform-specific library artifacts: confirmed.
- Source definitions matching `C4JRender::...` in the scanned source tree: none detected by the current audit.
- Full linker dependency graph: not yet established.
- Android APK: not built.
- iOS IPA: not built.
- Playable mobile build: not validated.

Do not add a workflow that reports a successful APK or IPA until it compiles and packages the actual game implementation.
