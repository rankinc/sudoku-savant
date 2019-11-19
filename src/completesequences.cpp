#include <iostream>
using namespace std;

#include "completesequences.h"
#include "freedom.h"
#include "puzzle.h"
#include "sequence.h"
#include "square.h"

namespace sudoku
{

const FindCompleteSequences FindCompleteSequences::instance;

/*********************************************************************
 */
int
FindCompleteSequences::check(Puzzle *puzzle, const Freedom &freedom) const
{
  TRACE( cout << "COMPLETING SEQUENCES FOR "
              << freedom.getSequence()->getName()
              << endl; )

  Sequence *sequence = freedom.getSequence();
  int changes = 0;

  for (Freedom::size_type n = 1; n < freedom.size(); ++n)
  {
    if (freedom[n].size() == 1)
    {
      const unsigned sqIndex = freedom[n].getHighestBit();
      Square *square = &(*sequence)[sqIndex];

      if (square->getNumber() == 0)
      {
        puzzle->addCompletedSequence(square, sequence, n);
        square->setNumber(n);
        ++changes;

        TRACE( cout << "Number " << n
                    << " must be placed in square " << freedom[n]
                    << endl; )
      }
    }
  } /* for */

  TRACE( cout << "DONE: changes=" << changes << endl; )

  return changes;
}

}

