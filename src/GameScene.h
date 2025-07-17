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
#include "Config.h"

namespace rgb {
class IRReceiver;
}

class GameScene : public rgb::Scene {
  using Every = rgb::Every;
  using Duration = rgb::Duration;
  using Color = rgb::Color;
  using Timestamp = rgb::Timestamp;

public:
  static constexpr auto DESTROY_COLOR = Color::WHITE(.05f);
  static constexpr auto GAME_OVER_COLOR = Color::WHITE(.01f);
  constexpr static auto GHOST_COLOR = Color::WHITE(.01f);

  explicit GameScene(rgb::PixelGrid& grid, rgb::IRReceiver& irReceiver, AdafruitI2CGamepad& gamepad);

  auto update() -> void override;
  auto draw() -> void override;
  auto setup() -> void override;
  auto cleanup() -> void override;

private:
  auto newGame() -> void;

  auto processMoveResult(MoveResult& result) -> void;
  auto runRowClearAnimation(MoveResult& result) -> void;
  auto runGameOverAnimation() -> void;

  auto setupIRReceiver() -> void;
  auto setupGamepad() -> void;

  auto mapToColor(Cell cell) {
    Color color = Color::OFF();

    if (cell.destroying) {
      color += DESTROY_COLOR;
    }

    auto speed = Duration::Milliseconds(600);
    auto time = rgb::Clock::Now().mod(speed).to<float>() / speed.to<float>();
    switch (cell.type) {
      case PieceType::EMPTY:
        break;
      case PieceType::GHOST:
        color = GHOST_COLOR;
        break;
      case PieceType::GAMEOVER:
        color = Color::RED(.04).lerpWrap(Color::RED(.005), time);
        break;
      case PieceType::O:
        color += Color::YELLOW(.02f);
        break;
      case PieceType::J:
        color += Color::BLUE(.02f);
        break;
      case PieceType::L:
        color += Color::ORANGE(.02f);
        break;
      case PieceType::T:
        color += Color::CYAN(.02f);
        break;
      case PieceType::Z:
        color += Color::GREEN(.02f);
        break;
      case PieceType::S:
        color += Color::MAGENTA(.02f);
        break;
      case PieceType::I:
        color += Color::RED(.02f);
        break;
      default:
        ASSERT(false, "Unhandled PieceType");
    }
    return color;
  }

  Timestamp gameStartAt{};
  Timestamp gameEndAt{};
  Tetris<COLUMN_COUNT, ROW_COUNT> tetris{};
  rgb::PixelGrid& grid;
  rgb::IRReceiver& irReceiver;
  AdafruitI2CGamepad& gamepad;
  Every autoDropTimer = Every{Duration::Seconds(1), [this]() {
    auto result = tetris.movePiece({0, 1});
    processMoveResult(result);
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
