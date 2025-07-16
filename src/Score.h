//
// Created by Brandon on 6/7/25.
//

#ifndef TETRISGAME_SCORE_H
#define TETRISGAME_SCORE_H

#include <array>

struct Score {
  unsigned int clearedRows{};
  unsigned int points{};
  std::array<unsigned int, 4> combos{};

  auto reset() {
    clearedRows = 0;
    points = 0;
    combos.fill(0);
  }
};

#endif //TETRISGAME_SCORE_H
