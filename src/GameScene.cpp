//
// Created by Brandon on 6/1/25.
//

#include "GameScene.h"
#include "DebugScreen.h"
#include "Wireless.h"
#include "Assertions.h"
#include "SevenSegmentDisplay.h"
#include "AdafruitI2CGamepad.h"

using namespace rgb;

GameScene::GameScene(
  PixelGrid& grid,
  AdafruitI2CGamepad& gamepad,
  rgb::MAX7219EightDigitSevenSegmentDisplay& sevenSegDisplay
):
  grid{grid}, gamepad{gamepad}, sevenSegDisplay(sevenSegDisplay),
  leftTrigger{[&](){
    return gamepad.analogX <= .03f;
  }},
  rightTrigger{[&](){
    return gamepad.analogX >= .97f;
  }},
  downTrigger{[&](){
    return gamepad.analogY <= .03f;
  }}
  {
}

auto GameScene::setup() -> void {
  setupGamepad();
  INFO("NEW GAME");
  newGame();
}

auto GameScene::update() -> void {
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

auto GameScene::draw() -> void {
  using std::to_string;
  auto middleCol = DISPLAY_COLUMN_COUNT / 2;

  for (int row = 0; row < ROW_COUNT; ++row) {
    for (int col = 0; col < GAME_COLUMN_COUNT; ++col) {
      auto color = mapToColor(tetris.board[row][col]);
      auto dCol = col + 3;
      if (dCol < middleCol) {
        u16 position = (middleCol * row) + dCol;
//        ASSERT(position < ROW_COUNT * COLUMN_COUNT / 2, "position too big");
        static_cast<PixelList&>(grid)[position] = color;
      }
      else {
        u16 position = ((row + ROW_COUNT) * middleCol) + (dCol - middleCol);
//        ASSERT(position >= 128, "position too small");
        static_cast<PixelList&>(grid)[position] = color;
      }
    }
  }

  auto color = mapToColor(tetris.currentPiece->type);
  for (int row = 0; row < ROW_COUNT; ++row) {
    for (int col = 0; col < 3; ++col) {
      u16 position = (middleCol * row) + col;
      static_cast<PixelList&>(grid)[position] = color;
    }
    for (int col = 13; col < 16; ++col) {
      u16 position = ((row + ROW_COUNT) * middleCol) + (col - middleCol);
      static_cast<PixelList&>(grid)[position] = color;
    }
  }

  auto now = Clock::Now();
  long long time;
  if (!gameEndAt.isZero()) {
    time = (gameEndAt - gameStartAt).asSeconds();
  }
  else {
    time = (now - gameStartAt).asSeconds();
  }

  auto minutes = time / 60;
  auto seconds = time % 60;

  auto minutesStr = minutes >= 10 ? to_string(minutes) : "0" + to_string(minutes);
  auto secondsStr = seconds >= 10 ? to_string(seconds) : "0" + to_string(seconds);

//  DebugScreen::PrintLine(0, "Points: " + to_string(tetris.getScore().points));
//  DebugScreen::PrintLine(1, "Rows: " + to_string(tetris.getScore().clearedRows));
//  DebugScreen::PrintLine(2, "Time: " + minutesStr + ":" + secondsStr);
//  DebugScreen::PrintLine(3, std::string("Dropping: ") + (dropping ? "true" : "false"));
//  DebugScreen::PrintLine(4, "FPS: " + to_string(Clock::Fps()));

  sevenSegDisplay.clear();
  sevenSegDisplay.writeNumber(tetris.getScore().points, 0, SevenSegmentDigit::_0);
}

auto GameScene::cleanup() -> void {

}

auto GameScene::newGame() -> void {
  gameStartAt = Clock::Now();
  gameEndAt = Timestamp{};
  tetris.newGame();
  autoDropTimer.reset();
  dropTimerHandle.cancel();
  rainbowing = false;
}

auto GameScene::processMoveResult(MoveResult& result) -> void {
  autoDropTimer.reset();
  if (result.nextPiece) {
    if (result.rowsCleared > 0) {
      runRowClearAnimation(result);
    }
    else {
      tetris.nextPiece();
      if (tetris.isGameOver()) {
        gameEndAt = Clock::Now();
        runGameOverAnimation();
      }
    }
  }
}

auto GameScene::runRowClearAnimation(MoveResult& result) -> void {
  INFO("Run Row Clear Animation");
  inAnimation = true;
  Timer::SetImmediateTimeout(
    [&, result, frame = 0](TimerContext& context) mutable {
      if (frame == tetris.columnCount()) {
        tetris.clearRows(result.rowNumbers, result.rowsCleared);
        tetris.nextPiece();
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

auto GameScene::runGameOverAnimation() -> void {
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

auto GameScene::setupGamepad() -> void {
  gamepad.buttonB.onPress([this](){
    if (dropping) {
      dropTimerHandle.cancel();
      dropping = false;
    }
    else if (!tetris.isGameOver()) {
      dropping = true;
      dropTimerHandle = Timer::SetImmediateTimeout([&](auto& context){
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
