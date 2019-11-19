#ifndef ENUMS_H
#define ENUMS_H

namespace sudoku
{
  enum SAVING
  {
    SAVE_PUZZLE,
    SAVE_PLAY
  };

  enum DRAWSTATE
  {
    DRAW = 0,
    NORMAL = 1,
    UPDATE = NORMAL,
    HIGHLIGHT = 2
  };
}

#endif

