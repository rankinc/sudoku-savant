#ifndef MULTISEQSTRATEGY_H
#define MULTISEQSTRATEGY_H

#include "strategy.h"

namespace sudoku
{
  class Square;
  class Sequence;
  class Bitmask;

  class MultiSequenceStrategy : public Strategy
  {
  protected:
    MultiSequenceStrategy(bool expensive=false) : Strategy(expensive) {}

    virtual int check(Puzzle*, const Freedom&) const;
    virtual bool isEnabled(const Puzzle*, const Freedom&) const;

  public:
    static bitmask_t getCommonSequences(const Square*, const Square*);
    static int forbidAtIntersection(Sequence*,
                                    Sequence*,
                                    const Square*,
                                    const Bitmask&,
                                    SquareCollection&);
  };
}

#endif

