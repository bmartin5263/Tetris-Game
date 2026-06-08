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
#include "BotController.h"
#include "ColorfulPalette.h"
#include "RainbowPalette.h"
#include "SolidPalette.h"
#include "Carousel.h"

using namespace rgb;

inline auto sevenSegDisplay = MAX7219EightDigitSevenSegmentDisplay{D5};
inline auto gamepad = AdafruitI2CGamepad{};
inline auto botController = BotController{};
inline auto grid = FastLEDMatrix<8, 8, D2_RGB, RgbwSupport::ENABLE, 3, 2>();
inline auto colorfulPalette = ColorfulPalette{};
inline auto rainbowPalette = RainbowPalette{};
inline auto whitePalette = SolidPalette{Color::WHITE() * .02f};
inline auto redPalette = SolidPalette{Color::RED() * .02f};
inline auto orangePalette = SolidPalette{Color::ORANGE() * .02f};
inline auto yellowPalette = SolidPalette{Color::YELLOW() * .02f};
inline auto greenPalette = SolidPalette{Color::GREEN() * .02f};
inline auto cyanPalette = SolidPalette{Color::CYAN() * .02f};
inline auto bluePalette = SolidPalette{Color::BLUE() * .02f};
inline auto purplePalette = SolidPalette{Color::MAGENTA() * .02f};

class TetrisApplication : public UserApplication<> {
  constexpr static auto LEFT = Point{-1, 0};
  constexpr static auto RIGHT = Point{1, 0};
  constexpr static auto DOWN = Point{0, 1};

protected:
  auto configure(Configurer& app) -> void override {
    grid.setBrightness(1.0f);
    app.addLEDs(grid);
    app.addSensor(gamepad);
    app.addSensor(botController);

    SPI.begin();
    sevenSegDisplay.start();

    setupGamepad();
    newGame();
  }

  auto update() -> void override {
    if (inAnimation) {
      return;
    }
    if (botController.hasChar()) {
      processBotCommand(botController.takeChar());
    }

    if (leftTrigger.test()) {
      INFO("Gamepad Left");
      tetris.movePiece(LEFT);
    }
    if (rightTrigger.test()) {
      INFO("Gamepad Right");
      tetris.movePiece(RIGHT);
    }
    if (downTrigger.test()) {
      INFO("Gamepad Down");
      if (!dropping) {
        auto result = tetris.movePiece(DOWN);
        processMoveResult(result);
      }
    }

    if (every(Duration::Seconds(1), autoDropTimer)) {
      INFO("Auto Drop");
      auto result = tetris.movePiece(DOWN);
      if (result.nextPiece) {
        INFO("Auto Next Piece");
      }
      processMoveResult(result);
    }
  }

  auto draw() -> void override {
    auto* palette = palettes.get();

    // Fill in the game
    for (int row = 0; row < ROW_COUNT; ++row) {
      for (int col = 0; col < GAME_COLUMN_COUNT; ++col) {
        auto color = palette->mapToColor(tetris.board[row][col]);
        grid.set(col + 3, row, color);
      }
    }

    // Fill in borders
    constexpr auto FADE_DURATION = Duration::Milliseconds(150);
    auto elapsed = Clock::Now().timeSince(nextTetrominoAt);
    auto t = static_cast<float>(elapsed.value) / static_cast<float>(FADE_DURATION.value);
    auto color = palette->mapToColor(tetris.currentPiece->type).lerpClamp(palette->mapToColor(tetris.nextPiece->type), t);

    for (int row = 0; row < ROW_COUNT; ++row) {
      for (int col = 0; col < 3; ++col) {
        grid.set(col, row, color);
      }
      for (int col = 13; col < 16; ++col) {
        grid.set(col, row, color);
      }
    }

    sevenSegDisplay.clear();
    sevenSegDisplay.writeNumber(static_cast<int>(tetris.getScore().points), SevenSegmentDigit::_0);
  }

  auto postDraw() -> void override {

  }

private:

  auto newGame() -> void {
    tetris.newGame();
    autoDropTimer = Clock::Now();
    dropTimerHandle.cancel();
  }

  auto processBotCommand(char c) -> void {
    INFO("Processing Bot Command %c", c);
    switch (c) {
      case 'I':
      case 'i':
        INFO("Bot Rotate Right");
        tetris.rotatePieceRight();
        break;
      case 'E':
      case 'e':
        INFO("Bot Rotate Left");
        tetris.rotatePieceLeft();
        break;
      case 'D':
      case 'd':
        INFO("Bot Down");
        if (!dropping) {
          auto result = tetris.movePiece(DOWN);
          processMoveResult(result);
        }
        break;
      case 'R':
      case 'r':
        INFO("Bot Right");
        tetris.movePiece(RIGHT);
        break;
      case 'L':
      case 'l':
        INFO("Bot Left");
        tetris.movePiece(LEFT);
        break;
      case 'N':
      case 'n':
        INFO("Bot New Game");
        tetris.newGame();
        break;
      case 'G':
      case 'g':
        INFO("Bot New Game (full reset)");
        newGame();
        if (tetris.isGhostEnabled()) {
          tetris.toggleGhost();
        }
        break;
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9': {
        auto index = static_cast<size_t>(c - '0');
        INFO("Bot Select Palette %zu", index);
        palettes.set(index);
        break;
      }
      default:
        break;
    }
  }

  auto processMoveResult(MoveResult& result) -> void {
    autoDropTimer = Clock::Now();
    if (result.nextPiece) {
      dropTimerHandle.cancel();
      if (result.rowsCleared > 0) {
        runRowClearAnimation(result);
      }
      else {
        INFO("Advancing to Next Piece");
        tetris.advanceNextPiece();
        nextTetrominoAt = Clock::Now();
        if (tetris.isGameOver()) {
          runGameOverAnimation();
        }
      }
    }
  }

  auto runRowClearAnimation(MoveResult& result) -> void {
    INFO("Started Row Clear Animation");
    inAnimation = true;
    Timer::SetImmediateTimeout(
      [&, result, column = 0](TimerContext& context) mutable {
        if (column == Tetris::COLUMNS) {
          tetris.clearRows(result.rowNumbers, result.rowsCleared);
          tetris.advanceNextPiece();
          nextTetrominoAt = Clock::Now();
          autoDropTimer = Clock::Now();
          inAnimation = false;
          INFO("Finished Row Clear Animation, Advancing to Next Piece");
        }
        else {
          for (int i = 0; i < result.rowsCleared; ++i) {
            int rowNum = static_cast<int>(result.rowNumbers[i]);
            tetris.board[rowNum][column].destroying = true;
          }
          ++column;
          context.repeatIn = Duration::Milliseconds(10);
        }
      }).detach();
  }

  auto runGameOverAnimation() -> void {
    INFO("Started Game Over Animation");
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
          INFO("Game Over");
          inAnimation = false;
        }
      }).detach();
  }

  auto setupGamepad() -> void {
    gamepad.buttonB.onPress([this](){
      INFO("Gamepad Drop");
      if (dropping) {
        dropTimerHandle.cancel();
        dropping = false;
      }
      else if (!tetris.isGameOver()) {
        dropping = true;
        dropTimerHandle = Timer::SetImmediateTimeout([&](TimerContext& context){
          auto result = tetris.movePiece(DOWN);
          if (result.nextPiece) {
            dropping = false;
            INFO("Drop Finished");
            processMoveResult(result);
          }
          else {
            context.repeatIn = Duration::Milliseconds(5);
          }
        });
      }
    });
    gamepad.buttonA.onPress([this](){
      INFO("Gamepad Rotate Right");
      if (!inAnimation) {
        tetris.rotatePieceRight();
      }
    });
    gamepad.buttonY.onPress([this](){
      INFO("Gamepad Rotate Left");
      if (!inAnimation) {
        tetris.rotatePieceLeft();
      }
    });
    gamepad.buttonX.onPress([this](){
      INFO("Gamepad Cycle Palette");
      palettes.next();
    });
    gamepad.buttonSelect.onPress([this](){
      INFO("Gamepad New Game");
      if (!inAnimation) {
        newGame();
      }
    });
    gamepad.buttonStart.onPress([this](){
      INFO("Gamepad Toggle Ghost");
      if (!inAnimation) {
        tetris.toggleGhost();
      }
    });
  }

  Tetris tetris{};
  Timestamp nextTetrominoAt{};
  TimerHandle rowClearAnimationHandle{};
  TimerHandle dropTimerHandle{};
  Carousel<const ColorPalette*, 10> palettes{std::array<const ColorPalette*, 10>{
    &colorfulPalette,
    &rainbowPalette,
    &whitePalette,
    &redPalette,
    &orangePalette,
    &yellowPalette,
    &greenPalette,
    &cyanPalette,
    &bluePalette,
    &purplePalette,
  }};
  Trigger leftTrigger{[](){
    return gamepad.analogX <= .03f;
  }};
  Trigger rightTrigger{[](){
    return gamepad.analogX >= .97f;
  }};
  Trigger downTrigger{[](){
    return gamepad.analogY <= .03f;
  }};
  Timestamp autoDropTimer{};
  bool inAnimation{false};
  bool dropping{false};
};


#endif //TETRISGAME_TETRISAPPLICATION_H
