//
// Created by Brandon on 6/1/25.
//

#ifndef TETRISGAME_TETRIS_H
#define TETRISGAME_TETRIS_H

#include "Color.h"
#include "Tetromino.h"
#include "Every.h"
#include "LEDMatrix.h"
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
  using PixelList = rgb::PixelList;
  using Every = rgb::Every;
  using Color = rgb::Color;
  using Cell = Color;
  using Board = std::array<std::array<Cell, COLUMNS>, ROWS>;
  using TimerHandle = rgb::TimerHandle;

  auto newGame() -> void;
  auto update() -> void;
  auto draw(PixelList& ledChain) -> void;

  auto moveTetromino(Point movement) -> void;
  auto dropTetromino() -> void;
  auto rotateTetrominoLeft() -> void;
  auto rotateTetrominoRight() -> void;

private:
  auto autoFall() -> bool;
  auto nextTetromino() -> void;
  auto placeCurrentTetromino() -> void { placeTetromino(currentTetromino, currentPosition); }
  auto removeCurrentTetromino() -> void { removeTetromino(currentTetromino, currentPosition); }
  auto placeTetromino(const Tetromino* tetromino, Point position) -> void;
  auto removeTetromino(const Tetromino* tetromino, Point position) -> void;
  auto canPlaceTetromino(const Tetromino* tetromino, Point position) -> bool;
  auto canPlaceTetrominoAtOffset(Point offset) -> bool;
  auto shouldClearRows() -> bool;

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
  Every autoDropTimer = Every{Duration::Seconds(1), [this]() {
    autoFall();
  }};
  TimerHandle clearRowsHandle{};
  bool gameOver{false};
};

template<size_t COLUMNS, size_t ROWS>
auto Tetris<COLUMNS, ROWS>::dropTetromino() -> void {
  if (gameOver) {
    return;
  }
  while (autoFall());
}

template<size_t COLUMNS, size_t ROWS>
auto Tetris<COLUMNS, ROWS>::newGame() -> void {
  INFO("START()");
  std::for_each(board.begin(), board.end(), [](auto& row) {
    std::fill(row.begin(), row.end(), Color::OFF());
  });
  gameOver = false;
  autoDropTimer.reset();
  nextTetromino();
}

template<size_t COLUMNS, size_t ROWS>
auto Tetris<COLUMNS, ROWS>::shouldClearRows() -> bool {
  for (auto& row : board) {

  }
  return false;
}

template<size_t COLUMNS, size_t ROWS>
auto Tetris<COLUMNS, ROWS>::moveTetromino(Point movement) -> void {
  if (gameOver) {
    return;
  }

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
  if (gameOver) {
    return;
  }
  auto pickUpCurrentTetromino = PickUpCurrentTetromino(*this);
  auto rotated = currentTetromino->leftRotation;
  if (canPlaceTetromino(rotated, currentPosition)) {
    currentTetromino = rotated;
  }
}

template<size_t COLUMNS, size_t ROWS>
auto Tetris<COLUMNS, ROWS>::rotateTetrominoRight() -> void {
  if (gameOver) {
    return;
  }
  auto pickUpCurrentTetromino = PickUpCurrentTetromino(*this);
  auto rotated = currentTetromino->rightRotation;
  if (canPlaceTetromino(rotated, currentPosition)) {
    currentTetromino = rotated;
  }
}

template<size_t COLUMNS, size_t ROWS>
auto Tetris<COLUMNS, ROWS>::update() -> void {
  if (!gameOver) {
    autoDropTimer.update();
  }
}

template<size_t COLUMNS, size_t ROWS>
auto Tetris<COLUMNS, ROWS>::draw(PixelList& ledChain) -> void {
  for (int row = 0; row < ROWS; ++row) {
    for (int col = 0; col < COLUMNS; ++col) {
      ledChain[((row * ROWS) + col)] = board[row][col];
    }
  }
  ledChain[((currentPosition.y * ROWS) + currentPosition.x)] = Color::WHITE();
}

template<size_t COLUMNS, size_t ROWS>
auto Tetris<COLUMNS, ROWS>::autoFall() -> bool {
  INFO("AutoDrop");
  auto offset = Point {0, 1};
  if (canPlaceTetrominoAtOffset(offset)) {
    auto pickUpCurrentTetromino = PickUpCurrentTetromino(*this);
    currentPosition += offset;
    return true;
  }
  else {
    nextTetromino();
    return false;
  }
}

template<size_t COLUMNS, size_t ROWS>
auto Tetris<COLUMNS, ROWS>::nextTetromino() -> void {
  currentPosition = START_POSITION;
  currentTetromino = Tetromino::Random();
  autoDropTimer.reset();

  if (!canPlaceTetromino(currentTetromino, currentPosition)) {
    for (auto& row : board) {
      for (auto& c : row) {
        if (c != Color::OFF()) {
          c = Color::RED();
        }
      }
    }
    INFO("GAMEOVER");
    gameOver = true;
  }

  placeCurrentTetromino();
}

template<size_t COLUMNS, size_t ROWS>
auto Tetris<COLUMNS, ROWS>::canPlaceTetrominoAtOffset(Point offset) -> bool {
  auto pickUpCurrentTetromino = PickUpCurrentTetromino(*this);
  auto pos = currentPosition + offset;
  return canPlaceTetromino(currentTetromino, pos);
}

template<size_t COLUMNS, size_t ROWS>
auto Tetris<COLUMNS, ROWS>::placeTetromino(const Tetromino* tetromino, Point position) -> void {
  for (auto& offset : tetromino->body) {
    auto pos = position + offset;
    ASSERT(pos.x >= 0 && pos.x < COLUMNS, "Position x out of range");
    ASSERT(pos.y >= 0 && pos.y < ROWS, "Position y out of range");
    board[pos.y][pos.x] = tetromino->color;
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
        && board[actualPosition.y][actualPosition.x] == Color::OFF();
  });
}

template<size_t COLUMNS, size_t ROWS>
auto Tetris<COLUMNS, ROWS>::removeTetromino(const Tetromino* tetromino, Point position) -> void {
  for (auto& offset : tetromino->body) {
    auto pos = position + offset;
    ASSERT(pos.x >= 0 && pos.x < COLUMNS, "Position x out of range");
    ASSERT(pos.y >= 0 && pos.y < ROWS, "Position y out of range");
    ASSERT(board[pos.y][pos.x] != Color::OFF(), "No tetromino found");
    board[pos.y][pos.x] = Color::OFF();
  }
}

#endif //TETRISGAME_TETRIS_H
