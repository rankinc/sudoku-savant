#include <iostream>
using namespace std;

#include "crosssequences.h"
#include "freedom.h"
#include "puzzle.h"
#include "sequence.h"
#include "square.h"

namespace sudoku
{

const FindCrossSequenceConstraints FindCrossSequenceConstraints::instance;

/*********************************************************************
 */
int
FindCrossSequenceConstraints::check(Puzzle *puzzle,
                                    const Freedom &freedom) const
{
  TRACE( cout << "CHECKING FOR CROSS-SEQUENCE CONSTRAINTS ON "
              << freedom.getSequence()->getName()
              << endl; )

  const Sequence *sequence = freedom.getSequence();
  int changes = 0;

  SquareCollection exclusions;

  for (Freedom::size_type n = 1; n < freedom.size(); ++n)
  {
    Sequence *cross = sequence->findCrossSequence(freedom[n]);
    if ((cross != NULL) && cross->isMissing(n))
    {
      for (auto& square : cross->getSquares()) {

        try
        {
          if ( (square->getNumber() == 0) &&
               !square->belongsTo(sequence) &&
               square->forbid(n) )
          {
            exclusions.push_back(square);
          }
        }
        catch(...)
        {
          /*
           * Oops! We have reached a contradiction! Make sure
           * that we preserve our impossible move so that we
           * can review it later ...
           */
          exclusions.push_back(square);
          puzzle->addCrossSequenceConstraint(sequence, cross, n, exclusions);
          throw;
        }

      } /* for */

      /*
       * Did these two sequences exclude any candidates?
       */
      if ( !exclusions.empty() )
      {
        puzzle->addCrossSequenceConstraint(sequence, cross, n, exclusions);
        changes += exclusions.size();
        exclusions.clear();
      }
    }
  } /* for */

  TRACE( cout << "DONE: changes=" << changes << endl; )

  return changes;
}

}

