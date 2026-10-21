//
// Created by Brandon on 4/30/26.
//

#include "ColorfulPalette.h"
#include "Clock.h"
#include "Assertions.h"

using namespace rgb;

namespace {
constexpr auto DESTROY_COLOR = Color::WHITE() * .05f;
constexpr auto GAME_OVER_LOW_COLOR = Color::RED() * .005f;
constexpr auto GAME_OVER_HIGH_COLOR = Color::RED() * .04f;
}

ColorfulPalette::ColorfulPalette(const float& brightness) : brightness{&brightness} {}

auto ColorfulPalette::mapToColor(Cell cell) const -> Color {
  Color color = Color::OFF();

  if (cell.destroying) {
    return DESTROY_COLOR;
  }

  auto speed = Duration::Milliseconds(600);
  auto time = Clock::Now().mod(speed).as<float>() / speed.as<float>();

  switch (cell.type) {
    case PieceType::EMPTY:
      break;
    case PieceType::GHOST:
      color = Color::WHITE() * *brightness;
      break;
    case PieceType::GAMEOVER:
      color = GAME_OVER_HIGH_COLOR.lerpWrap(GAME_OVER_LOW_COLOR, time);
      break;
    default:
      color = mapToColor(cell.type);
  }
  return color;
}

auto ColorfulPalette::mapToColor(PieceType type) const -> Color {
  Color color = Color::OFF();
  switch (type) {
    case PieceType::O:
      color += Color::YELLOW() * *brightness;
      break;
    case PieceType::J:
      color += Color::BLUE() * *brightness;
      break;
    case PieceType::L:
      color += Color::ORANGE() * *brightness;
      break;
    case PieceType::T:
      color += Color::CYAN() * *brightness;
      break;
    case PieceType::Z:
      color += Color::GREEN() * *brightness;
      break;
    case PieceType::S:
      color += Color::MAGENTA() * *brightness;
      break;
    case PieceType::I:
      color += Color::RED() * *brightness;
      break;
    default:
      ASSERT(false, "Unhandled PieceType");
  }
  return color;
}
