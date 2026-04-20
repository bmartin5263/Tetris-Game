//
// Created by Brandon on 4/19/26.
//

#ifndef TETRISGAME_TETRISAPPLICATION_H
#define TETRISGAME_TETRISAPPLICATION_H

#include "UserApplication.h"
#include "SevenSegmentDisplay.h"
#include "FastLEDMatrix.h"
#include "Clock.h"
#include "Every.h"
#include "Trigger.h"
#include "Types.h"
#include "MoveResult.h"
#include "Piece.h"
#include "Tetris.h"
#include "AdafruitI2CGamepad.h"

using namespace rgb;

auto sevenSegDisplay = MAX7219EightDigitSevenSegmentDisplay{D5};
auto gamepad = AdafruitI2CGamepad{};
auto grid = FastLEDMatrix<8, 8, D2_RGB, RgbwSupport::ENABLE, 3, 2>();

class TetrisApplication : public UserApplication<> {
  static constexpr auto DESTROY_COLOR = Color::WHITE() * .05f;
  static constexpr auto GAME_OVER_COLOR = Color::WHITE() * .01f;
  constexpr static auto GHOST_COLOR = Color::WHITE() * .01f;
  constexpr static auto GAME_OVER_LOW_COLOR = Color::RED() * .005f;
  constexpr static auto GAME_OVER_HIGH_COLOR = Color::RED() * .04f;

protected:
  auto configure(Configurer& app) -> void override {
    grid.setBrightness(1.0f);
    app.addLEDs(grid);
    app.addSensor(gamepad);
    app.useHeartbeatLED();

    SPI.begin();
    sevenSegDisplay.start();
    gamepad.start();

    setupGamepad();
    INFO("NEW GAME");
    newGame();
  }

  auto update() -> void override {
    if (leftTrigger.test()) {
      if (!inAnimation) {
        tetris.movePiece(Point {-1, 0});
      }
    }
    if (rightTrigger.test()) {
      if (!inAnimation) {
        tetris.movePiece(Point{1, 0});
      }
    }
    if (downTrigger.test()) {
      if (!inAnimation && !dropping) {
        auto result = tetris.movePiece(Point {0, 1});
        processMoveResult(result);
      }
    }

    if (!inAnimation) {
      autoDropTimer.update();
    }
  }

  auto draw() -> void override {
    // Fill in the game
    for (int row = 0; row < ROW_COUNT; ++row) {
      for (int col = 0; col < GAME_COLUMN_COUNT; ++col) {
        auto color = mapToColor(tetris.board[row][col]);
        grid.set(col + 3, row, color);
      }
    }

    // Fill in borders
    auto color = mapToColor(tetris.nextPiece->type);
    for (int row = 0; row < ROW_COUNT; ++row) {
      for (int col = 0; col < 3; ++col) {
        grid.set(col, row, color);
      }
      for (int col = 13; col < 16; ++col) {
        grid.set(col, row, color);
      }
    }

    sevenSegDisplay.clear();
    sevenSegDisplay.writeNumber(tetris.getScore().points, 0, SevenSegmentDigit::_0);
  }

  auto postDraw() -> void override {

  }

private:

  auto newGame() -> void {
    gameStartAt = Clock::Now();
    gameEndAt = Timestamp{};
    tetris.newGame();
    autoDropTimer.reset();
    dropTimerHandle.cancel();
    rainbowing = false;
  }

  auto processMoveResult(MoveResult& result) -> void {
    autoDropTimer.reset();
    if (result.nextPiece) {
      if (result.rowsCleared > 0) {
        runRowClearAnimation(result);
      }
      else {
        tetris.advanceNextPiece();
        if (tetris.isGameOver()) {
          gameEndAt = Clock::Now();
          runGameOverAnimation();
        }
      }
    }
  }

  auto runRowClearAnimation(MoveResult& result) -> void {
    INFO("Run Row Clear Animation");
    inAnimation = true;
    Timer::SetImmediateTimeout(
      [&, result, frame = 0](TimerContext& context) mutable {
        if (frame == tetris.columnCount()) {
          tetris.clearRows(result.rowNumbers, result.rowsCleared);
          tetris.advanceNextPiece();
          autoDropTimer.reset();
          inAnimation = false;
          INFO("Row Clear Animation Done");
          if (result.rowsCleared == 4) {
            rainbowing = true;
            rainbowTimerHandle = Timer::SetTimeout(Duration::Seconds(3), [&](){ rainbowing = false; });
          }
        }
        else {
          for (int i = 0; i < result.rowsCleared; ++i) {
            int rowNum = static_cast<int>(result.rowNumbers[i]);
            tetris.board[rowNum][frame].destroying = true;
            if (frame > 0) {
              tetris.board[rowNum][frame - 1].type = PieceType::EMPTY;
            }
          }
          ++frame;
          context.repeatIn = Duration::Milliseconds(10);
        }
      }).detach();
  }

  auto runGameOverAnimation() -> void {
    TRACE("Run GameOver Animation");
    inAnimation = true;
    Timer::SetImmediateTimeout(
      [&](TimerContext& context) mutable {
        bool found = false;
        for (int row = ROW_COUNT - 1; row >= 0; --row) {
          for (int col = 0; col < GAME_COLUMN_COUNT; ++col) {
            auto& cell = tetris.board[row][col];
            if (cell.type != PieceType::EMPTY && cell.type != PieceType::GAMEOVER) {
              cell.type = PieceType::GAMEOVER;
              found = true;
              break;
            }
          }
          if (found) {
            break;
          }
        }
        if (found) {
          context.repeatIn = Duration::Milliseconds(10);
        }
        else {
          inAnimation = false;
        }
      }).detach();
  }

  auto setupGamepad() -> void {
    gamepad.buttonB.onPress([this](){
      if (dropping) {
        dropTimerHandle.cancel();
        dropping = false;
      }
      else if (!tetris.isGameOver()) {
        dropping = true;
        dropTimerHandle = Timer::SetImmediateTimeout([&](TimerContext& context){
          auto result = tetris.movePiece(Point {0, 1});
          if (result.nextPiece) {
            dropping = false;
            processMoveResult(result);
          }
          else {
            context.repeatIn = Duration::Milliseconds(5);
          }
        });
      }
    });
    gamepad.buttonA.onPress([this](){
      if (!inAnimation) {
        tetris.rotatePieceRight();
      }
    });
    gamepad.buttonY.onPress([this](){
      if (!inAnimation) {
        tetris.rotatePieceLeft();
      }
    });
    gamepad.buttonSelect.onPress([this](){
      if (!inAnimation) {
        newGame();
      }
    });
    gamepad.buttonStart.onPress([this](){
      tetris.toggleGhost();
    });
  }

  auto mapToColor(Cell cell) -> Color {
    Color color = Color::OFF();

    if (cell.destroying) {
      color += DESTROY_COLOR;
    }

    auto speed = Duration::Milliseconds(600);
    auto time = Clock::Now().mod(speed).as<float>() / speed.as<float>();

    if (rainbowing && cell.type != PieceType::EMPTY) {
      color = Color::HslToRgb(time);
      return color;
    }

    switch (cell.type) {
      case PieceType::EMPTY:
        break;
      case PieceType::GHOST:
        color = GHOST_COLOR;
        break;
      case PieceType::GAMEOVER:
        color = GAME_OVER_HIGH_COLOR.lerpWrap(GAME_OVER_LOW_COLOR, time);
        break;
      default:
        color = mapToColor(cell.type);
    }
    return color;
  }

  static auto mapToColor(PieceType type) -> Color {
    Color color = Color::OFF();
    switch (type) {
      case PieceType::O:
        color += Color::YELLOW() * .02f;
        break;
      case PieceType::J:
        color += Color::BLUE() * .02f;
        break;
      case PieceType::L:
        color += Color::ORANGE() * .02f;
        break;
      case PieceType::T:
        color += Color::CYAN() * .02f;
        break;
      case PieceType::Z:
        color += Color::GREEN() * .02f;
        break;
      case PieceType::S:
        color += Color::MAGENTA() * .02f;
        break;
      case PieceType::I:
        color += Color::RED() * .02f;
        break;
      default:
        ASSERT(false, "Unhandled PieceType");
    }
    return color;
  }

  Timestamp gameStartAt{};
  Timestamp gameEndAt{};
  Tetris tetris{};
  TimerHandle rowClearAnimationHandle{};
  TimerHandle dropTimerHandle{};
  TimerHandle rainbowTimerHandle{};
  Trigger leftTrigger{[](){
    return gamepad.analogX <= .03f;
  }};
  Trigger rightTrigger{[](){
    return gamepad.analogX >= .97f;
  }};
  Trigger downTrigger{[](){
    return gamepad.analogY <= .03f;
  }};
  Every autoDropTimer = Every{Duration::Seconds(1), [this]() {
    auto result = tetris.movePiece({0, 1});
    processMoveResult(result);
  }};
  bool inAnimation{false};
  bool dropping{false};
  bool rainbowing{false};
};


#endif //TETRISGAME_TETRISAPPLICATION_H
