//
// Created by Brandon on 6/1/25.
//

#include "GameScene.h"
#include "IRReceiver.h"
#include "DebugScreen.h"

using namespace rgb;

#define TIMER_FUNCTION [](TimerResult& options)

GameScene::GameScene(PixelList& grid, IRReceiver& irReceiver): grid{grid}, irReceiver{irReceiver} {
}

auto GameScene::setup() -> void {
  irReceiver.buttonLeft.onPress([this](){
    if (!inAnimation) {
      tetris.moveTetromino(Point {-1, 0});
    }
  });

  irReceiver.buttonRight.onPress([this](){
    if (!inAnimation) {
      tetris.moveTetromino(Point{1, 0});
    }
  });

  irReceiver.buttonDown.onPress([this](){
    if (!inAnimation) {
      auto result = tetris.moveTetromino(Point {0, 1});
      processMoveResult(result);
    }
  });

  irReceiver.buttonOk.onPress([this](){
    if (!inAnimation) {
      tetris.rotateTetrominoRight();
    }
  });

  irReceiver.buttonStar.onPress([this](){
    newGame();
  });

  irReceiver.button0.onPress([this](){
    if (!inAnimation) {
      auto result = tetris.dropTetromino();
      processMoveResult(result);
    }
  });

  INFO("NEW GAME");
  newGame();
}

auto GameScene::update() -> void {
  if (!inAnimation) {
//    autoDropTimer.update();
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
  if (result.nextTetromino) {
    if (result.rowsCleared > 0) {
      runRowClearAnimation(result);
    }
    else {
      tetris.nextTetromino();
    }
  }
}

constexpr auto DESTROY_COLOR = Color(.8, .0, .2) * .03f;

auto GameScene::runRowClearAnimation(MoveResult& result) -> void {
  INFO("Run Row Clear Animation");
  auto x = std::make_unique<int>();
  inAnimation = true;
  rowClearAnimationHandle = Timer::SetTimeout(
    Duration::Milliseconds(0),
    [&, result, frame = 0, phase = 1](TimerOptions& options) mutable {
      INFO("Animation Frame");
      if (phase == 0) {
        if (frame == tetris.columnCount() / 2) {
          phase = 1;
          options.repeatIn = Duration::Microseconds(1);
        }
        else {
          auto right = tetris.columnCount() / 2;
          auto left= right - 1;
          for (int i = 0; i < result.rowsCleared; ++i) {
            auto rowNum = result.rowNumbers[i];
            tetris.board[rowNum][left - frame] = DESTROY_COLOR;
            tetris.board[rowNum][right + frame] = DESTROY_COLOR;
          }
          ++frame;
          options.repeatIn = Duration::Milliseconds(80);
        }
      }
//      else {
//        constexpr static auto FADE_TIME = rgb::Duration::Milliseconds(300);
//        auto time = (rgb::Clock::Now() % FADE_TIME).to<float>();
//        auto amount = rgb::LerpWrap(0.0f, 1.0f, time / FADE_TIME.to<float>());
//        auto deathColor = DESTROY_COLOR.lerp(Color::OFF(), amount);
//        INFO("death color = %f, %f, %f, %f", deathColor.r, deathColor.g, deathColor.b, deathColor.w);
//        for (int i = 0; i < result.rowsCleared; ++i) {
//          auto rowNum = result.rowNumbers[i];
//          for (int c = 0; c < tetris.columnCount(); ++c) {
//            tetris.board[rowNum][c] = deathColor;
//            tetris.board[rowNum][c] = deathColor;
//          }
//        }
//
//        if (deathColor.r < 0.003f) {
//          tetris.clearRows(result.rowNumbers, result.rowsCleared);
//          tetris.nextTetromino();
//          autoDropTimer.reset();
//          inAnimation = false;
//          INFO("Row Clear Animation Done");
//        }
//        else {
//          options.repeatIn = Duration::Microseconds(1);
//        }
//      }
      else {
        constexpr static auto FADE_TIME = rgb::Duration::Milliseconds(50);
        auto flash = DESTROY_COLOR;
        flash.w = .35f;
        auto time = (rgb::Clock::Now() % FADE_TIME).to<float>();
        auto amount = rgb::LerpWrap(0.0f, 1.0f, time / FADE_TIME.to<float>());
        auto deathColor = DESTROY_COLOR.lerp(flash, amount);
        INFO("death color = %f, %f, %f, %f", deathColor.r, deathColor.g, deathColor.b, deathColor.w);
        for (int i = 0; i < result.rowsCleared; ++i) {
          auto rowNum = result.rowNumbers[i];
          for (int c = 0; c < tetris.columnCount(); ++c) {
            tetris.board[rowNum][c] = deathColor;
            tetris.board[rowNum][c] = deathColor;
          }
        }

        if (deathColor.w >= .3f) {
          tetris.clearRows(result.rowNumbers, result.rowsCleared);
          tetris.nextTetromino();
          autoDropTimer.reset();
          inAnimation = false;
          INFO("Row Clear Animation Done");
        }
        else {
          options.repeatIn = Duration::Microseconds(1);
        }
      }
    });
}
