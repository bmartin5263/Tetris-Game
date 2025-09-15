//
// Created by Brandon on 6/1/25.
//

#ifndef TETRISGAME_TETRIS_H
#define TETRISGAME_TETRIS_H

#include "Piece.h"
#include "Every.h"
#include "PixelGrid.h"
#include "Types.h"
#include "Assertions.h"
#include "Timer.h"
#include "Score.h"
#include "MoveResult.h"
#include "Iterable.h"

struct Cell {
  PieceType type{PieceType::EMPTY};
  bool destroying{false};
};

template<size_t COLUMNS>
struct Row {
  std::array<Cell, COLUMNS> data;

  auto operator[](size_t index) -> Cell& {
    return data[index];
  }

  auto begin() {
    return data.begin();
  }

  auto end() {
    return data.end();
  }

  auto isFilled() -> bool {
    auto isNotEmpty = [](const Cell& c) { return c.type != PieceType::EMPTY; };
    return std::all_of(data.begin(), data.end(), isNotEmpty);
  }

  auto clear() -> void {
    data.fill(Cell{});
  }
};

template<size_t COLUMNS, size_t ROWS>
class Tetris {
  using Point = rgb::Point;
  using PixelGrid = rgb::PixelGrid;
  using Board = std::array<Row<COLUMNS>, ROWS>;
  using Timestamp = rgb::Timestamp;
  template<typename T>
  using Iterable = rgb::Iterable<T>;

public:
  constexpr static auto START_POSITION = rgb::Point { static_cast<int>(COLUMNS / 2) - 1, 1 };
  constexpr static auto POINT_VALUES = std::array { 100, 300, 1200, 3600 };

  // Initializes a new game
  auto newGame() -> void;
  auto nextPiece() -> void;
  auto clearRows(const std::array<size_t, 4>& rows, size_t count) -> void;


  auto movePiece(Point movement) -> MoveResult;
  auto dropPiece() -> MoveResult;
  auto rotatePieceLeft() -> void;
  auto rotatePieceRight() -> void;
  auto getScore() -> const Score&;
  auto toggleGhost() -> void;
  constexpr auto isGameOver() -> bool;

  auto columnCount() -> size_t { return COLUMNS; }


  Board board{PieceType::EMPTY};
  const Piece* currentPiece{&Piece::O};
private:
  Point currentPosition{START_POSITION};
  Point ghostPosition{START_POSITION};
  Score score{};
  bool gameOver{false};
  bool ghostEnabled{false};

  auto placeCurrentPiece() -> void {
    TRACE("PLACE CURRENT TETROMINO");
    if (ghostEnabled) {
      placePiece(currentPiece, ghostPosition, PieceType::GHOST);
    }
    placePiece(currentPiece, currentPosition);
  }

  auto removeCurrentPiece() -> void {
    TRACE("REMOVE CURRENT TETROMINO");
    if (ghostEnabled) {
      removePiece(currentPiece, ghostPosition);
    }
    removePiece(currentPiece, currentPosition);
  }

  auto calculateGhostPosition() -> Point;
  auto placePiece(const Piece* piece, Point position) -> void;
  auto placePiece(const Piece* piece, Point position, PieceType type) -> void;
  auto removePiece(const Piece* piece, Point position) -> void;
  auto canPlacePiece(const Piece* piece, Point position) -> bool;
  auto canPlaceCurrentPieceAtOffset(Point offset) -> bool;
  auto clearRow(size_t rowNum) -> void;
  auto calculateRowsToClear(MoveResult& result) -> void;

  struct PickUpCurrentPiece {
    Tetris<COLUMNS, ROWS>& tetris;
    explicit PickUpCurrentPiece(Tetris<COLUMNS, ROWS>& tetris): tetris(tetris) {
      tetris.removeCurrentPiece();
    }

    ~PickUpCurrentPiece() {
      tetris.placeCurrentPiece();
    }
  };
};

template<size_t COLUMNS, size_t ROWS>
constexpr auto Tetris<COLUMNS, ROWS>::isGameOver() -> bool {
  return gameOver;
}

template<size_t COLUMNS, size_t ROWS>
auto Tetris<COLUMNS, ROWS>::toggleGhost() -> void {
  ghostEnabled = !ghostEnabled;
  if (ghostEnabled) {
    // TODO buggy code
    removePiece(currentPiece, currentPosition);
    placePiece(currentPiece, ghostPosition, PieceType::GHOST);
    placePiece(currentPiece, currentPosition);
  }
  else {
    removePiece(currentPiece, ghostPosition);
    placePiece(currentPiece, currentPosition);
  }
}

template<size_t COLUMNS, size_t ROWS>
auto Tetris<COLUMNS, ROWS>::calculateGhostPosition() -> Tetris::Point {
  auto position = currentPosition;
  while (canPlacePiece(currentPiece, position)) {
    position.y += 1;
  }
  return position - Point {0, 1};
}

template<size_t COLUMNS, size_t ROWS>
auto Tetris<COLUMNS, ROWS>::getScore() -> const Score& {
  return score;
}

template<size_t COLUMNS, size_t ROWS>
auto Tetris<COLUMNS, ROWS>::dropPiece() -> MoveResult {
  if (gameOver) {
    return {};
  }
  auto offset = Point {0, 1};
  while (canPlaceCurrentPieceAtOffset(offset)) {
    ++offset.y;
  }
  --offset.y;
  auto result = movePiece(offset);
  result.nextPiece = true;
  calculateRowsToClear(result);
  return result;
}

template<size_t COLUMNS, size_t ROWS>
auto Tetris<COLUMNS, ROWS>::newGame() -> void {
  TRACE("NEW GAME");
  std::for_each(board.begin(), board.end(), [](auto& row) {
    row.clear();
  });

  score.reset();
  gameOver = false;
  nextPiece();

//  auto off = PieceType::EMPTY;
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
//  placeCurrentPiece();

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
auto Tetris<COLUMNS, ROWS>::clearRows(const std::array<size_t, 4>& rows, size_t count) -> void {
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
auto Tetris<COLUMNS, ROWS>::rotatePieceLeft() -> void {
  if (gameOver) {
    return;
  }
  auto pickUpCurrentPiece = PickUpCurrentPiece(*this);
  auto rotated = currentPiece->leftRotation;
  if (canPlacePiece(rotated, currentPosition)) {
    currentPiece = rotated;
    ghostPosition = calculateGhostPosition();
  }
}

template<size_t COLUMNS, size_t ROWS>
auto Tetris<COLUMNS, ROWS>::rotatePieceRight() -> void {
  if (gameOver) {
    return;
  }
  auto pickUpCurrentPiece = PickUpCurrentPiece(*this);
  auto rotated = currentPiece->rightRotation;
  if (canPlacePiece(rotated, currentPosition)) {
    currentPiece = rotated;
    ghostPosition = calculateGhostPosition();
  }
}

template<size_t COLUMNS, size_t ROWS>
auto Tetris<COLUMNS, ROWS>::movePiece(Point movement) -> MoveResult {
  if (gameOver) {
    return {};
  }

  TRACE("MOVE TETROMINO");
  auto result = MoveResult{};
  auto isDrop = movement.y > 0;
  if (canPlaceCurrentPieceAtOffset(movement)) {
    auto pickUpCurrentPiece = PickUpCurrentPiece(*this);
    currentPosition += movement;
    ghostPosition = calculateGhostPosition();
    result.didMove = true;
  }
  if (!result.didMove && isDrop) {
    result.nextPiece = true;
    calculateRowsToClear(result);
  }

  return result;
}



template<size_t COLUMNS, size_t ROWS>
auto Tetris<COLUMNS, ROWS>::nextPiece() -> void {
  currentPosition = START_POSITION;
  currentPiece = Piece::Random();
  ghostPosition = calculateGhostPosition();

  if (!canPlacePiece(currentPiece, currentPosition)) {
    currentPosition.y -= 1;
    if (!canPlacePiece(currentPiece, currentPosition)) {
      gameOver = true;
    }
  }

  placeCurrentPiece();
  TRACE("Next Piece Complete");
}

template<size_t COLUMNS, size_t ROWS>
auto Tetris<COLUMNS, ROWS>::canPlaceCurrentPieceAtOffset(Point offset) -> bool {
  TRACE("CAN PLACE CURRENT AT OFFSET");
  auto pickUpCurrentPiece = PickUpCurrentPiece(*this);
  auto pos = currentPosition + offset;
  return canPlacePiece(currentPiece, pos);
}

template<size_t COLUMNS, size_t ROWS>
auto Tetris<COLUMNS, ROWS>::placePiece(const Piece* piece, Point position) -> void {
  placePiece(piece, position, piece->type);
}

template<size_t COLUMNS, size_t ROWS>
auto Tetris<COLUMNS, ROWS>::placePiece(const Piece* piece, Point position, PieceType pieceType) -> void {
  for (auto& offset : piece->body) {
    auto pos = position + offset;
    ASSERT(pos.x >= 0 && pos.x < COLUMNS, "Position x out of range");
    ASSERT(pos.y >= 0 && pos.y < ROWS, "Position y out of range");
    TRACE("Placing at %i, %i", pos.x, pos.y);
    board[pos.y][pos.x].type = pieceType;
  }
}

template<size_t COLUMNS, size_t ROWS>
auto Tetris<COLUMNS, ROWS>::canPlacePiece(const Piece* piece, Point position) -> bool {
  return std::all_of(piece->begin(), piece->end(), [&](auto offset){
    auto actualPosition = position + offset;
    return actualPosition.x >= 0
        && actualPosition.x < COLUMNS
        && actualPosition.y >= 0
        && actualPosition.y < ROWS
        && board[actualPosition.y][actualPosition.x].type == PieceType::EMPTY;
  });
}

template<size_t COLUMNS, size_t ROWS>
auto Tetris<COLUMNS, ROWS>::removePiece(const Piece* piece, Point position) -> void {
  for (auto& offset : piece->body) {
    auto pos = position + offset;
    ASSERT(pos.x >= 0 && pos.x < COLUMNS, "Position x out of range");
    ASSERT(pos.y >= 0 && pos.y < ROWS, "Position y out of range");
    board[pos.y][pos.x].type = PieceType::EMPTY;
  }
}

#endif //TETRISGAME_TETRIS_H
