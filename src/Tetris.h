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
#include "Score.h"
#include "MoveResult.h"
#include "Iterable.h"

using Cell = rgb::Color;

template<size_t COLUMNS>
struct Row {
  using Color = rgb::Color;

  std::array<Cell, COLUMNS> data;

  auto operator[](size_t index) -> Color& {
    return data[index];
  }

  auto begin() {
    return data.begin();
  }

  auto end() {
    return data.end();
  }

  auto isFilled() -> bool {
    auto isNotOff = [](auto& c) { return c != Color::OFF(); };
    return std::all_of(data.begin(), data.end(), isNotOff);
  }

  auto clear() -> void {
    data.fill(Color::OFF());
  }
};

template<size_t COLUMNS, size_t ROWS>
class Tetris {
public:
  constexpr static auto SIZE = COLUMNS * ROWS;
  constexpr static auto START_POSITION = rgb::Point { static_cast<int>(COLUMNS / 2) - 1, 0 };
  constexpr static auto BOTTOM_POSITION = rgb::Point { static_cast<int>(COLUMNS / 2) - 1, ROWS - 1 };
  constexpr static auto POINT_VALUES = std::array { 100, 300, 1200, 3600 };

  using Point = rgb::Point;
  using PixelList = rgb::PixelList;
  using Color = rgb::Color;
  using Board = std::array<Row<COLUMNS>, ROWS>;
  template<typename T>
  using Iterable = rgb::Iterable<T>;

  auto draw(PixelList& ledChain) -> void;

  // Initializes a new game
  auto newGame() -> void;
  auto nextTetromino() -> void;
  auto clearRows(const std::array<size_t, 4> rows, size_t count) -> void;


  auto moveTetromino(Point movement) -> MoveResult;
  auto dropTetromino() -> MoveResult;
  auto rotateTetrominoLeft() -> void;
  auto rotateTetrominoRight() -> void;
  auto getScore() -> const Score&;

  auto columnCount() -> size_t { return COLUMNS; }

  Board board{Color::OFF()};
private:
  const Tetromino* currentTetromino{&Tetromino::O};
  Point currentPosition{START_POSITION};
  Point ghostPosition{START_POSITION};
  Score score{};
  bool gameOver{false};
  bool ghostEnabled{true};

  auto placeCurrentTetromino() -> void {
    INFO("PLACE CURRENT TETROMINO");
    if (ghostEnabled) {
      placeTetromino(currentTetromino, ghostPosition, Color::WHITE(.01f));
    }
    placeTetromino(currentTetromino, currentPosition);
  }

  auto removeCurrentTetromino() -> void {
    INFO("REMOVE CURRENT TETROMINO");
    if (ghostEnabled) {
      removeTetromino(currentTetromino, ghostPosition);
    }
    removeTetromino(currentTetromino, currentPosition);
  }

  auto calculateGhostPosition() -> Point;
  auto placeTetromino(const Tetromino* tetromino, Point position) -> void;
  auto placeTetromino(const Tetromino* tetromino, Point position, Color color) -> void;
  auto removeTetromino(const Tetromino* tetromino, Point position) -> void;
  auto canPlaceTetromino(const Tetromino* tetromino, Point position) -> bool;
  auto canPlaceCurrentTetrominoAtOffset(Point offset) -> bool;
  auto clearRow(size_t rowNum) -> void;
  auto calculateRowsToClear(MoveResult& result) -> void;

  struct PickUpCurrentTetromino {
    Tetris<COLUMNS, ROWS>& tetris;
    explicit PickUpCurrentTetromino(Tetris<COLUMNS, ROWS>& tetris): tetris(tetris) {
      tetris.removeCurrentTetromino();
    }

    ~PickUpCurrentTetromino() {
      tetris.placeCurrentTetromino();
    }
  };
};

template<size_t COLUMNS, size_t ROWS>
auto Tetris<COLUMNS, ROWS>::calculateGhostPosition() -> Tetris::Point {
  auto position = currentPosition;
  position.y += 1;
  while (canPlaceTetromino(currentTetromino, position) && position != BOTTOM_POSITION) {
    position.y += 1;
  }
  return position - Point {0, 1};
}

template<size_t COLUMNS, size_t ROWS>
auto Tetris<COLUMNS, ROWS>::getScore() -> const Score& {
  return score;
}

template<size_t COLUMNS, size_t ROWS>
auto Tetris<COLUMNS, ROWS>::dropTetromino() -> MoveResult {
  if (gameOver) {
    return {};
  }
  auto offset = Point {0, 1};
  while (canPlaceCurrentTetrominoAtOffset(offset)) {
    ++offset.y;
  }
  --offset.y;
  auto result = moveTetromino(offset);
  result.nextTetromino = true;
  calculateRowsToClear(result);
  return result;
}

template<size_t COLUMNS, size_t ROWS>
auto Tetris<COLUMNS, ROWS>::newGame() -> void {
  INFO("NEW GAME");
  std::for_each(board.begin(), board.end(), [](auto& row) {
    std::fill(row.begin(), row.end(), Color::OFF());
  });

  gameOver = false;
  nextTetromino();

//  auto off = Color::OFF();
//  auto red = Color::RED();
//  auto blu = Color::BLUE();
//  auto gre = Color::GREEN();
//  auto yel = Color::YELLOW();
//  board = {
//    Row<COLUMNS> {off, off, off, off, off, off, off, off},
//    Row<COLUMNS> {off, off, off, off, off, off, off, off},
//    Row<COLUMNS> {off, off, off, off, off, off, off, off},
//    Row<COLUMNS> {off, off, off, off, off, off, off, off},
//    Row<COLUMNS> {yel, yel, yel, yel, yel, off, yel, yel},
//    Row<COLUMNS> {blu, blu, blu, blu, blu, blu, blu, blu},
//    Row<COLUMNS> {gre, gre, gre, gre, gre, off, gre, gre},
//    Row<COLUMNS> {red, red, red, red, red, red, red, red}
//  };
//  placeCurrentTetromino();

}

template<size_t COLUMNS, size_t ROWS>
auto Tetris<COLUMNS, ROWS>::calculateRowsToClear(MoveResult& result) -> void {
  for (size_t rowNum = 0; rowNum < ROWS; ++rowNum) {
    auto& row = board[rowNum];
    if (row.isFilled()) {
      result.addRowToClear(rowNum);
    }
  }
}

template<size_t COLUMNS, size_t ROWS>
auto Tetris<COLUMNS, ROWS>::clearRows(const std::array<size_t, 4> rows, size_t count) -> void {
  if (count > 0) {
    score.points += POINT_VALUES[count - 1];
    score.clearedRows += count;
    score.combos[count - 1] += 1;
  }
  for (int i = 0; i < count; ++i) {
    clearRow(rows[i]);
  }
}


template<size_t COLUMNS, size_t ROWS>
auto Tetris<COLUMNS, ROWS>::clearRow(size_t rowNum) -> void {
  auto rowToDropNum = rowNum;
  while (rowToDropNum > 0) {
    board[rowToDropNum] = board[rowToDropNum - 1];
    --rowToDropNum;
  }
  board[0].clear();
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
    ghostPosition = calculateGhostPosition();
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
    ghostPosition = calculateGhostPosition();
  }
}

template<size_t COLUMNS, size_t ROWS>
auto Tetris<COLUMNS, ROWS>::draw(PixelList& ledChain) -> void {
  for (int row = 0; row < ROWS; ++row) {
    for (int col = 0; col < COLUMNS; ++col) {
      ledChain[((row * ROWS) + col)] = board[row][col];
    }
  }
}

template<size_t COLUMNS, size_t ROWS>
auto Tetris<COLUMNS, ROWS>::moveTetromino(Point movement) -> MoveResult {
  if (gameOver) {
    return {};
  }

  INFO("MOVE TETROMINO");
  auto result = MoveResult{};
  auto isDrop = movement.y > 0;
  if (canPlaceCurrentTetrominoAtOffset(movement)) {
    auto pickUpCurrentTetromino = PickUpCurrentTetromino(*this);
    currentPosition += movement;
    ghostPosition = calculateGhostPosition();
    result.didMove = true;
  }
  if (!result.didMove && isDrop) {
    result.nextTetromino = true;
    calculateRowsToClear(result);
  }

  return result;
}



template<size_t COLUMNS, size_t ROWS>
auto Tetris<COLUMNS, ROWS>::nextTetromino() -> void {
  currentPosition = START_POSITION;
  currentTetromino = Tetromino::Random();
  ghostPosition = calculateGhostPosition();

  if (!canPlaceTetromino(currentTetromino, currentPosition)) {
    for (auto& row : board) {
      for (auto& c : row) {
        if (c != Color::OFF()) {
          c = Color(.03f, .03f, .03f);
        }
      }
    }
    gameOver = true;
  }

  placeCurrentTetromino();
  INFO("Next Tetromino Complete");
}

template<size_t COLUMNS, size_t ROWS>
auto Tetris<COLUMNS, ROWS>::canPlaceCurrentTetrominoAtOffset(Point offset) -> bool {
  INFO("CAN PLACE CURRENT AT OFFSET");
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
    INFO("Placing at %i, %i", pos.x, pos.y);
    board[pos.y][pos.x] = tetromino->color;
  }
}

template<size_t COLUMNS, size_t ROWS>
auto Tetris<COLUMNS, ROWS>::placeTetromino(const Tetromino* tetromino, Point position, Color color) -> void {
  for (auto& offset : tetromino->body) {
    auto pos = position + offset;
    ASSERT(pos.x >= 0 && pos.x < COLUMNS, "Position x out of range");
    ASSERT(pos.y >= 0 && pos.y < ROWS, "Position y out of range");
    INFO("Placing at %i, %i", pos.x, pos.y);
    board[pos.y][pos.x] = color;
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
//    ASSERT(board[pos.y][pos.x] != Color::OFF(), "No tetromino found");
    board[pos.y][pos.x] = Color::OFF();
  }
}

#endif //TETRISGAME_TETRIS_H
