# iOS porting map

This document records source paths verified in the repository. It is a dependency map, not a claim that the game can currently build on iOS.

## Priority order (updated 2026-10-12)

**Current milestone: produce a real, launchable iOS app containing the game engine.** Do not spend the next implementation steps on touch UI or Bluetooth controls; keep the existing platform-neutral input work intact for a later milestone.

1. Establish a minimal iOS host and compile a real, deliberately small subset of the engine on the macOS runner. A placeholder app that launches without the engine does not count as completion.
2. Resolve the first compile blockers in dependency order: platform headers and synchronization/types, filesystem/save paths, and the renderer contract.
3. Implement enough Metal-backed rendering and startup to reach an actual engine-owned screen/frame. Do not report an IPA as playable until the engine is linked and its launch path is exercised.
4. Add the Xcode project/scheme and make the workflow fail if it cannot find/build the intended target. Package an unsigned IPA only after a successful archive.
5. Only after that executable milestone, integrate the saved touch controls and GameController/DualSense adapter.

The current CI success for source inventory and input unit tests is not an iOS build. In the last checked run, the Xcode project lookup found zero projects, so archive and IPA steps were skipped. Keep that distinction explicit in all future build reports.

## Verified game entry and runtime

- `Minecraft.Client/Minecraft.cpp` owns the `Minecraft` runtime. It defines the singleton `Minecraft::m_instance`, constructs the runtime in `Minecraft::Minecraft(...)`, and declares the main lifecycle methods `init()`, `run()`, `run_middle()`, `run_end()`, and `tick(...)`.
- The constructor initializes substantial application state, including timer, level, renderer, player, GUI, textures, sound, and synchronization objects. It also calls `InitializeCriticalSection`, so the current implementation assumes a Windows-style synchronization API in at least this path.
- `Minecraft.Client/Windows64/Windows64_App.cpp` defines `CConsoleMinecraftApp app` and a desktop-specific `TemporaryCreateGameStart()`. That function sets game options, resets console storage, fakes a local player join, and launches `CGameNetworkManager::RunNetworkGameThreadProc` through `C4JThread`. It is a diagnostic reference, not a safe iOS entry point.
- The game start path therefore cannot be copied into an iOS app unchanged: it depends on console profile/storage/app services and a network-game thread.

## Rendering boundary

- `Minecraft.Client/glWrapper.cpp` maps legacy OpenGL-style calls such as `glTranslatef`, `glMatrixMode`, `glClear`, and display-list calls onto the global `RenderManager`.
- The Windows implementation contract is in `Minecraft.Client/Windows64/4JLibs/inc/4J_Render.h`. Its API directly exposes Direct3D 11 types, including `ID3D11Device`, `IDXGISwapChain`, `ID3D11Buffer`, and `D3D11_RECT`.
- As a result, `glWrapper.cpp` is not a renderer by itself. A Metal renderer must implement the operations consumed by the game and replace D3D-specific types at the platform boundary, or the renderer interface must first be made platform-neutral.
- The platform-specific renderer libraries under the Windows/console folders are not an iOS-compatible implementation simply because the legacy wrapper resembles OpenGL.

## Input boundary

- `Minecraft.Client/Input.cpp` reads movement axes through `InputManager.GetJoypadStick_LX/LY`, queries game-action permissions, and reads controller-button state from the local player.
- A touch layer should feed the same gameplay action/input state through a dedicated iOS input adapter. It should not make the core `Input::tick` depend directly on UIKit.
- The input adapter still depends on app/game-mode/player state, so selecting `Input.cpp` alone is not enough to create a useful first build target.

## Platform header problem

- `Minecraft.Client/stdafx.h` includes `Common/Consoles_App.h` and selects one of the platform-specific `4JLibs/inc/4J_*.h` headers. Its fallback branch includes Orbis-specific headers when none of the earlier platform macros match.
- `Minecraft.Client/Common/Consoles_App.h` includes `<xuiapp.h>` and console UI/audio headers. It is not an Apple-safe common application header in its current form.
- `Minecraft.World/stdafx.h` includes platform SDK headers conditionally and falls back to a generic path that includes `<sal.h>`; that annotation header is not an iOS SDK header.
- Defining a new `IOS` macro alone would not solve this. The common headers need a deliberate Apple branch and a review of types, filesystem, threading, timing, audio, and platform services.

## Blockers before an honest Xcode target

1. Define an Apple platform abstraction for types, synchronization, filesystem/save access, timing, input, audio, and application lifecycle.
2. Identify the renderer implementation source and all functions called through `RenderManager`; design a Metal-backed implementation that satisfies the required contract.
3. Trace a supported local-world creation path that does not require console profiles, console storage, or a fake network join.
4. Build a dependency list for that path, separating portable world logic from client/UI/platform-specific translation units.
5. Add the Xcode app target only when the selected real sources and platform adapters are known. The first target should fail loudly if required engine sources are omitted; it must not present a blank shell as a successful game port.

## Evidence links

- [Minecraft runtime](../Minecraft.Client/Minecraft.cpp)
- [Desktop startup reference](../Minecraft.Client/Windows64/Windows64_App.cpp)
- [Legacy graphics wrapper](../Minecraft.Client/glWrapper.cpp)
- [Windows renderer interface](../Minecraft.Client/Windows64/4JLibs/inc/4J_Render.h)
- [Gameplay input](../Minecraft.Client/Input.cpp)
- [Client platform precompiled header](../Minecraft.Client/stdafx.h)
- [World platform precompiled header](../Minecraft.World/stdafx.h)
