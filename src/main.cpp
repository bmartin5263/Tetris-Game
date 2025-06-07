#include <Arduino.h>
#include "AppBuilder.h"
#include "App.h"
#include "LEDMatrix.h"
#include "GameScene.h"
#include "IRReceiver.h"

using namespace rgb;

auto irReceiver = IRReceiver{};
auto sensors = std::array {
  Runnable { []() {
    irReceiver.update();
  }},
};

auto grid = LEDMatrix<8, 8>(D2_RGB);
auto debugLeds = grid.slice(8);
auto leds = std::array {
  static_cast<LEDList*>(&grid)
};

auto gameScene = GameScene{grid, irReceiver};
auto scenes = std::array {
  static_cast<Scene*>(&gameScene)
};

void setup() {
  irReceiver.start(D3);
  AppBuilder::Create()
      .SetScenes(scenes)
      .SetLEDs(leds)
      .SetSensors(sensors)
      .DebugOutputLED(&debugLeds)
      .Start();
}

void loop() {
  App::Loop();
}