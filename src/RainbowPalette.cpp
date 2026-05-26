//
// Created by Brandon on 4/30/26.
//

#include "RainbowPalette.h"
#include "Clock.h"

using namespace rgb;

namespace {
constexpr auto DURATION = Duration::Seconds(10);
}

auto RainbowPalette::mapToColor(PieceType type) const -> Color {
  auto time = Clock::Now().percentOfWrapped(DURATION);
  return Color::HslToRgb(time) * .02f;
}
