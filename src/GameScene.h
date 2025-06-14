//
// Created by Brandon on 6/1/25.
//

#ifndef TETRISGAME_GAMESCENE_H
#define TETRISGAME_GAMESCENE_H

#include "Scene.h"
#include "PixelList.h"
#include "Tetris.h"
#include "Timer.h"
#include "AdafruitI2CGamepad.h"
#include "Trigger.h"

namespace rgb {
class IRReceiver;
}

class GameScene : public rgb::Scene {
  using Every = rgb::Every;
  using Duration = rgb::Duration;

public:
  constexpr static auto SIZE = 8;

  explicit GameScene(rgb::PixelList& grid, rgb::IRReceiver& irReceiver, rgb::AdafruitI2CGamepad& gamepad);

  auto update() -> void override;
  auto draw() -> void override;
  auto setup() -> void override;
  auto cleanup() -> void override;

private:
  auto newGame() -> void;

  auto processMoveResult(MoveResult& result) -> void;
  auto runRowClearAnimation(MoveResult& result) -> void;

  Tetris<SIZE, SIZE> tetris{};
  rgb::PixelList& grid;
  rgb::IRReceiver& irReceiver;
  rgb::AdafruitI2CGamepad& gamepad;
  Every autoDropTimer = Every{Duration::Seconds(1), [this]() {
//    auto result = tetris.moveTetromino({0, 1});
//    processMoveResult(result);
  }};
  rgb::TimerHandle rowClearAnimationHandle{};
  rgb::Trigger leftTrigger{[&](){
    return gamepad.analogX <= .03f;
  }};
  rgb::Trigger rightTrigger{[&](){
    return gamepad.analogX >= .97f;
  }};
  rgb::Trigger downTrigger{[&](){
    return gamepad.analogY <= .03f;
  }};
  bool inAnimation{false};
};


#endif //TETRISGAME_GAMESCENE_H
