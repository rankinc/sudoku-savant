#ifndef MULTISEQPATTERNS_H
#define MULTISEQPATTERNS_H

#include "multiseqstrategy.h"

namespace sudoku
{
  typedef std::list<const Freedom*> WorkFreedom;

  class FindMultiSequencePatterns : public MultiSequenceStrategy
  {
  private:
    FindMultiSequencePatterns() : MultiSequenceStrategy() {}

    static int checkPatterns(Puzzle*, unsigned, const TotalFreedom&);
    static void eliminateReflections(SubsetCollection&, SubsetCollection&);
    static int enforceSymmetry(Puzzle*,
                               unsigned,
                               const Subset&,
                               const WorkFreedom&);
    static unsigned getSmallestPatternSize(const SubsetCollection&);
    static unsigned getLargestPatternSize(const SubsetCollection&);

    static bool forbid(Square&, unsigned);

    static unsigned max(unsigned a, unsigned b) __attribute__((const)) {
        return (a > b ? a : b);
    }

    static unsigned min(unsigned a, unsigned b) __attribute__((const)) {
        return (a < b ? a : b);
    }

    virtual bool isAllEnabled(const Puzzle*) const;
    virtual int checkAll(Puzzle*, const TotalFreedom&) const;
    virtual int update(Puzzle*, const TotalFreedom&) const;

  public:
    static const FindMultiSequencePatterns instance;
  };
}

#endif

