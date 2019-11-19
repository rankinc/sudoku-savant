#ifndef SUBSET_STRATEGY_H
#define SUBSET_STRATEGY_H

#include "singleseqstrategy.h"

namespace sudoku
{
  class FindHiddenSubsets;
  class FindNakedSubsets;

  class SubsetStrategy : public SingleSequenceStrategy
  {
  protected:
    SubsetStrategy(bool expensive=false) : SingleSequenceStrategy(expensive) {}

    virtual int checkSubsets(Puzzle*, unsigned, const Freedom&) const = 0;

    class Modifier : public SingleSequenceStrategy
    {
    private:
      const SubsetStrategy* m_strategy;
      unsigned              m_subsetSize;
      unsigned              m_maxSubsetSize;

    public:
      Modifier(const SubsetStrategy*, unsigned, unsigned);

      const SubsetStrategy* getStrategy() const { return m_strategy; }
      unsigned getSubsetSize() const { return m_subsetSize; }
      unsigned getMaxSubsetSize() const { return m_maxSubsetSize; }

      virtual int check(Puzzle*, const Freedom&) const;

      friend class FindHiddenSubsets;
      friend class FindNakedSubsets;
    };
  };
}

#endif

