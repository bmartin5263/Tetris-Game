//
// Created by Brandon on 5/26/26.
//

#include "CalibrationPalette.h"

using namespace rgb;

namespace {
constexpr auto CALIBRATION_COLOR = Color::WHITE() * .01f;
}

auto CalibrationPalette::mapToColor(Cell cell) const -> Color {
  return CALIBRATION_COLOR;
}

auto CalibrationPalette::mapToColor(PieceType type) const -> Color {
  return CALIBRATION_COLOR;
}
