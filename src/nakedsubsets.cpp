#include <iostream>
using namespace std;

#include "nakedsubsets.h"
#include "freedom.h"
#include "puzzle.h"
#include "sequence.h"
#include "square.h"

namespace sudoku
{

const FindNakedSubsets FindNakedSubsets::instance;

/*********************************************************************
 */
bool
FindNakedSubsets::isEnabled(const Puzzle *puzzle,
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
         (puzzle->getNakedSubsetSize() != 0);
}


/********************************************************************
 */
int
FindNakedSubsets::checkSubsets(Puzzle *puzzle,
                               unsigned highSubsetSize,
                               const Freedom &freedom) const
{
  TRACE( cout << "CHECKING FOR NAKED SUBSETS OF NUMBERS IN "
              << freedom.getSequence()->getName()
              << endl; )

  int totalChanges = 0;

  SubsetCollection subsets;
  if ( freedom.findNakedSubsets(highSubsetSize, subsets) )
  {
    Sequence *sequence = freedom.getSequence();

    const auto end = subsets.cend();
    auto subset = subsets.cbegin();

    while (subset != end)
    {
      int changes = 0;

      try
      {
        const Sequence::size_type size = sequence->size();

        for (Sequence::size_type i = 0; i < size; ++i)
        {
          Square *square = sequence->getSquare(i);

          if ( (square->getNumber() == 0) &&
               subset->getNumbers().isClear(i) &&
               square->forbid(subset->getSquares()) )
          {
            TRACE( cout << "Forbidding " << square->getName()
                        << " to be " << subset->getSquares()
                        << endl; )

            ++changes;
          }
        } /* for */

        if (changes != 0)
        {
          const unsigned subsize = subset->countNumbers();

          /*
           * We want to remember how large the biggest
           * naked subset that we have found so far is ...
           */
          if (subsize > puzzle->getLargestNakedSubsetSize())
            puzzle->setLargestNakedSubsetSize( subsize );

          puzzle->addNakedSubset(sequence, *subset);
          totalChanges += changes;
        }
      }
      catch(...)
      {
        puzzle->addNakedSubset(sequence, *subset);
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
FindNakedSubsets::check(Puzzle *puzzle, const Freedom &freedom) const
{
  return checkSubsets(
                    puzzle,
                    freedom.getSubsetSizeCeiling(puzzle->getNakedSubsetSize()),
                    freedom);
}


/********************************************************************
 */
int
FindNakedSubsets::update(Puzzle *puzzle, const TotalFreedom &total) const
{
  TRACE( cout << "UPDATING NAKED SUBSETS STRATEGY FROM "
              << puzzle->getNakedSubsetSize()
              << " SQUARES"
              << endl; )

  const unsigned maxSubsetSize = puzzle->getMaxNakedSubsetSize();
  const unsigned baseSubsetSize = puzzle->getNakedSubsetSize();
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
      puzzle->setNakedSubsetSize( puzzle->getLargestNakedSubsetSize() );
      TRACE( cout << "NAKED SUBSETS STRATEGY UPDATED TO "
                  << puzzle->getNakedSubsetSize()
                  << " SQUARES"
                  << endl; )
    }
  }

  return changes;
}

}

