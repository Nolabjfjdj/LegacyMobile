# Mobile porting assessment

## Current state

This repository is a native C++ game codebase organised around Visual Studio/MSBuild projects. The repository currently has no Android Gradle project, Android manifest, Xcode project/workspace, or root CMake build that can produce mobile applications.

This document records the source inspection and defines a staged route toward mobile support. It is not a claim that the game already builds on Android or iOS.

## Findings from the current source

### Rendering

- `Minecraft.Client/glWrapper.cpp` adapts legacy OpenGL-style calls to the global `RenderManager` interface. Examples include matrix operations, texture creation/binding, clearing, and colour state.
- The platform render API is declared as `C4JRender` in platform-specific `4JLibs/inc/4J_Render.h` headers. The Windows header ends with `extern C4JRender RenderManager;`.
- The Windows 64-bit render interface directly uses Direct3D 11 types such as `ID3D11Device`, `IDXGISwapChain`, `ID3D11Buffer`, and `D3D11_RECT`. The Windows UI renderer also calls the D3D11 GDraw backend.
- The PSVita header has a Vita GXM include, but its visible render interface still declares some D3D11-typed parameters. Platform headers therefore cannot be assumed to be cleanly portable just because similar APIs exist for each console.
- The tree contains platform-specific precompiled render libraries such as `4J_Render_PC.lib`, `4J_Render.lib`, and platform-specific GDraw source files. The actual implementation of the `C4JRender` abstraction is not present as an obvious portable source file in the repository tree. We must inspect linker dependencies and source availability before choosing whether to adapt an implementation or write a new backend.
- This means `glWrapper.cpp` is a useful boundary, but it is not by itself a complete mobile renderer. A mobile target needs an implementation for the render API, graphics resources, shaders, frame lifecycle, and presentation.

### Input

- `Minecraft.Client/Input.cpp` reads movement and look axes through `InputManager.GetJoypadStick_*` and checks action permissions through the game's local game mode.
- The repository includes platform-specific input headers and precompiled `4J_Input` libraries. The game-facing input path is controller-oriented, so a mobile target will need a supported implementation of that input interface or an adapter that injects touch-derived actions.
- Touch controls should be added at the input abstraction boundary so movement, camera look, jump, sneak, and menu actions can feed the existing gameplay code.

### Platform and engine setup

- `Minecraft.Client/stdafx.h` includes Windows headers and Direct3D 11 under `_WINDOWS64`, Xbox/Durango SDKs under their platform macros, and PlayStation SDK headers for those targets.
- `Minecraft.Client/Minecraft.cpp` contains the engine implementation and references many game systems. The existing Windows app startup in `Minecraft.Client/Windows64/Windows64_App.cpp` also initialises platform-specific game state.
- The Visual Studio projects contain configurations for existing desktop/console targets, but no mobile application entry point or mobile build system was found in the repository root.

## Porting plan

1. **Resolve renderer and input dependencies.** Inspect project linker settings and the use of platform libraries. Establish which implementations are available as source and which are binary-only before trying to link a mobile target.
2. **Define the mobile graphics backend.** Evaluate OpenGL ES for a shared Android/iOS backend or native graphics APIs where needed. Implement the complete game-facing render API, including vertex formats, textures, shader behavior, state, command buffers, and presentation.
3. **Separate platform services.** Add mobile implementations for application lifecycle, filesystem/save paths, audio, timing, and platform-specific UI/services. Do not include Windows/Xbox/PlayStation SDK headers in mobile targets.
4. **Add touch input through the existing action layer.** Provide a virtual stick, camera-look area, and buttons that map to existing gameplay actions; retain controller support where available.
5. **Create native build projects.** Start with an Android NDK/CMake build once the source list and portable dependencies are known. Add an iOS Xcode target using the same portable C++ core and a native iOS lifecycle/rendering bridge.
6. **Validate in stages.** First compile a minimal native target, then link the world/core code, then initialise rendering, load a world, and verify touch input. Only publish APK/IPA artifacts after those checks pass.

## Current validation status

- Source inspection: initial pass completed.
- Android APK build: not configured or validated.
- iOS IPA build: not configured or validated.
- Playable mobile build: not yet available.

Do not treat a successful placeholder workflow or an empty package as a successful game port.
