//
// Created by Brandon on 6/1/25.
//

#include "GameScene.h"
#include "IRReceiver.h"

using namespace rgb;

GameScene::GameScene(rgb::LEDChain& grid, IRReceiver& irReceiver): grid(grid), irReceiver(irReceiver) {

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
}

auto GameScene::update() -> void {
  tetris.update();
}

auto GameScene::draw() -> void {
  tetris.draw(grid);
}

auto GameScene::cleanup() -> void {

}