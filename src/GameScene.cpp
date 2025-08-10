//
// Created by Brandon on 6/1/25.
//

#include "GameScene.h"
#include "IRReceiver.h"
#include "DebugScreen.h"
#include "Wireless.h"
#include "Assertions.h"

using namespace rgb;

GameScene::GameScene(PixelGrid& grid, IRReceiver& irReceiver, AdafruitI2CGamepad& gamepad):
  grid{grid}, irReceiver{irReceiver}, gamepad{gamepad} {
}

auto GameScene::setup() -> void {
  setupIRReceiver();
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
  auto middleCol = COLUMN_COUNT / 2;
  auto middleRow = ROW_COUNT / 2;
//  for (int gameRow = 0; gameRow < ROW_COUNT; ++gameRow) {
//    for (int gameCol = 0; gameCol < COLUMN_COUNT; ++gameCol) {
//      auto color = mapToColor(tetris.board[gameRow][gameCol]);
//      grid[{gameCol, gameRow}] = color;
//    }
//    for (int gameCol = middleCol; gameCol < COLUMN_COUNT; ++gameCol) {
//      auto color = mapToColor(tetris.board[gameRow][gameCol]);
//
//      int physicalRow = middleRow + gameRow;
//      int physicalCol = gameCol - COLUMN_COUNT;
//
//      grid[{physicalRow, physicalCol}] = color;
//    }
//  }
//  grid[{0, 0}] = Color::BLUE(.01f);
//  grid[{ROW_COUNT - 1, COLUMN_COUNT - 1}] = Color::RED(.01f);

  for (int row = 0; row < ROW_COUNT; ++row) {
    for (int col = 0; col < COLUMN_COUNT; ++col) {
      auto color = mapToColor(tetris.board[row][col]);
      if (col < middleCol) {
        u16 position = (middleCol * row) + col;
        ASSERT(position < 128, "position too big");
        static_cast<PixelList&>(grid)[position] = color;
      }
      else {
        // Row = 0
        // Col = 8
        // Exp = 128
        // 32 rows, 8 columns, we want to be in row 16, column 0

        // Row = 0
        // Col = 9
        // Exp = 129
        // 32 rows, 8 columns, we want to be in row 16, column 1

        u16 position = ((row + ROW_COUNT) * middleCol) + (col - middleCol);
        ASSERT(position >= 128, "position too small");
        static_cast<PixelList&>(grid)[position] = color;
      }

//      auto v = static_cast<float>((row * ROW_COUNT) + col);
//      auto h = v / (ROW_COUNT * COLUMN_COUNT);
//      auto c = Color::HslToRgb(h);
//      grid[Point{col, row}] = c * .03f;
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

  DebugScreen::PrintLine(0, "Points: " + to_string(tetris.getScore().points));
  DebugScreen::PrintLine(1, "Rows: " + to_string(tetris.getScore().clearedRows));
  DebugScreen::PrintLine(2, "Time: " + minutesStr + ":" + secondsStr);
  DebugScreen::PrintLine(3, std::string("Dropping: ") + (dropping ? "true" : "false"));
  DebugScreen::PrintLine(4, "FPS: " + to_string(Clock::Fps()));
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
        for (int col = 0; col < COLUMN_COUNT; ++col) {
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

auto GameScene::setupIRReceiver() -> void {
  irReceiver.buttonLeft.onPress([this](){
    if (!inAnimation) {
      tetris.movePiece(Point {-1, 0});
    }
  });

  irReceiver.buttonRight.onPress([this](){
    if (!inAnimation) {
      tetris.movePiece(Point{1, 0});
    }
  });

  irReceiver.buttonDown.onPress([this](){
    if (!inAnimation) {
      auto result = tetris.movePiece(Point {0, 1});
      processMoveResult(result);
    }
  });

  irReceiver.buttonOk.onPress([this](){
    if (!inAnimation) {
      tetris.rotatePieceRight();
    }
  });

  irReceiver.buttonStar.onPress([this](){
    newGame();
  });

  irReceiver.button0.onPress([this](){
    if (!inAnimation) {
      auto result = tetris.dropPiece();
      processMoveResult(result);
    }
  });
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
