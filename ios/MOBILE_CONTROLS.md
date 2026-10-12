# Mobile controls and controller integration

This is the input contract for the future iOS app target. It is deliberately independent of UIKit and GameController so the input arbitration rules can be compiled and tested before an Xcode target exists.

## Touch layout

The intended in-game overlay is:

- A floating virtual joystick on the lower-left for movement.
- Dragging the right side of the gameplay view to rotate the camera.
- Contextual action buttons on the lower-right: jump, primary action, secondary action/use, inventory, and pause. Only buttons that the current screen/game mode supports should be shown.
- A settings option to reposition/resize the overlay and adjust opacity.
- Touch input should not be rendered over menus that already have their own controls.

The overlay is an iOS UI layer. It must not add UIKit imports to the C++ world or gameplay sources.

## Bluetooth controllers

Use Apple's GameController framework in the iOS host layer to discover controllers, observe connection/disconnection, and read extended gamepad axes/buttons. iOS exposes supported Bluetooth controllers through this framework; the app should not attempt to implement Bluetooth HID itself.

The DualSense should use the standard extended-gamepad mapping where iOS exposes it. Initial mapping target:

| Controller input | Engine input intent |
| --- | --- |
| Left stick | Move |
| Right stick | Look |
| Cross (bottom face button) | Jump |
| R2 | Primary action / break |
| L2 | Secondary action / use |
| Options/Menu | Pause |
| Square / other face buttons | Contextual action or inventory, finalized after the existing action mapping is traced |

The mapping is a proposal, not yet connected to InputManager. Existing gameplay input reads InputManager.GetJoypadStick_LX/LY, GetJoypadStick_RX/RY, and action values. The iOS adapter must translate GameController state into that boundary or a platform-neutral replacement; it must not make Input::tick depend on UIKit.

## Input ownership and overlay visibility

MobileInputState establishes deterministic source priority:

- With no controller connected, touch input is active and touch controls should be visible.
- When a controller connects, controller state becomes authoritative and the touch overlay should be hidden.
- Touch button/movement changes are ignored while a controller is connected.
- When the controller disconnects, touch controls become visible again and the touch source resumes.
- Controller disconnection clears stale controller axes and button states.
- Look deltas are consumed once per frame; movement axes and held buttons persist until changed.

This is connection-based, matching the requested behavior. A later settings option may allow manual override, but it should not be necessary for the default behavior.

## Current implementation status

- ios/input/MobileInputState.h/.cpp implements and tests the platform-neutral input arbitration/state.
- The iOS GameController discovery, touch overlay rendering, and mapping into the legacy InputManager still require the native iOS app target and engine input adapter.
- This work does not claim that a controller already works in a built app. The current repository still has no Xcode project, so iOS integration and IPA generation remain blocked until the actual app target and renderer are implemented.
