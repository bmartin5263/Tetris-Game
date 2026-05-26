//
// Created by Brandon on 4/30/26.
//

#ifndef TETRISGAME_COLORFULPALETTE_H
#define TETRISGAME_COLORFULPALETTE_H

#include "ColorPalette.h"

class ColorfulPalette : public ColorPalette {
public:
  ColorfulPalette() = default;
  ~ColorfulPalette() override = default;
  ColorfulPalette(const ColorfulPalette&) = default;
  ColorfulPalette(ColorfulPalette&&) = default;
  auto operator=(const ColorfulPalette&) -> ColorfulPalette& = default;
  auto operator=(ColorfulPalette&&) -> ColorfulPalette& = default;

  auto mapToColor(Cell cell) const -> rgb::Color override;
  auto mapToColor(PieceType type) const -> rgb::Color override;
};

#endif //TETRISGAME_COLORFULPALETTE_H
