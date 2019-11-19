#ifndef XYZWINGS_H
#define XYZWINGS_H

#include "multiseqstrategy.h"

namespace sudoku
{
  class FindXYZWings : public MultiSequenceStrategy
  {
  private:
    FindXYZWings() : MultiSequenceStrategy(true) {}

    virtual int checkAll(Puzzle*, const TotalFreedom&) const;
    virtual bool isAllEnabled(const Puzzle*) const;

  public:
    static const FindXYZWings instance;
  };
}

#endif

