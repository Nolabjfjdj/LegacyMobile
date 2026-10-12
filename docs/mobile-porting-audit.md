# Mobile Porting Audit

## Current status

This branch records the first source-level checks needed before adding Android APK and iOS IPA workflows. A workflow must compile the real game; producing an empty application or a placeholder binary is not considered a successful port.

## Findings from the current source tree

- The repository has Visual Studio solution/project files (`MinecraftPC.sln`, `MinecraftConsoles.sln`, and `.vcxproj` files), but no root `CMakeLists.txt`, Android Gradle project, or Xcode project was found in the inspected tree.
- `Minecraft.Client/glWrapper.cpp` routes legacy-style graphics calls into the global `RenderManager` API. The wrapper covers matrix operations, texture operations, render state, and display-list-like command buffers.
- The platform-specific `4J_Render.h` headers expose a large rendering interface. The Windows header uses Direct3D 11 types such as `ID3D11Device`, `IDXGISwapChain`, `ID3D11Buffer`, and `ID3D11ShaderResourceView`.
- The repository tree contains prebuilt platform-specific render libraries (for example, `4J_Render.lib` and console `.a` files), but the inspected paths do not expose a matching portable renderer implementation source file.
- `Minecraft.Client/Input.cpp` reads player movement and look input through `InputManager`; `4J_Input.h` exposes stick axes, triggers, and mapped actions. A mobile backend will need to feed these existing actions from touch controls (and optionally physical controllers).

## Recommended implementation order

1. Locate or provide the source implementation behind `C4JRender`; if it is not available, implement a new backend for the required rendering interface rather than trying to link the Windows/console binaries on mobile.
2. Create a platform-independent rendering boundary and implement an Android-compatible backend first. Reuse the existing game/rendering call sites wherever possible.
3. Add an input adapter that maps touchscreen controls to the existing `InputManager` actions.
4. Add the Android NDK/Gradle app wrapper and compile it on GitHub Actions. Only publish an APK artifact after a real native build succeeds.
5. Port the same boundary to iOS, add the Xcode/iOS app wrapper, and produce an IPA artifact from a successful macOS runner build.

## Validation rule

A green workflow is not proof that the game is playable. Before calling either artifact playable, verify that it launches, renders the title/menu, accepts input, and can enter a world on the target platform.