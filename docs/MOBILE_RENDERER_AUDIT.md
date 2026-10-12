# Mobile renderer dependency audit

## Result

The current renderer cannot be linked into an Android or iOS application as-is. This is a source/dependency blocker, not merely a missing Gradle or Xcode project.

## Evidence in the repository

- `Minecraft.Client/Windows64/4JLibs/inc/4J_Render.h` exposes a `C4JRender` interface that directly mentions Direct3D 11 types, including `ID3D11Device`, `IDXGISwapChain`, `D3D11_RECT`, `ID3D11Buffer`, and `ID3D11ShaderResourceView`.
- The same interface contains a broad rendering contract: frame lifecycle, vertex submission, command buffers, textures, blend/depth/fog/lighting state, viewport handling, and screen capture. A mobile backend must implement the behavior the game actually uses; a no-op backend would not produce a playable game.
- `Minecraft.Client/glWrapper.cpp` routes legacy GL-style calls into `RenderManager`. It is an adapter to that interface, not a standalone OpenGL ES renderer.
- `Minecraft.Client/Windows64/4JLibs/libs` contains prebuilt Windows `.lib` files such as `4J_Render_PC.lib`, `4J_Render.lib`, and `4J_Input.lib`. `Minecraft.Client/PSVita/4JLibs/libs` contains platform-specific `.a` libraries. These artifacts are built for their respective platforms and cannot be assumed linkable by Android NDK or iOS toolchains.
- The input interface in `4J_Input.h` is centered on controller actions, remapping, and pad state. Touch must be translated into the existing game actions or supported by a mobile input implementation.

## Recommended implementation order

1. Locate all references to the renderer and input binary libraries in the Visual Studio project files, and record the full link dependency graph.
2. Determine whether the source for the Windows renderer implementation exists in the repository. If it is only in prebuilt libraries, do not try to link those libraries on mobile.
3. Define a platform-neutral renderer contract based on the subset of `C4JRender` actually called by the game, while preserving current desktop/console interfaces.
4. Implement and test a real mobile renderer backend. OpenGL ES is a candidate because the game already passes through a GL-like wrapper, but the wrapper alone does not remove the Direct3D-shaped backend contract.
5. Only after a minimal native C++ target can compile and render a test frame should Android Gradle/NDK and iOS/Xcode packaging be wired to build distributable apps.

## Validation status

- Dependency inspection: partial; the public interfaces and platform-specific library artifacts have been confirmed.
- Full linker dependency graph: not yet established.
- Android APK: not built.
- iOS IPA: not built.
- Playable mobile build: not validated.

Do not add a workflow that reports a successful APK or IPA until it compiles and packages the actual game implementation.
