#include <Arduino.h>
#include "AppBuilder.h"
#include "App.h"
#include "LEDCircuit.h"
#include "GameScene.h"
#include "IRReceiver.h"

#include <chrono>

using namespace rgb;

auto irReceiver = IRReceiver{D3};
auto sensors = std::array {
  Runnable { []() {
    irReceiver.update();
  }},
};

auto grid = LEDCircuit<64>(D2_RGB);
auto debugLeds = grid.slice(8);
auto leds = std::array {
  static_cast<Drawable*>(&grid)
};

auto gameScene = GameScene{grid, irReceiver};
auto scenes = std::array {
  static_cast<Scene*>(&gameScene)
};

void setup() {
  irReceiver.start();
  AppBuilder::Create()
      .EnableIntroScene(gameScene, Duration::Seconds(1))
      .SetScenes(scenes)
      .SetLEDs(leds)
      .SetSensors(sensors)
      .DebugOutputLED(&debugLeds)
      .Start();
}

void loop() {
  App::Loop();
}