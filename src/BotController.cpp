//
// Created by Brandon on 4/26/26.
//

#include "BotController.h"
#include "Log.h"
#include <Arduino.h>

auto BotController::doStart() -> bool {
  INFO("Starting BotController");
  Serial1.begin(115200, SERIAL_8N1, A0, A1);
  return true;
}

auto BotController::doRead() -> void {
  if (Serial1.available() > 0) {
    char_ = static_cast<char>(Serial1.read());
    hasChar_ = true;
    INFO("Bot sent %c", char_);
  }
}
