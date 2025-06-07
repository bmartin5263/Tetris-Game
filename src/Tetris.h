//
// Created by Brandon on 6/1/25.
//

#ifndef TETRISGAME_TETRIS_H
#define TETRISGAME_TETRIS_H

#include "Color.h"
#include "Tetromino.h"
#include "Every.h"
#include "LEDChain.h"
#include "Types.h"
#include "Assertions.h"
#include "Timer.h"

template<size_t COLUMNS, size_t ROWS>
class Tetris {
public:
  constexpr static auto SIZE = COLUMNS * ROWS;
  constexpr static auto START_POSITION = rgb::Point { static_cast<int>(COLUMNS / 2) - 1, 0 };

  using Duration = rgb::Duration;
  using Point = rgb::Point;
  using LEDChain = rgb::LEDChain;
  using Every = rgb::Every;
  using Color = rgb::Color;
  using Cell = Color;
  using Board = std::array<Cell, SIZE>;
  using TimerHandle = rgb::TimerHandle;

  Tetris();

  auto update() -> void;
  auto draw(LEDChain& ledChain) -> void;

  auto moveTetromino(Point movement) -> void;
  auto rotateTetrominoLeft() -> void;
  auto rotateTetrominoRight() -> void;

private:
  auto autoDrop() -> void;
  auto nextTetromino() -> void;
  auto placeCurrentTetromino() -> void { placeTetromino(currentTetromino, currentPosition); }
  auto removeCurrentTetromino() -> void { removeTetromino(currentTetromino, currentPosition); }
  auto placeTetromino(const Tetromino* tetromino, Point position) -> void;
  auto removeTetromino(const Tetromino* tetromino, Point position) -> void;
  auto canPlaceTetromino(const Tetromino* tetromino, Point position) -> bool;
  auto canPlaceTetrominoAtOffset(Point offset) -> bool;

  struct PickUpCurrentTetromino {
    Tetris<COLUMNS, ROWS>& tetris;
    PickUpCurrentTetromino(Tetris<COLUMNS, ROWS>& tetris): tetris(tetris) {
      tetris.removeCurrentTetromino();
    }

    ~PickUpCurrentTetromino() {
      tetris.placeCurrentTetromino();
    }
  };

  Board board{Color::OFF()};
  const Tetromino* currentTetromino{&Tetromino::O};
  Point currentPosition{START_POSITION};
  Every autoDropTimer = Every(Duration::Seconds(1), [this]() {
    autoDrop();
  });
  TimerHandle clearRowsHandle{};
  bool gameOver{false};
};

template<size_t COLUMNS, size_t ROWS>
auto Tetris<COLUMNS, ROWS>::moveTetromino(Point movement) -> void {
  bool isDrop = movement.y > 0;
  bool didMove = false;
  {
    auto pickUpCurrentTetromino = PickUpCurrentTetromino(*this);
    if (canPlaceTetromino(currentTetromino, currentPosition + movement)) {
      currentPosition += movement;
      if (movement.y > 0) {
        autoDropTimer.reset();
      }
      didMove = true;
    }
  }
  if (!didMove && isDrop) {
    nextTetromino();
  }
}

template<size_t COLUMNS, size_t ROWS>
auto Tetris<COLUMNS, ROWS>::rotateTetrominoLeft() -> void {
  auto pickUpCurrentTetromino = PickUpCurrentTetromino(*this);
  auto rotated = currentTetromino->leftRotation;
  if (canPlaceTetromino(rotated, currentPosition)) {
    currentTetromino = rotated;
  }
}

template<size_t COLUMNS, size_t ROWS>
auto Tetris<COLUMNS, ROWS>::rotateTetrominoRight() -> void {
  auto pickUpCurrentTetromino = PickUpCurrentTetromino(*this);
  auto rotated = currentTetromino->rightRotation;
  if (canPlaceTetromino(rotated, currentPosition)) {
    currentTetromino = rotated;
  }
}

template<size_t COLUMNS, size_t ROWS>
Tetris<COLUMNS, ROWS>::Tetris() {
  nextTetromino();
}

template<size_t COLUMNS, size_t ROWS>
auto Tetris<COLUMNS, ROWS>::update() -> void {
  if (!gameOver) {
    autoDropTimer.update();
  }
}

template<size_t COLUMNS, size_t ROWS>
auto Tetris<COLUMNS, ROWS>::draw(LEDChain& ledChain) -> void {
  for (int i = 0; i < 64; ++i) {
    ledChain[i] = board[i];
  }
}

template<size_t COLUMNS, size_t ROWS>
auto Tetris<COLUMNS, ROWS>::autoDrop() -> void {
  auto offset = Point {0, 1};
  if (canPlaceTetrominoAtOffset(offset)) {
    auto pickUpCurrentTetromino = PickUpCurrentTetromino(*this);
    currentPosition += offset;
  }
  else {
    nextTetromino();
  }
}

template<size_t COLUMNS, size_t ROWS>
auto Tetris<COLUMNS, ROWS>::nextTetromino() -> void {
  currentPosition = START_POSITION;
  currentTetromino = Tetromino::Random();

  if (!canPlaceTetromino(currentTetromino, currentPosition)) {
    for (auto& c : board) {
      if (c != Color::OFF()) {
        c = Color::RED();
      }
    }
    gameOver = true;
  }

  placeCurrentTetromino();
}

template<size_t COLUMNS, size_t ROWS>
auto Tetris<COLUMNS, ROWS>::canPlaceTetrominoAtOffset(Point offset) -> bool {
  auto pickUpCurrentTetromino = PickUpCurrentTetromino(*this);
  auto pos = currentPosition + offset;
  auto canPlace = canPlaceTetromino(currentTetromino, pos);
  return canPlace;
}

template<size_t COLUMNS, size_t ROWS>
auto Tetris<COLUMNS, ROWS>::placeTetromino(const Tetromino* tetromino, Point position) -> void {
  for (auto& offset : tetromino->body) {
    auto pos = position + offset;
    ASSERT(pos.x >= 0 && pos.x < COLUMNS, "Position x out of range");
    ASSERT(pos.y >= 0 && pos.y < ROWS, "Position y out of range");
    board[pos.flatten(COLUMNS)] = tetromino->color;
  }
}

template<size_t COLUMNS, size_t ROWS>
auto Tetris<COLUMNS, ROWS>::canPlaceTetromino(const Tetromino* tetromino, Point position) -> bool {
  return std::all_of(tetromino->begin(), tetromino->end(), [&](auto offset){
    auto actualPosition = position + offset;
    return actualPosition.x >= 0
        && actualPosition.x < COLUMNS
        && actualPosition.y >= 0
        && actualPosition.y < ROWS
        && board[actualPosition.flatten(COLUMNS)] == Color::OFF();
  });
}

template<size_t COLUMNS, size_t ROWS>
auto Tetris<COLUMNS, ROWS>::removeTetromino(const Tetromino* tetromino, Point position) -> void {
  for (auto& offset : tetromino->body) {
    auto pos = position + offset;
    ASSERT(pos.x >= 0 && pos.x < COLUMNS, "Position x out of range");
    ASSERT(pos.y >= 0 && pos.y < ROWS, "Position y out of range");
    board[pos.flatten(COLUMNS)] = Color::OFF();
  }
}

#endif //TETRISGAME_TETRIS_H
