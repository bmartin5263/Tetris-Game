//
// Created by Brandon on 4/26/26.
//

#ifndef TETRISGAME_BOTCONTROLLER_H
#define TETRISGAME_BOTCONTROLLER_H

#include "Sensor.h"

class BotController : public rgb::Sensor {
public:
  auto doStart() -> bool override;
  auto doRead() -> void override;

  [[nodiscard]] auto hasChar() const -> bool { return hasChar_; }
  auto takeChar() -> char {
    hasChar_ = false;
    return char_;
  }

private:
  char char_{};
  bool hasChar_{false};
};


#endif //TETRISGAME_BOTCONTROLLER_H
