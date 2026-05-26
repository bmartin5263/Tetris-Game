//
// Created by Brandon on 4/30/26.
//

#ifndef TETRISGAME_RAINBOWPALETTE_H
#define TETRISGAME_RAINBOWPALETTE_H

#include "ColorfulPalette.h"

class RainbowPalette : public ColorfulPalette {
public:
  RainbowPalette() = default;
  ~RainbowPalette() override = default;
  RainbowPalette(const RainbowPalette&) = default;
  RainbowPalette(RainbowPalette&&) = default;
  auto operator=(const RainbowPalette&) -> RainbowPalette& = default;
  auto operator=(RainbowPalette&&) -> RainbowPalette& = default;

  using ColorfulPalette::mapToColor;
  auto mapToColor(PieceType type) const -> rgb::Color override;
};

#endif //TETRISGAME_RAINBOWPALETTE_H
