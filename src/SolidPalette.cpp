//
// Created by Brandon on 5/26/26.
//

#include "SolidPalette.h"

using namespace rgb;

SolidPalette::SolidPalette(Color color) : color{color} {}

auto SolidPalette::mapToColor(Cell cell) const -> Color {
  return color;
}

auto SolidPalette::mapToColor(PieceType type) const -> Color {
  return color;
}
