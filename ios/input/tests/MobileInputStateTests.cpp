#include "../MobileInputState.h"

#include <cassert>
#include <cmath>
#include <limits>

using legacy_mobile::InputButton;
using legacy_mobile::MobileInputState;

int main() {
    MobileInputState input;
    assert(input.shouldShowTouchControls());

    input.setTouchMovement(2.0f, -0.5f);
    input.addTouchLookDelta(0.25f, -0.125f);
    input.setTouchButton(InputButton::Jump, true);
    auto touch = input.consumeFrame();
    assert(std::fabs(touch.moveX - 1.0f) < 0.001f);
    assert(std::fabs(touch.moveY + 0.5f) < 0.001f);
    assert(std::fabs(touch.lookX - 0.25f) < 0.001f);
    assert(touch.isPressed(InputButton::Jump));
    assert(std::fabs(input.consumeFrame().lookX) < 0.001f);

    input.setControllerConnected(true);
    assert(!input.shouldShowTouchControls());
    input.setControllerAxes(-0.75f, 0.5f, 0.2f, -0.3f);
    input.setControllerButton(InputButton::PrimaryAction, true);
    input.setTouchMovement(0.9f, 0.9f);
    input.setTouchButton(InputButton::Jump, true);
    auto controller = input.snapshot();
    assert(controller.controllerConnected);
    assert(std::fabs(controller.moveX + 0.75f) < 0.001f);
    assert(controller.isPressed(InputButton::PrimaryAction));
    assert(!controller.isPressed(InputButton::Jump));

    input.setControllerConnected(false);
    assert(input.shouldShowTouchControls());
    auto disconnected = input.snapshot();
    assert(!disconnected.controllerConnected);
    assert(std::fabs(disconnected.moveX - 1.0f) < 0.001f);
    assert(!disconnected.isPressed(InputButton::PrimaryAction));

    input.setTouchMovement(std::numeric_limits<float>::quiet_NaN(), 0.0f);
    assert(std::fabs(input.snapshot().moveX) < 0.001f);
    return 0;
}
