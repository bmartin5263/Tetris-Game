//
// Created by Brandon on 6/1/25.
//

#include "GameScene.h"
#include "IRReceiver.h"
#include "DebugScreen.h"

using namespace rgb;

GameScene::GameScene(PixelList& grid, IRReceiver& irReceiver): grid{grid}, irReceiver{irReceiver} {
}

auto GameScene::setup() -> void {
  irReceiver.buttonLeft.onPress([this](){
    tetris.moveTetromino(Point {-1, 0});
  });
  irReceiver.buttonRight.onPress([this](){
    tetris.moveTetromino(Point {1, 0});
  });
  irReceiver.buttonDown.onPress([this](){
    tetris.moveTetromino(Point {0, 1});
  });
  irReceiver.buttonOk.onPress([this](){
    tetris.rotateTetrominoRight();
  });
  irReceiver.buttonStar.onPress([this](){
    tetris.newGame();
  });
  irReceiver.button0.onPress([this](){
    tetris.dropTetromino();
  });
  tetris.newGame();
}

auto GameScene::update() -> void {
  tetris.update();
}

auto GameScene::draw() -> void {
  tetris.draw(grid);
  auto fpsStr = "FPS: " + std::to_string(Clock::Fps());
  auto pointsStr = "Points: " + std::to_string(tetris.getScore().points);
  auto rowsStr = "Rows: " + std::to_string(tetris.getScore().clearedRows);
  DebugScreen::PrintLine(0, pointsStr);
  DebugScreen::PrintLine(1, rowsStr);
  DebugScreen::PrintLine(4, fpsStr);
}

auto GameScene::cleanup() -> void {

}
