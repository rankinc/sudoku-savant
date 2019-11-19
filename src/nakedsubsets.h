#ifndef NAKED_SUBSETS_H
#define NAKED_SUBSETS_H

#include "subsetstrategy.h"

namespace sudoku
{
  class FindNakedSubsets : public SubsetStrategy
  {
  protected:
    FindNakedSubsets() : SubsetStrategy() {}

    virtual int check(Puzzle*, const Freedom&) const;
    virtual int update(Puzzle*, const TotalFreedom&) const;
    virtual bool isEnabled(const Puzzle*, const Freedom&) const;

    virtual int checkSubsets(Puzzle*, unsigned, const Freedom&) const;

  public:
    static const FindNakedSubsets instance;
  };
}

#endif

