//
// Created by Brandon on 4/30/26.
//

#ifndef TETRISGAME_COLORPALETTE_H
#define TETRISGAME_COLORPALETTE_H

#include "RgbColor.h"
#include "PieceType.h"
#include "Tetris.h"

class ColorPalette {
public:
  ColorPalette() = default;
  virtual ~ColorPalette() = default;
  ColorPalette(const ColorPalette&) = default;
  ColorPalette(ColorPalette&&) = default;
  auto operator=(const ColorPalette&) -> ColorPalette& = default;
  auto operator=(ColorPalette&&) -> ColorPalette& = default;

  virtual auto mapToColor(Cell cell) const -> rgb::Color = 0;
  virtual auto mapToColor(PieceType type) const -> rgb::Color = 0;
};

#endif //TETRISGAME_COLORPALETTE_H
