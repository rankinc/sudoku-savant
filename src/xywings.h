#ifndef XYWINGS_H
#define XYWINGS_H

#include "multiseqstrategy.h"

namespace sudoku
{
  class FindXYWings : public MultiSequenceStrategy
  {
  private:
    FindXYWings() : MultiSequenceStrategy(true) {}

    virtual int checkAll(Puzzle*, const TotalFreedom&) const;
    virtual bool isAllEnabled(const Puzzle*) const;

  public:
    static const FindXYWings instance;
  };
}

#endif

