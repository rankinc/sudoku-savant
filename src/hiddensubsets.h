#ifndef HIDDEN_SUBSETS_H
#define HIDDEN_SUBSETS_H

#include "subsetstrategy.h"

namespace sudoku
{
  class FindHiddenSubsets : public SubsetStrategy
  {
  protected:
    FindHiddenSubsets() : SubsetStrategy() {}

    virtual int check(Puzzle*, const Freedom&) const;
    virtual int update(Puzzle*, const TotalFreedom&) const;
    virtual bool isEnabled(const Puzzle*, const Freedom&) const;

    virtual int checkSubsets(Puzzle*, unsigned, const Freedom&) const;

  public:
    static const FindHiddenSubsets instance;
  };
}

#endif

