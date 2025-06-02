//
// Created by Brandon on 6/1/25.
//

#ifndef TETRISGAME_GAMESCENE_H
#define TETRISGAME_GAMESCENE_H

#include "Scene.h"
#include "LEDChain.h"
#include "Tetris.h"

namespace rgb {
class IRReceiver;
}

class GameScene : public rgb::Scene {
public:
  constexpr static auto SIZE = 8;

  explicit GameScene(rgb::LEDChain& grid, rgb::IRReceiver& irReceiver);

  auto update() -> void override;
  auto draw() -> void override;
  auto setup() -> void override;
  auto cleanup() -> void override;

private:
  Tetris<SIZE, SIZE> tetris;
  rgb::LEDChain& grid;
  rgb::IRReceiver& irReceiver;

};


#endif //TETRISGAME_GAMESCENE_H
