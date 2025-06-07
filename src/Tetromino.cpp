//
// Created by Brandon on 6/1/25.
//

#include "Tetromino.h"
#include <random>
#include "Clock.h"

using namespace rgb;

const Tetromino Tetromino::O(Color::YELLOW(), {Point{0, 0}, {1, 0}, {0, 1}, {1, 1}}, &O, &O);

const Tetromino Tetromino::J_0(Color::BLUE(), {Point{0, 0}, {1, 0}, {2, 0}, {2, 1}}, &J_3, &J_1);
const Tetromino Tetromino::J_1(Color::BLUE(), {Point{1, -1}, {1, 0}, {0, 1}, {1, 1}}, &J_0, &J_2);
const Tetromino Tetromino::J_2(Color::BLUE(), {Point{0, 0}, {0, 1}, {1, 1}, {2, 1}}, &J_1, &J_3);
const Tetromino Tetromino::J_3(Color::BLUE(), {Point{0, -1}, {1, -1}, {0, 0}, {0, 1}}, &J_2, &J_0);

const Tetromino Tetromino::L_0(Color::ORANGE(), {Point{0, 0}, {1, 0}, {2, 0}, {0, 1}}, &L_3, &L_1);
const Tetromino Tetromino::L_1(Color::ORANGE(), {Point{0, -1}, {1, -1}, {1, 0}, {1, 1}}, &L_0, &L_2);
const Tetromino Tetromino::L_2(Color::ORANGE(), {Point{2, 0}, {0, 1}, {1, 1}, {2, 1}}, &L_1, &L_3);
const Tetromino Tetromino::L_3(Color::ORANGE(), {Point{0, -1}, {0, 0}, {0, 1}, {1, 1}}, &L_2, &L_0);

const Tetromino Tetromino::T_0(Color::CYAN(), {Point{0, 0}, {1, 0}, {2, 0}, {1, 1}}, &T_3, &T_1);
const Tetromino Tetromino::T_1(Color::CYAN(), {Point{2, -1}, {1, 0}, {2, 0}, {2, 1}}, &T_0, &T_2);
const Tetromino Tetromino::T_2(Color::CYAN(), {Point{1, 0}, {0, 1}, {1, 1}, {2, 1}}, &T_1, &T_3);
const Tetromino Tetromino::T_3(Color::CYAN(), {Point{0, -1}, {1, 0}, {0, 0}, {0, 1}}, &T_2, &T_0);

const Tetromino Tetromino::Z_0(Color::GREEN(), {Point{0, 0}, {1, 0}, {1, 1}, {2, 1}}, &Z_1, &Z_1);
const Tetromino Tetromino::Z_1(Color::GREEN(), {Point{1, -1}, {1, 0}, {0, 0}, {0, 1}}, &Z_0, &Z_0);

const Tetromino Tetromino::S_0(Color::MAGENTA(), {Point{1, 0}, {2, 0}, {0, 1}, {1, 1}}, &S_1, &S_1);
const Tetromino Tetromino::S_1(Color::MAGENTA(), {Point{0, -1}, {0, 0}, {1, 0}, {1, 1}}, &S_0, &S_0);

const Tetromino Tetromino::I_0(Color::RED(), {Point{0, 1}, {1, 1}, {2, 1}, {3, 1}}, &I_1, &I_1);
const Tetromino Tetromino::I_1(Color::RED(), {Point{2, -2}, {2, -1}, {2, 0}, {2, 1}}, &I_0, &I_0);

Tetromino::Tetromino(Tetromino::Color color, std::array<Point, 4> body, const Tetromino* leftRotation,
                     const Tetromino* rightRotation)
  : color(color), body(body), leftRotation(leftRotation), rightRotation(rightRotation) {

}

const std::array<const Tetromino*, 7> Tetromino::STARTERS = std::array{
  &Tetromino::O,
  &Tetromino::J_0,
  &Tetromino::L_0,
  &Tetromino::T_0,
  &Tetromino::Z_0,
  &Tetromino::S_0,
  &Tetromino::I_0
};

// TODO - micros() is bad random seed
auto Tetromino::Random() -> const Tetromino* {

  // Create a random device and seed the generator with a high-resolution clock
  static std::mt19937 generator(static_cast<unsigned int>(micros()));

  // Create a distribution for valid array indices
  std::uniform_int_distribution<int> distribution(0, 6);

  int index = distribution(generator);
  return STARTERS[index];
}
