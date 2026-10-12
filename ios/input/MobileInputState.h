#pragma once

#include <cstdint>
#include <mutex>

namespace legacy_mobile {

enum class InputButton : std::uint8_t {
    Jump = 0,
    PrimaryAction,
    SecondaryAction,
    Inventory,
    Pause,
    Sprint,
    Sneak,
    Count
};

struct InputSnapshot {
    float moveX = 0.0f;
    float moveY = 0.0f;
    float lookX = 0.0f;
    float lookY = 0.0f;
    std::uint32_t buttons = 0;
    bool controllerConnected = false;

    bool isPressed(InputButton button) const;
};

class MobileInputState {
public:
    void setControllerConnected(bool connected);
    bool shouldShowTouchControls() const;

    void setTouchMovement(float x, float y);
    void addTouchLookDelta(float x, float y);
    void setTouchButton(InputButton button, bool pressed);

    void setControllerAxes(float moveX, float moveY, float lookX, float lookY);
    void setControllerButton(InputButton button, bool pressed);

    InputSnapshot snapshot() const;
    InputSnapshot consumeFrame();

private:
    static float clampAxis(float value);
    static std::uint32_t buttonMask(InputButton button);
    static void setButton(std::uint32_t& buttons, InputButton button, bool pressed);

    mutable std::mutex mutex_;
    bool controllerConnected_ = false;
    float touchMoveX_ = 0.0f;
    float touchMoveY_ = 0.0f;
    float touchLookX_ = 0.0f;
    float touchLookY_ = 0.0f;
    std::uint32_t touchButtons_ = 0;
    float controllerMoveX_ = 0.0f;
    float controllerMoveY_ = 0.0f;
    float controllerLookX_ = 0.0f;
    float controllerLookY_ = 0.0f;
    std::uint32_t controllerButtons_ = 0;
};

}
