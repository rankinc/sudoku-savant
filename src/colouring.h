#ifndef COLOURING_H
#define COLOURING_H

#include "multiseqstrategy.h"

namespace sudoku
{
  class UseColouring : public MultiSequenceStrategy
  {
  private:
    UseColouring() : MultiSequenceStrategy(true) {}

    bool isAllEnabled(const Puzzle*) const;
    int checkAll(Puzzle*, const TotalFreedom&) const;

  public:
    static const UseColouring instance;
  };
}

#endif

