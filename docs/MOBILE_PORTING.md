# Mobile porting assessment

## Current state

This repository is a native C++ game codebase organised around Visual Studio/MSBuild projects. The repository currently has no Android Gradle project, Android manifest, Xcode project/workspace, or root CMake build that can produce mobile applications.

This document records the first source inspection and defines a safe sequence for adding mobile support. It is not a claim that the game already builds on Android or iOS.

## Findings from the current source

### Rendering

- `Minecraft.Client/glWrapper.cpp` adapts a set of legacy OpenGL-style calls to the global `RenderManager` interface. Examples include matrix operations, texture creation/binding, clearing, and colour state.
- The platform render interface is exposed through `C4JRender` in platform-specific `4JLibs/inc/4J_Render.h` headers.
- The Windows 64-bit implementation's render header takes Direct3D 11 types such as `ID3D11Device`, `IDXGISwapChain`, and `D3D11_RECT`. The Windows UI renderer also calls the D3D11 GDraw backend.
- The PSVita render header uses the Vita GXM SDK, but its visible `C4JRender::Initialise` declaration still mentions D3D11 types. This is a warning that platform headers and type aliases must be checked carefully before reusing them in a mobile build.
- Existing GDraw backends are platform-specific (for example D3D11 on Windows and GXM on Vita). A mobile port needs a deliberate graphics backend or an adapter, not just a new compiler target.

### Input

- `Minecraft.Client/Input.cpp` reads movement and look axes through `InputManager.GetJoypadStick_*` and checks action permissions through the game's local game mode.
- The current player-input path assumes a gamepad-oriented input manager. Touch controls should be added at the input abstraction boundary so movement, camera look, jump, sneak, and menu actions can feed the existing gameplay code.

### Platform and engine setup

- `Minecraft.Client/stdafx.h` includes Windows headers and Direct3D 11 under `_WINDOWS64`, Xbox/Durango SDKs under their platform macros, and PlayStation SDK headers for those targets.
- `Minecraft.Client/Minecraft.cpp` contains the engine implementation and references many game systems. The existing Windows app startup in `Minecraft.Client/Windows64/Windows64_App.cpp` also initialises platform-specific game state.
- The repository currently has no mobile application entry point or mobile build system in its root.

## Porting plan

1. **Map the renderer boundary.** Identify the concrete `C4JRender` implementation and all dependencies used by `RenderManager`, including vertex buffers, textures, shaders, frame lifecycle, and presentation.
2. **Choose a mobile graphics path.** Evaluate a shared OpenGL ES backend for Android and iOS, or separate native backends where required. Keep the game-facing rendering API stable where practical.
3. **Separate platform services.** Add mobile implementations for application lifecycle, filesystem/save paths, audio, timing, and platform-specific UI/services. Do not include Windows/Xbox/PlayStation SDK headers in mobile targets.
4. **Add touch input through the existing action layer.** Provide a virtual stick, camera-look area, and buttons that map to the existing gameplay actions; retain controller support where available.
5. **Create native build projects.** Start with an Android NDK/CMake build once the source list and portable dependencies are known. Add an iOS Xcode target using the same portable C++ core and a native iOS lifecycle/rendering bridge.
6. **Validate in stages.** First compile a minimal native target, then link the world/core code, then initialise rendering, load a world, and verify touch input. Only publish APK/IPA artifacts after those checks pass.

## Current validation status

- Source inspection: initial pass completed.
- Android APK build: not configured or validated.
- iOS IPA build: not configured or validated.
- Playable mobile build: not yet available.

Do not treat a successful placeholder workflow or an empty package as a successful game port.
