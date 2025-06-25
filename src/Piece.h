//
// Created by Brandon on 6/1/25.
//

#ifndef TETRISGAME_TETRINO_H
#define TETRISGAME_TETRINO_H

#include <array>
#include <utility>
#include <unordered_map>
#include "Point.h"
#include "Color.h"

class Piece {
  using Point = rgb::Point;
  using Color = rgb::Color;

  Piece(Color color, std::array<Point, 4> body, const Piece* leftRotation, const Piece* rightRotation);

public:
  auto begin() const -> const Point* { return body.begin(); }
  auto end() const -> const Point* { return body.end(); }

  const Color color;
  const std::array<Point, 4> body;
  const Piece* const leftRotation;
  const Piece* const rightRotation;

  static auto Random() -> const Piece*;

public:
  static const Piece O;
  static const Piece J_0;
  static const Piece J_1;
  static const Piece J_2;
  static const Piece J_3;
  static const Piece L_0;
  static const Piece L_1;
  static const Piece L_2;
  static const Piece L_3;
  static const Piece T_0;
  static const Piece T_1;
  static const Piece T_2;
  static const Piece T_3;
  static const Piece Z_0;
  static const Piece Z_1;
  static const Piece S_0;
  static const Piece S_1;
  static const Piece I_0;
  static const Piece I_1;

  static const std::array<const Piece*, 7> STARTERS;
};

#endif //TETRISGAME_TETRINO_H
