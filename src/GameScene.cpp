//
// Created by Brandon on 6/1/25.
//

#include "GameScene.h"
#include "IRReceiver.h"
#include "DebugScreen.h"

using namespace rgb;

#define TIMER_FUNCTION [](TimerResult& options)

GameScene::GameScene(PixelGrid& grid, IRReceiver& irReceiver, AdafruitI2CGamepad& gamepad):
  grid{grid}, irReceiver{irReceiver}, gamepad{gamepad} {
}

auto GameScene::setup() -> void {
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

  gamepad.buttonB.onPress([this](){
    if (!inAnimation) {
      inAnimation = true;
      Timer::SetImmediateTimeout([&](auto& context){
        auto result = tetris.movePiece(Point {0, 1});
        if (result.nextPiece) {
          inAnimation = false;
          processMoveResult(result);
        }
        else {
          context.repeatIn = Duration::Milliseconds(5);
        }
      }).detach();
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
    if (!inAnimation) {
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
  tetris.draw(grid);

  DebugScreen::PrintLine(0, "Points: " + to_string(tetris.getScore().points));
  DebugScreen::PrintLine(1, "Rows: " + to_string(tetris.getScore().clearedRows));
  DebugScreen::PrintLine(4, "FPS: " + std::to_string(Clock::Fps()));
}

auto GameScene::cleanup() -> void {

}

auto GameScene::newGame() -> void {
  tetris.newGame();
  autoDropTimer.reset();
}

auto GameScene::processMoveResult(MoveResult& result) -> void {
  autoDropTimer.reset();
  if (result.nextPiece) {
    if (result.rowsCleared > 0) {
      runRowClearAnimation(result);
    }
    else {
      tetris.nextPiece();
    }
  }
}

//constexpr auto DESTROY_COLOR = Color(.8, .0, .2) * .03f;
constexpr auto DESTROY_COLOR = Color(1.0f, 1.0f, 1.0f) * .03f;

//auto GameScene::runRowClearAnimation(MoveResult& result) -> void {
//  INFO("Run Row Clear Animation");
//  inAnimation = true;
//  Timer::SetImmediateTimeout(
//    [&, result, frame = 0](TimerContext& context) mutable {
//      if (frame == tetris.columnCount() / 2) {
//        Timer::ContinuouslyFor(Duration::Milliseconds(200), [&, result](TimerContext& context) {
//          auto deathColor = DESTROY_COLOR.lerp(Color::OFF(), context.percentComplete);
//          for (int i = 0; i < result.rowsCleared; ++i) {
//            auto rowNum = result.rowNumbers[i];
//            for (int c = 0; c < tetris.columnCount(); ++c) {
//              tetris.board[rowNum][c] = deathColor;
//              tetris.board[rowNum][c] = deathColor;
//            }
//          }
//
//          if (context.percentComplete >= 1.0f) {
//            tetris.clearRows(result.rowNumbers, result.rowsCleared);
//            tetris.nextPiece();
//            autoDropTimer.reset();
//            inAnimation = false;
//            INFO("Row Clear Animation Done");
//          }
//        }).detach();
//      }
//      else {
//        auto right = tetris.columnCount() / 2;
//        auto left= right - 1;
//        for (int i = 0; i < result.rowsCleared; ++i) {
//          auto rowNum = result.rowNumbers[i];
//          tetris.board[rowNum][left - frame] = DESTROY_COLOR;
//          tetris.board[rowNum][right + frame] = DESTROY_COLOR;
//        }
//        ++frame;
//        context.repeatIn = Duration::Milliseconds(60);
//      }
//    }).detach();
//}

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
      }
      else {
        for (int i = 0; i < result.rowsCleared; ++i) {
          auto rowNum = result.rowNumbers[i];
          tetris.board[rowNum][frame].w = .05f;
          if (frame > 0) {
            tetris.board[rowNum][frame - 1] = DESTROY_COLOR;
          }
        }
        ++frame;
        context.repeatIn = Duration::Milliseconds(10);
      }
    }).detach();
}
