#include "MobileInputState.h"

#include <algorithm>
#include <cmath>

namespace legacy_mobile {

bool InputSnapshot::isPressed(InputButton button) const {
    const auto index = static_cast<std::uint8_t>(button);
    if (index >= static_cast<std::uint8_t>(InputButton::Count)) {
        return false;
    }
    return (buttons & (std::uint32_t{1} << index)) != 0;
}

float MobileInputState::clampAxis(float value) {
    if (!std::isfinite(value)) {
        return 0.0f;
    }
    return std::max(-1.0f, std::min(1.0f, value));
}

std::uint32_t MobileInputState::buttonMask(InputButton button) {
    const auto index = static_cast<std::uint8_t>(button);
    if (index >= static_cast<std::uint8_t>(InputButton::Count)) {
        return 0;
    }
    return std::uint32_t{1} << index;
}

void MobileInputState::setButton(std::uint32_t& buttons, InputButton button, bool pressed) {
    const auto mask = buttonMask(button);
    if (pressed) {
        buttons |= mask;
    } else {
        buttons &= ~mask;
    }
}

void MobileInputState::setControllerConnected(bool connected) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (connected && !controllerConnected_) {
        touchMoveX_ = 0.0f;
        touchMoveY_ = 0.0f;
        touchLookX_ = 0.0f;
        touchLookY_ = 0.0f;
        touchButtons_ = 0;
    }
    controllerConnected_ = connected;
    if (!connected) {
        controllerMoveX_ = 0.0f;
        controllerMoveY_ = 0.0f;
        controllerLookX_ = 0.0f;
        controllerLookY_ = 0.0f;
        controllerButtons_ = 0;
    }
}

bool MobileInputState::shouldShowTouchControls() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return !controllerConnected_;
}

void MobileInputState::setTouchMovement(float x, float y) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!controllerConnected_) {
        touchMoveX_ = clampAxis(x);
        touchMoveY_ = clampAxis(y);
    }
}

void MobileInputState::addTouchLookDelta(float x, float y) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!controllerConnected_) {
        touchLookX_ += clampAxis(x);
        touchLookY_ += clampAxis(y);
    }
}

void MobileInputState::setTouchButton(InputButton button, bool pressed) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!controllerConnected_) {
        setButton(touchButtons_, button, pressed);
    }
}

void MobileInputState::setControllerAxes(float moveX, float moveY, float lookX, float lookY) {
    std::lock_guard<std::mutex> lock(mutex_);
    controllerMoveX_ = clampAxis(moveX);
    controllerMoveY_ = clampAxis(moveY);
    controllerLookX_ = clampAxis(lookX);
    controllerLookY_ = clampAxis(lookY);
}

void MobileInputState::setControllerButton(InputButton button, bool pressed) {
    std::lock_guard<std::mutex> lock(mutex_);
    setButton(controllerButtons_, button, pressed);
}

InputSnapshot MobileInputState::snapshot() const {
    std::lock_guard<std::mutex> lock(mutex_);
    InputSnapshot result;
    result.controllerConnected = controllerConnected_;
    if (controllerConnected_) {
        result.moveX = controllerMoveX_;
        result.moveY = controllerMoveY_;
        result.lookX = controllerLookX_;
        result.lookY = controllerLookY_;
        result.buttons = controllerButtons_;
    } else {
        result.moveX = touchMoveX_;
        result.moveY = touchMoveY_;
        result.lookX = touchLookX_;
        result.lookY = touchLookY_;
        result.buttons = touchButtons_;
    }
    return result;
}

InputSnapshot MobileInputState::consumeFrame() {
    std::lock_guard<std::mutex> lock(mutex_);
    InputSnapshot result;
    result.controllerConnected = controllerConnected_;
    if (controllerConnected_) {
        result.moveX = controllerMoveX_;
        result.moveY = controllerMoveY_;
        result.lookX = controllerLookX_;
        result.lookY = controllerLookY_;
        result.buttons = controllerButtons_;
    } else {
        result.moveX = touchMoveX_;
        result.moveY = touchMoveY_;
        result.lookX = touchLookX_;
        result.lookY = touchLookY_;
        result.buttons = touchButtons_;
        touchLookX_ = 0.0f;
        touchLookY_ = 0.0f;
    }
    return result;
}

}
