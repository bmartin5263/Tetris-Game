#include <Arduino.h>
#include "AppBuilder.h"
#include "App.h"
#include "LEDMatrix.h"
#include "GameScene.h"
#include "IRReceiver.h"
#include "PixelSlice.h"
#include "AdafruitI2CGamepad.h"
#include "Config.h"

using namespace rgb;

auto irReceiver = IRReceiver{};
auto gamepad = AdafruitI2CGamepad{};
auto sensors = std::array {
  Runnable { []() {
    irReceiver.update();
  }},
  Runnable { []() {
    gamepad.update();
  }}
};

auto grid = LEDMatrix<COLUMN_COUNT, ROW_COUNT>(D2_RGB);
auto debugLeds = grid.slice(8);
auto leds = std::array {
  static_cast<LEDCircuit*>(&grid)
};

auto gameScene = GameScene{grid, irReceiver, gamepad};
auto scenes = std::array {
  static_cast<Scene*>(&gameScene)
};

void setup() {
  gamepad.start();
  irReceiver.start(D3);
  DebugScreen::Start(FlipDisplay{true});
  AppBuilder::Create()
      .SetScenes(scenes)
      .SetLEDs(leds)
      .SetSensors(sensors)
      .DebugOutputLED(&debugLeds)
      .Start();
}

void loop() {
  if (DebugScreen::ReadyForUpdate()) {
    DebugScreen::Display();
  }
  App::Loop();
}