# iOS executable milestone

Updated: 2026-10-12

## Goal

The immediate goal is a genuine iOS app that links the game's real engine code, launches on an iPhone, and can be packaged as an unsigned IPA. Touch controls and Bluetooth controller integration are deferred. Existing `ios/input/` code and tests must be retained, not deleted.

A blank Swift/Objective-C shell or an IPA that does not contain a running game engine does not satisfy this milestone.

## Verified blockers

### 1. No Xcode app target exists

The current `.github/workflows/ios-build.yml` searches for an `.xcodeproj` or `.xcworkspace`. The last checked workflow run found zero projects and skipped scheme selection, archive, IPA packaging, and IPA upload. Its green status therefore reflects successful source inventory/input tests, not a built iOS app.

### 2. Renderer implementation is not present as portable source

- `Minecraft.Client/glWrapper.cpp` is a compatibility adapter. It forwards matrix, texture, draw-state, clear, and display-list operations to the global `RenderManager`.
- `Minecraft.Client/Windows64/4JLibs/inc/4J_Render.h` declares `C4JRender`, but its Windows contract exposes D3D11 types including `ID3D11Device`, `IDXGISwapChain`, `ID3D11Buffer`, `D3D11_RECT`, `D3DXIMAGE_INFO`, and `HRESULT`.
- The Windows64 4JLibs directory contains the render header and prebuilt `.lib` binaries, but no corresponding renderer implementation `.cpp` source. Equivalent renderer libraries are supplied for legacy consoles, not as an iOS/Metal backend.
- Therefore, simply adding the existing engine translation units to an Xcode project cannot produce a working renderer. An Apple-safe rendering contract and a Metal implementation (or another source-available portable backend) are required.

### 3. Startup is coupled to platform services

- `Minecraft.Client/Minecraft.cpp` creates game rendering systems and textures, uses legacy GL-style calls, loads save data under `saves`, starts a background downloader, and calls `RenderManager` for frame setup/presentation.
- The constructor uses `InitializeCriticalSection`; this is another Windows platform dependency to isolate.
- The desktop startup example in `Minecraft.Client/Windows64/Windows64_App.cpp` relies on console app/profile/storage/network-thread setup and is not an iOS entry point.
- `Minecraft.Client/stdafx.h` and `Minecraft.World/stdafx.h` select legacy platform SDK headers and do not yet provide a complete Apple path.

## Work sequence

1. **Build feasibility:** inventory the exact engine dependencies and identify the smallest source-available slice that can compile with Apple Clang. Keep engine sources, assets, and licenses intact.
2. **Apple platform layer:** add explicit Apple branches for types, synchronization, paths/filesystem, timing, logging, and lifecycle. Avoid broad fake compatibility headers that silently stub required engine behavior.
3. **Renderer:** define a portable subset of the renderer API actually used by the selected engine sources, then implement it with Metal. Initially aim for a real rendered engine-owned screen, then progress toward world rendering; do not report gameplay as working until it is actually exercised.
4. **Startup and assets:** establish a supported local startup path that initializes required assets and reaches the renderer without depending on console profiles/storage or an artificial network join.
5. **Xcode target:** add the real app target and shared scheme only once its sources and resources are known. Make the CI job fail if the project/scheme/archive is missing or packaging is skipped.
6. **IPA verification:** archive for a generic iOS device with signing disabled, package the resulting app into an IPA, upload it, and report clearly that an unsigned IPA still needs a valid signing/install route before it can be launched on a physical iPhone.
7. **Deferred milestone:** after the engine-backed app launches, connect the retained touch input foundation and GameController/DualSense mapping.

## Honest status

- Platform-neutral mobile input state and its C++ unit tests: implemented; CI tests have passed.
- iOS project, Metal renderer, real engine startup, and IPA: not implemented yet.
- No claim of a playable or launchable game should be made until the app target links the engine and a real device/simulator launch path is verified.
