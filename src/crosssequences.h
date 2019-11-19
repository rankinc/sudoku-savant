#ifndef CROSSSEQUENCES_H
#define CROSSSEQUENCES_H

#include "singleseqstrategy.h"

namespace sudoku
{
  class FindCrossSequenceConstraints : public SingleSequenceStrategy
  {
  private:
    FindCrossSequenceConstraints() : SingleSequenceStrategy() {}

    virtual int check(Puzzle*, const Freedom&) const;

  public:
    static const FindCrossSequenceConstraints instance;
  };
}

#endif

