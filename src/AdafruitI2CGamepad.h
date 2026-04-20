//
// Created by Brandon on 6/12/25.
//

#ifndef RGBLIB_ADAFRUITI2CGAMEPAD_H
#define RGBLIB_ADAFRUITI2CGAMEPAD_H

#include "Types.h"
#include "Func.h"
#include "Sensor.h"
#include "GamePadButton.h"
#include "MyAdafruitSeesaw.h"

class AdafruitI2CGamepad : public rgb::Sensor {
  constexpr static auto doNothing() -> void {}
public:
  auto doStart() -> bool override;
  auto doRead() -> void override;

  Adafruit_seesaw seesaw;
  GamePadButton buttonX{};
  GamePadButton buttonY{};
  GamePadButton buttonA{};
  GamePadButton buttonB{};
  GamePadButton buttonStart{};
  GamePadButton buttonSelect{};
  rgb::normal analogX{};
  rgb::normal analogY{};
};


#endif //RGBLIB_ADAFRUITI2CGAMEPAD_H
