# iOS porting work

## Purpose

This directory is reserved for the native iOS port of LegacyMobile. The goal is to run the existing C++ game, not to replace it with a demo or a newly written voxel prototype.

**Current status: planning only.** There is no Xcode project, iOS application target, iOS renderer, or playable IPA in this repository yet.

## Architecture to target

- Keep portable game/world logic in C++ wherever its dependencies allow it.
- Use a native iOS application lifecycle and Objective-C++ (.mm) only at the boundary between Apple frameworks and C++.
- Prefer Metal as the candidate renderer for iOS. The existing Windows renderer exposes Direct3D 11 types and its prebuilt libraries cannot be linked into an iOS target.
- Add touch controls at the existing game-action/input boundary so the game logic can continue to consume movement, look, jump, and menu actions.
- Keep Apple-only code out of the existing Windows and console build configurations.

## Required work before a playable iOS build

1. Inventory the C++ source files and dependencies that can be compiled without Windows or console SDKs.
2. Identify whether the renderer implementation is available as source. The current repository exposes the C4JRender interface and platform-specific prebuilt render libraries; a Metal implementation will be needed unless a compatible source implementation is found.
3. Define a platform-neutral rendering boundary that does not expose Direct3D 11 types.
4. Replace or adapt the platform services required by the game: application lifecycle, file/save access, audio, timing, and input.
5. Create an Xcode target that compiles the actual selected C++ sources and the Objective-C++ bridge.
6. Validate progressively on a macOS/Xcode build runner and, ultimately, on an iOS device. Do not publish an IPA as playable until a world loads, renders, accepts input, and can be exited safely.

## Validation policy

A successful build of an empty app shell is not a successful game port. Any future CI job must report the exact target it compiled and must not label a shell-only build as a playable game. An IPA should only be distributed after the game integration has passed the checks above.

## First milestone

Establish a reproducible source/dependency inventory for the existing C++ engine and determine the smallest real set of game sources that can compile without desktop/console platform headers. Do not create an Xcode project by guessing the source list before this inventory is complete.
