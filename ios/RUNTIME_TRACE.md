# Runtime trace for the iOS port

The CI inventory on 2026-10-12 parsed both Visual Studio project files: `Minecraft.Client` contains 668 C++ sources and 911 headers; `Minecraft.World` contains 715 C++ sources and 846 headers. The source tree exists; iOS platform integration is the blocker.

## Startup

`Minecraft.Client/Minecraft.cpp` defines the `Minecraft` singleton and lifecycle. `Minecraft::init()` initializes `McRegionLevelStorageSource` under a `saves` directory, loads options and texture packs, creates textures and fonts, constructs `GameRenderer` and `LevelRenderer`, issues legacy `gl*` state calls, starts a background downloader, creates the GUI, and selects the title screen. Therefore the first real target needs working asset/file access, a renderer, and thread support before this startup function can be used.

## Frame loop

The older monolithic `Minecraft::run()` implementation is disabled in the current source. Comments in `Minecraft.cpp` explain that the console port splits execution into `run_middle()` and `run_end()` so a platform-owned game loop can call those pieces. Before connecting iOS display callbacks, identify the real platform caller and the required ordering between input, simulation, rendering, and presentation.

## Renderer

`Minecraft.Client/glWrapper.cpp` routes legacy GL-style calls to `RenderManager`. The Windows renderer interface at `Minecraft.Client/Windows64/4JLibs/inc/4J_Render.h` exposes D3D11 types directly. A Metal backend must implement the rendering operations the game actually uses, or the interface must first be made platform-neutral.

## Startup reference is not portable

`Minecraft.Client/Windows64/Windows64_App.cpp` defines a desktop app object and `TemporaryCreateGameStart()`. That function relies on profile/storage managers, console app options, a fake local-player join, and a network-game thread. It is evidence of one existing start path, not an iOS entry point.

## Input

`Minecraft.Client/Input.cpp` reads movement through `InputManager.GetJoypadStick_LX/LY` and controller button state. A UIKit touch adapter should feed the existing gameplay action boundary without introducing UIKit dependencies into the world/gameplay code.

## Next dependency-tracing tasks

1. Find the platform caller for `run_middle()` and `run_end()` and document the call order.
2. Find the concrete `RenderManager` implementation and enumerate the renderer methods used by the client.
3. Trace local world creation to separate world generation from console profile/storage/network bootstrap.
4. Only then select source files for a real Xcode target; do not create a shell-only IPA and call it a game build.
