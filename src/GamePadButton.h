//
// Created by Brandon on 2/17/25.
//

#ifndef RGBLIB_GAMEPADBUTTON_H
#define RGBLIB_GAMEPADBUTTON_H


#include <memory>
#include "Func.h"
#include "Types.h"
#include "ButtonState.h"

class GamePadButton {
  static constexpr auto doNothing() -> void {}

public:
  explicit GamePadButton();
  auto onPress(Runnable callback) noexcept -> GamePadButton&;

  auto update(bool pressed) -> rgb::ButtonState;
  auto getState() const noexcept -> rgb::ButtonState;
  auto isPressed() const noexcept -> bool { return state == rgb::ButtonState::PRESS || state == rgb::ButtonState::PRESSED; };

private:
  rgb::ButtonState state;
  Runnable onPressCallback;
};


#endif //RGBLIB_GAMEPADBUTTON_H
