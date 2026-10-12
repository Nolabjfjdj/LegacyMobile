# Mobile renderer contract

## Goal

Create a mobile graphics boundary that can eventually be used by the existing game code without exposing Direct3D, Xbox, or PlayStation SDK types to Android or iOS. This document is a design contract, not an implemented renderer.

## Why this is the next blocker

The current `C4JRender` interface is not graphics-API-neutral. The Windows implementation surface includes `ID3D11Device`, `IDXGISwapChain`, `ID3D11Buffer`, `ID3D11ShaderResourceView`, and `D3D11_RECT`. The PSVita header also exposes some of those names alongside GXM types. The existing `glWrapper.cpp` forwards legacy OpenGL-style calls into `RenderManager`; it does not implement OpenGL itself.

The platform-specific `4J_Render` and `4J_Input` archives are built for their original toolchains and cannot simply be linked into Android NDK or iOS targets.

## Boundary rules

A future mobile backend must follow these rules:

- No Direct3D, Xbox, PlayStation, Windows SDK, or platform-specific GPU handle types in shared interfaces.
- Use fixed-width integer types and plain data structures for vertex data, rectangles, colours, and sizes.
- Keep GPU handles opaque to game code. A texture or buffer identifier must not be an `ID3D11*`, `SceGxm*`, or native API pointer.
- Make graphics context/device creation and surface resizing explicit.
- Define resource lifetime and context-loss behavior; Android may recreate its surface after pause/resume.
- Keep rendering calls on the graphics thread unless the backend explicitly guarantees otherwise.
- Preserve the semantics of the current API before attempting rendering optimisations.

## Required capabilities

| Area | Existing game-facing behavior to preserve | Mobile backend requirement |
| --- | --- | --- |
| Frame lifecycle | `Tick`, `StartFrame`, `Present`, `Clear` | Create/recreate a surface, begin a frame, clear colour/depth, present |
| Matrices | Identity, translate, rotate, scale, perspective, orthographic, push/pop, multiply/get | Maintain compatible matrix stacks and matrix conventions |
| Geometry | Triangle list/strip/fan, quad list, line list/strip | Convert unsupported primitive types such as quads to supported triangles |
| Vertex formats | Standard, compressed, lit, texture-generated | Reproduce each layout and its shader inputs |
| Shaders | Standard, projection, forced-LOD pixel shader modes | Implement and test equivalent shader variants |
| Textures | Create/free/bind, upload/update, parameters, texture levels | Track opaque handles, formats, filtering, wrapping, and mip levels |
| State | Blend, depth test/write/function, culling, colour mask, alpha test, polygon offset | Translate legacy state to explicit API state without leaking between draws |
| Lighting/fog | Two light slots, ambient colour, fog mode/range/density/colour | Match the existing fixed-function-style behavior in shaders |
| Command buffers | Create, record, clear, replay, deferred mode | Reproduce semantics or provide a verified compatible fallback |
| Capture | Thumbnail and screenshot methods | Read back a frame and encode an image without blocking normal gameplay excessively |

## Recommended implementation sequence

1. **Freeze the contract:** inventory every public method and every call site in the platform render headers. Record behavior, defaults, and special platform branches.
2. **Introduce neutral value types:** rectangles, clear flags, primitive types, vertex formats, shader modes, texture parameters, and opaque resource IDs. Do not change the existing console/desktop ABI in this step.
3. **Build a backend test harness:** create a mobile surface, clear it to a known colour, draw a textured triangle, resize, and recreate the surface after pause/resume.
4. **Implement one API at a time:** Android first with a supported NDK graphics path; keep the interface neutral so an iOS backend can be added. Do not report the game as running until the actual game renderer can render a frame.
5. **Port legacy state carefully:** add tests for matrix transforms, blending, depth, alpha testing, fog, lighting, texture upload, and each vertex format.
6. **Connect the game only after the harness passes:** then link the relevant game sources and replace the current platform render manager with the mobile implementation.

## Explicit non-goals

- This contract does not add a fake APK or IPA.
- It does not pretend that `glWrapper.cpp` is a complete renderer.
- It does not alter existing desktop or console rendering code.
- It does not claim that any mobile build is playable.

## Exit criteria

The renderer milestone is complete only when a mobile test application can create a graphics surface, draw and present the test scene, recreate the surface after lifecycle changes, and pass the backend tests on-device. A playable game milestone additionally requires the real game assets, game-core link, audio/storage/input services, world loading, and touch input to work.
