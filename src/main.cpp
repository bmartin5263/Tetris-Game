#include "TetrisApplication.h"

using namespace rgb;

auto app = TetrisApplication{};

auto setup() -> void {
  app.setup();
}

auto loop() -> void {
  app.loop();
}