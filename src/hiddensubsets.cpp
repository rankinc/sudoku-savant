#include <iostream>
using namespace std;

#include "hiddensubsets.h"
#include "freedom.h"
#include "puzzle.h"
#include "sequence.h"
#include "square.h"

namespace sudoku
{

const FindHiddenSubsets FindHiddenSubsets::instance;

/*********************************************************************
 */
bool
FindHiddenSubsets::isEnabled(const Puzzle *puzzle,
                             const Freedom &freedom) const
{
  /*
   * There is no point looking for subsets of two or more numbers
   * unless we have at least four squares left to populate, because
   * having only two squares already forms a subset, and a subset of
   * two numbers among three squares makes that third square into a
   * singlet.
   */
  return (freedom.getSequence()->countMissing() >= 4) &&
         (puzzle->getHiddenSubsetSize() != 0);
}


/*********************************************************************
 */
int
FindHiddenSubsets::checkSubsets(Puzzle *puzzle,
                                unsigned highSubsetSize,
                                const Freedom &freedom) const
{
  TRACE( cout << "CHECKING FOR HIDDEN SUBSETS OF NUMBERS IN "
              << freedom.getSequence()->getName()
              << endl; )

  int totalChanges = 0;

  SubsetCollection subsets;
  if ( freedom.findHiddenSubsets(highSubsetSize, subsets) )
  {
    const auto end = subsets.cend();
    auto subset = subsets.cbegin();

    Sequence *sequence = freedom.getSequence();
    const bitmask_t NUMBER_MASK = sequence->getNumberMask();

    while (subset != end)
    {
      const Bitmask inverse(
        Bitmask::createFromBits(~subset->getNumbers().getBits() & NUMBER_MASK)
      );
      int changes = 0;

      try
      {
        const Sequence::size_type size = sequence->size();
        for (Sequence::size_type i = 0; i < size; ++i)
        {
          if ( subset->getSquares().isSet(i) )
          {
            Square& square = (*sequence)[i];

            TRACE( if (square.getNumber() == 0); else abort(); )
                  
            if ( square.forbid(inverse) )
            {
              TRACE( cout << "Forbidding " << square.getName()
                          << " to be " << inverse
                          << endl; )

              ++changes;
            }
          }
        } /* for */

        if (changes != 0)
        {
          const unsigned subsize = subset->countSquares();

          /*
           * We want to remember how large the biggest
           * subset that we have found so far is ...
           */
          if (subsize > puzzle->getLargestHiddenSubsetSize())
            puzzle->setLargestHiddenSubsetSize( subsize );

          puzzle->addHiddenSubset(sequence, *subset);
          totalChanges += changes;
        }
      }
      catch(...)
      {
        puzzle->addHiddenSubset(sequence, *subset);
        throw;
      }

      ++subset;
    } /* while */
  }

  TRACE( cout << "DONE(" << highSubsetSize
              << "): changes=" << totalChanges << endl; )

  return totalChanges;
}

int
FindHiddenSubsets::check(Puzzle *puzzle, const Freedom &freedom) const
{
  return checkSubsets(puzzle,
                      freedom.getSubsetSizeCeiling(puzzle->getHiddenSubsetSize()),
                      freedom);
}


/*********************************************************************
 */
int
FindHiddenSubsets::update(Puzzle *puzzle, const TotalFreedom &total) const
{
  TRACE( cout << "UPDATING HIDDEN NUMBER SUBSETS STRATEGY FROM "
              << puzzle->getHiddenSubsetSize()
              << " SQUARES"
              << endl; )

  const unsigned maxSubsetSize = puzzle->getMaxHiddenSubsetSize();
  const unsigned baseSubsetSize = puzzle->getHiddenSubsetSize();
  int changes = 0;

  if (baseSubsetSize < maxSubsetSize)
  {
    changes = Modifier(this,
                       baseSubsetSize,
                       maxSubsetSize).checkAll(puzzle, total);

    /*
     * Adjust the algorithm to search for subsets no larger
     * than the largest that we have found so far ...
     */
    if (changes != 0)
    {
      puzzle->setHiddenSubsetSize( puzzle->getLargestHiddenSubsetSize() );
      TRACE( cout << "HIDDEN SUBSETS STRATEGY UPDATED TO "
                  << puzzle->getHiddenSubsetSize()
                  << " SQUARES"
                  << endl; )
    }
  }

  return changes;
}

}

