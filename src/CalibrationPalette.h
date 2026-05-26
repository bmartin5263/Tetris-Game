//
// Created by Brandon on 5/26/26.
//

#ifndef TETRISGAME_CALIBRATIONPALETTE_H
#define TETRISGAME_CALIBRATIONPALETTE_H

#include "ColorPalette.h"

class CalibrationPalette : public ColorPalette {
public:
  CalibrationPalette() = default;
  ~CalibrationPalette() override = default;
  CalibrationPalette(const CalibrationPalette&) = default;
  CalibrationPalette(CalibrationPalette&&) = default;
  auto operator=(const CalibrationPalette&) -> CalibrationPalette& = default;
  auto operator=(CalibrationPalette&&) -> CalibrationPalette& = default;

  auto mapToColor(Cell cell) const -> rgb::Color override;
  auto mapToColor(PieceType type) const -> rgb::Color override;
};

#endif //TETRISGAME_CALIBRATIONPALETTE_H
