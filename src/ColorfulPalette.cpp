//
// Created by Brandon on 4/30/26.
//

#include "ColorfulPalette.h"
#include "Clock.h"
#include "Assertions.h"

using namespace rgb;

namespace {
constexpr auto DESTROY_COLOR = Color::WHITE() * .05f;
constexpr auto GHOST_COLOR = Color::WHITE() * .01f;
constexpr auto GAME_OVER_LOW_COLOR = Color::RED() * .005f;
constexpr auto GAME_OVER_HIGH_COLOR = Color::RED() * .04f;
}

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
      color = GHOST_COLOR;
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
      color += Color::YELLOW() * .02f;
      break;
    case PieceType::J:
      color += Color::BLUE() * .02f;
      break;
    case PieceType::L:
      color += Color::ORANGE() * .02f;
      break;
    case PieceType::T:
      color += Color::CYAN() * .02f;
      break;
    case PieceType::Z:
      color += Color::GREEN() * .02f;
      break;
    case PieceType::S:
      color += Color::MAGENTA() * .02f;
      break;
    case PieceType::I:
      color += Color(1.0f, 0.0f, 0.18f) * .02f;
      break;
    default:
      ASSERT(false, "Unhandled PieceType");
  }
  return color;
}
