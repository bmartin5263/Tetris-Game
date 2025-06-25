//
// Created by Brandon on 6/1/25.
//

#include "Piece.h"
#include <random>
#include "Clock.h"

using namespace rgb;

const Piece Piece::O(Color::YELLOW(.02f), {Point{0, 0}, {1, 0}, {0, 1}, {1, 1}}, &O, &O);

const Piece Piece::J_0(Color::BLUE(.02f), {Point{0, 0}, {1, 0}, {2, 0}, {2, 1}}, &J_3, &J_1);
const Piece Piece::J_1(Color::BLUE(.02f), {Point{1, -1}, {1, 0}, {0, 1}, {1, 1}}, &J_0, &J_2);
const Piece Piece::J_2(Color::BLUE(.02f), {Point{0, 0}, {0, 1}, {1, 1}, {2, 1}}, &J_1, &J_3);
const Piece Piece::J_3(Color::BLUE(.02f), {Point{0, -1}, {1, -1}, {0, 0}, {0, 1}}, &J_2, &J_0);

const Piece Piece::L_0(Color::ORANGE(.02f), {Point{0, 0}, {1, 0}, {2, 0}, {0, 1}}, &L_3, &L_1);
const Piece Piece::L_1(Color::ORANGE(.02f), {Point{0, -1}, {1, -1}, {1, 0}, {1, 1}}, &L_0, &L_2);
const Piece Piece::L_2(Color::ORANGE(.02f), {Point{2, 0}, {0, 1}, {1, 1}, {2, 1}}, &L_1, &L_3);
const Piece Piece::L_3(Color::ORANGE(.02f), {Point{0, -1}, {0, 0}, {0, 1}, {1, 1}}, &L_2, &L_0);

const Piece Piece::T_0(Color::CYAN(.02f), {Point{0, 0}, {1, 0}, {2, 0}, {1, 1}}, &T_3, &T_1);
const Piece Piece::T_1(Color::CYAN(.02f), {Point{2, -1}, {1, 0}, {2, 0}, {2, 1}}, &T_0, &T_2);
const Piece Piece::T_2(Color::CYAN(.02f), {Point{1, 0}, {0, 1}, {1, 1}, {2, 1}}, &T_1, &T_3);
const Piece Piece::T_3(Color::CYAN(.02f), {Point{0, -1}, {1, 0}, {0, 0}, {0, 1}}, &T_2, &T_0);

const Piece Piece::Z_0(Color::GREEN(.02f), {Point{0, 0}, {1, 0}, {1, 1}, {2, 1}}, &Z_1, &Z_1);
const Piece Piece::Z_1(Color::GREEN(.02f), {Point{1, -1}, {1, 0}, {0, 0}, {0, 1}}, &Z_0, &Z_0);

const Piece Piece::S_0(Color::MAGENTA(.02f), {Point{1, 0}, {2, 0}, {0, 1}, {1, 1}}, &S_1, &S_1);
const Piece Piece::S_1(Color::MAGENTA(.02f), {Point{0, -1}, {0, 0}, {1, 0}, {1, 1}}, &S_0, &S_0);

const Piece Piece::I_0(Color::RED(.02f), {Point{0, 0}, {1, 0}, {2, 0}, {3, 0}}, &I_1, &I_1);
const Piece Piece::I_1(Color::RED(.02f), {Point{2, -1}, {2, 0}, {2, 1}, {2, 2}}, &I_0, &I_0);

Piece::Piece(Piece::Color color, std::array<Point, 4> body, const Piece* leftRotation,
                     const Piece* rightRotation)
  : color(color), body(body), leftRotation(leftRotation), rightRotation(rightRotation) {

}

const std::array<const Piece*, 7> Piece::STARTERS = std::array{
  &Piece::O,
  &Piece::J_0,
  &Piece::L_0,
  &Piece::T_0,
  &Piece::Z_0,
  &Piece::S_0,
  &Piece::I_0
};

// TODO - micros() is bad random seed
auto Piece::Random() -> const Piece* {

  // Create a random device and seed the generator with a high-resolution clock
  static std::mt19937 generator(static_cast<unsigned int>(micros()));

  // Create a distribution for valid array indices
  std::uniform_int_distribution<int> distribution(0, 6);

  int index = distribution(generator);
  return STARTERS[index];
}
