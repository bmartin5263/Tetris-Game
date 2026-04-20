//
// Created by Brandon on 6/1/25.
//

#ifndef TETRISGAME_TETRIS_H
#define TETRISGAME_TETRIS_H

#include "Piece.h"
#include "Types.h"
#include "Assertions.h"
#include "Score.h"
#include "MoveResult.h"
#include "Iterable.h"
#include "Config.h"

struct Cell {
  PieceType type{PieceType::EMPTY};
  bool destroying{false};
};

struct Row {
  std::array<Cell, GAME_COLUMN_COUNT> data;

  auto operator[](size_t index) -> Cell& { return data[index]; }
  auto begin() { return data.begin(); }
  auto end() { return data.end(); }
  auto clear() -> void { data.fill(Cell{}); }

  auto isFilled() -> bool {
    auto isNotEmpty = [](const Cell& c) { return c.type != PieceType::EMPTY; };
    return std::all_of(data.begin(), data.end(), isNotEmpty);
  }
};

class Tetris {
  constexpr static auto COLUMNS = static_cast<size_t>(GAME_COLUMN_COUNT);
  constexpr static auto ROWS = static_cast<size_t>(ROW_COUNT);

  using Point = rgb::Point;
  using Board = std::array<Row, ROWS>;

public:
  constexpr static auto START_POSITION = rgb::Point { static_cast<int>(COLUMNS / 2) - 1, 1 };
  constexpr static auto POINT_VALUES = std::array { 100, 300, 1200, 3600 };

  auto newGame() -> void;
  auto advanceNextPiece() -> void;
  auto clearRows(const std::array<size_t, 4>& rows, size_t count) -> void;

  auto movePiece(Point movement) -> MoveResult;
  auto dropPiece() -> MoveResult;
  auto rotatePieceLeft() -> void;
  auto rotatePieceRight() -> void;
  auto getScore() -> const Score&;
  auto toggleGhost() -> void;
  constexpr auto isGameOver() const -> bool { return gameOver; }

  static auto columnCount() -> size_t { return COLUMNS; }

  Board board{PieceType::EMPTY};
  const Piece* currentPiece{&Piece::O};
  const Piece* nextPiece{&Piece::O};
private:
  Point currentPosition{START_POSITION};
  Point ghostPosition{START_POSITION};
  Score score{};
  bool gameOver{false};
  bool ghostEnabled{false};

  auto placeCurrentPiece() -> void;
  auto removeCurrentPiece() -> void;
  auto calculateGhostPosition() -> Point;
  auto placePiece(const Piece* piece, Point position) -> void;
  auto placePiece(const Piece* piece, Point position, PieceType type) -> void;
  auto removePiece(const Piece* piece, Point position) -> void;
  auto canPlacePiece(const Piece* piece, Point position) -> bool;
  auto canPlaceCurrentPieceAtOffset(Point offset) -> bool;
  auto clearRow(size_t rowNum) -> void;
  auto calculateRowsToClear(MoveResult& result) -> void;

  struct PickUpCurrentPiece {
    Tetris& tetris;
    explicit PickUpCurrentPiece(Tetris& tetris): tetris(tetris) {
      tetris.removeCurrentPiece();
    }

    ~PickUpCurrentPiece() {
      tetris.placeCurrentPiece();
    }
  };
};

#endif //TETRISGAME_TETRIS_H
