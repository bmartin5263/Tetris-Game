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

class Tetromino {
  using Point = rgb::Point;
  using Color = rgb::Color;

  Tetromino(Color color, std::array<Point, 4> body, const Tetromino* leftRotation, const Tetromino* rightRotation);

public:
  const Color color;
  const std::array<Point, 4> body;
  const Tetromino* const leftRotation;
  const Tetromino* const rightRotation;

  static auto Random() -> const Tetromino*;

public:
  // Square
  static const Tetromino O;
  static const Tetromino J_0;
  static const Tetromino J_1;
  static const Tetromino J_2;
  static const Tetromino J_3;
  static const Tetromino L_0;
  static const Tetromino L_1;
  static const Tetromino L_2;
  static const Tetromino L_3;
  static const Tetromino T_0;
  static const Tetromino T_1;
  static const Tetromino T_2;
  static const Tetromino T_3;
  static const Tetromino Z_0;
  static const Tetromino Z_1;
  static const Tetromino S_0;
  static const Tetromino S_1;
  static const Tetromino I_0;
  static const Tetromino I_1;

  static const std::array<const Tetromino*, 7> STARTERS;
};

#endif //TETRISGAME_TETRINO_H
