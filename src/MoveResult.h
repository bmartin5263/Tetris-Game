//
// Created by Brandon on 6/7/25.
//

#ifndef TETRISGAME_MOVERESULT_H
#define TETRISGAME_MOVERESULT_H

#include "array"

struct MoveResult {
  size_t rowsCleared{0};
  std::array<size_t, 4> rowNumbers{};
  bool didMove{false};
  bool nextPiece{false};

  auto addRowToClear(size_t rowNum) {
    rowNumbers[rowsCleared++] = rowNum;
  }
};

#endif //TETRISGAME_MOVERESULT_H
