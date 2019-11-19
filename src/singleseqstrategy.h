#ifndef SINGLESEQSTRATEGY_H
#define SINGLESEQSTRATEGY_H

#include "strategy.h"

namespace sudoku
{
  class SingleSequenceStrategy : public Strategy
  {
  protected:
    SingleSequenceStrategy(bool expensive=false) : Strategy(expensive) {}

    virtual int checkAll(Puzzle*, const TotalFreedom&) const;
    virtual bool isEnabled(const Puzzle*, const Freedom&) const;
  };
}

#endif

