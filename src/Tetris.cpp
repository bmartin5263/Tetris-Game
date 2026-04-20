//
// Created by Brandon on 6/1/25.
//

#include "Tetris.h"

auto Tetris::placeCurrentPiece() -> void {
  TRACE("PLACE CURRENT TETROMINO");
  if (ghostEnabled) {
    placePiece(currentPiece, ghostPosition, PieceType::GHOST);
  }
  placePiece(currentPiece, currentPosition);
}

auto Tetris::removeCurrentPiece() -> void {
  TRACE("REMOVE CURRENT TETROMINO");
  if (ghostEnabled) {
    removePiece(currentPiece, ghostPosition);
  }
  removePiece(currentPiece, currentPosition);
}

auto Tetris::toggleGhost() -> void {
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

auto Tetris::calculateGhostPosition() -> Tetris::Point {
  auto position = currentPosition;
  while (canPlacePiece(currentPiece, position)) {
    position.y += 1;
  }
  return position - Point {0, 1};
}

auto Tetris::getScore() -> const Score& {
  return score;
}

auto Tetris::dropPiece() -> MoveResult {
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

auto Tetris::newGame() -> void {
  TRACE("NEW GAME");
  std::for_each(board.begin(), board.end(), [](auto& row) {
    row.clear();
  });

  score.reset();
  gameOver = false;
  advanceNextPiece();
}

auto Tetris::calculateRowsToClear(MoveResult& result) -> void {
  for (size_t rowNum = 0; rowNum < ROWS; ++rowNum) {
    auto& row = board[rowNum];
    if (row.isFilled()) {
      result.addRowToClear(rowNum);
    }
  }
}

auto Tetris::clearRows(const std::array<size_t, 4>& rows, size_t count) -> void {
  if (count > 0) {
    score.points += POINT_VALUES[count - 1];
    score.clearedRows += count;
    score.combos[count - 1] += 1;
  }
  for (int i = 0; i < count; ++i) {
    clearRow(rows[i]);
  }
}

auto Tetris::clearRow(size_t rowNum) -> void {
  auto rowToDropNum = rowNum;
  while (rowToDropNum > 0) {
    board[rowToDropNum] = board[rowToDropNum - 1];
    --rowToDropNum;
  }
  board[0].clear();
}

auto Tetris::rotatePieceLeft() -> void {
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

auto Tetris::rotatePieceRight() -> void {
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

auto Tetris::movePiece(Point movement) -> MoveResult {
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

auto Tetris::advanceNextPiece() -> void {
  currentPosition = START_POSITION;
  currentPiece = nextPiece;
  nextPiece = Piece::Random();
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

auto Tetris::canPlaceCurrentPieceAtOffset(Point offset) -> bool {
  TRACE("CAN PLACE CURRENT AT OFFSET");
  auto pickUpCurrentPiece = PickUpCurrentPiece(*this);
  auto pos = currentPosition + offset;
  return canPlacePiece(currentPiece, pos);
}

auto Tetris::placePiece(const Piece* piece, Point position) -> void {
  placePiece(piece, position, piece->type);
}

auto Tetris::placePiece(const Piece* piece, Point position, PieceType pieceType) -> void {
  for (auto& offset : piece->body) {
    auto pos = position + offset;
    ASSERT(pos.x >= 0 && pos.x < COLUMNS, "Position x out of range");
    ASSERT(pos.y >= 0 && pos.y < ROWS, "Position y out of range");
    TRACE("Placing at %i, %i", pos.x, pos.y);
    board[pos.y][pos.x].type = pieceType;
  }
}

auto Tetris::canPlacePiece(const Piece* piece, Point position) -> bool {
  return std::all_of(piece->begin(), piece->end(), [&](auto offset){
    auto actualPosition = position + offset;
    return actualPosition.x >= 0
        && actualPosition.x < COLUMNS
        && actualPosition.y >= 0
        && actualPosition.y < ROWS
        && board[actualPosition.y][actualPosition.x].type == PieceType::EMPTY;
  });
}

auto Tetris::removePiece(const Piece* piece, Point position) -> void {
  for (auto& offset : piece->body) {
    auto pos = position + offset;
    ASSERT(pos.x >= 0 && pos.x < COLUMNS, "Position x out of range");
    ASSERT(pos.y >= 0 && pos.y < ROWS, "Position y out of range");
    board[pos.y][pos.x].type = PieceType::EMPTY;
  }
}
