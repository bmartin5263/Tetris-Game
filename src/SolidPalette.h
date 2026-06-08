//
// Created by Brandon on 5/26/26.
//

#ifndef TETRISGAME_SOLIDPALETTE_H
#define TETRISGAME_SOLIDPALETTE_H

#include "ColorPalette.h"

class SolidPalette : public ColorPalette {
public:
  explicit SolidPalette(rgb::Color color);
  ~SolidPalette() override = default;
  SolidPalette(const SolidPalette&) = default;
  SolidPalette(SolidPalette&&) = default;
  auto operator=(const SolidPalette&) -> SolidPalette& = default;
  auto operator=(SolidPalette&&) -> SolidPalette& = default;

  auto mapToColor(Cell cell) const -> rgb::Color override;
  auto mapToColor(PieceType type) const -> rgb::Color override;

private:
  rgb::Color color;
};

#endif //TETRISGAME_SOLIDPALETTE_H
