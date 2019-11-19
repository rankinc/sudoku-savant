#ifndef COMPLETESEQUENCES_H
#define COMPLETESEQUENCES_H

#include "singleseqstrategy.h"

namespace sudoku
{
  class FindCompleteSequences : public SingleSequenceStrategy
  {
  private:
    FindCompleteSequences() : SingleSequenceStrategy() {}

    virtual int check(Puzzle*, const Freedom&) const;

  public:
    static const FindCompleteSequences instance;
  };
}

#endif

