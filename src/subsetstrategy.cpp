#include <iostream>
using namespace std;

#include "subsetstrategy.h"
#include "freedom.h"
#include "puzzle.h"
#include "sequence.h"

namespace sudoku
{

/********************************************************************
 */
SubsetStrategy::Modifier::Modifier(const SubsetStrategy *strategy,
                                   unsigned sz,
                                   unsigned msz)
  : SingleSequenceStrategy(true)
  , m_strategy(strategy)
  , m_subsetSize(sz)
  , m_maxSubsetSize(msz)
{}


/********************************************************************
 */
int
SubsetStrategy::Modifier::check(Puzzle *puzzle, const Freedom &freedom) const
{
  const unsigned highSize = freedom.getSubsetSizeCeiling(getMaxSubsetSize());

  /*
   * We are "strengthening" this strategy by checking for bigger
   * subsets, so we need only consider searching for sequences
   * that can actually contain bigger subsets ...
   */
  if (highSize <= getSubsetSize())
  {
    TRACE( cout << "ABORT SUBSETS FOR " << freedom.getSequence()->getName()
                << ": NOTHING NEW TO LEARN FOR SUBSETS OF UP TO "
                << highSize
                << " SQUARES"
                << endl; )
    return 0;
  }

  return getStrategy()->checkSubsets(puzzle, highSize, freedom);
}

}

