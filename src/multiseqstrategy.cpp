#include <iostream>
using namespace std;

#include "multiseqstrategy.h"
#include "sequence.h"
#include "square.h"

namespace sudoku
{

/*********************************************************************
 */
bool
MultiSequenceStrategy::isEnabled(const Puzzle*, const Freedom&) const
{
  return false;
}


/*********************************************************************
 */
int
MultiSequenceStrategy::check(Puzzle*, const Freedom&) const
{
  return 0;
}


/*********************************************************************
 */
bitmask_t
MultiSequenceStrategy::getCommonSequences(const Square *sq1, const Square *sq2)
{
  bitmask_t common = 0;

  if (sq1->getRow() == sq2->getRow())
    common |= (1 << Geometry::ROW);

  if (sq1->getColumn() == sq2->getColumn())
    common |= (1 << Geometry::COLUMN);

  if (sq1->getBox() == sq2->getBox())
    common |= (1 << Geometry::BOX);

  return common;
}


/*********************************************************************
 */
int
MultiSequenceStrategy::forbidAtIntersection(Sequence *seq1,
                                            Sequence *seq2,
                                            const Square *exclude,
                                            const Bitmask &b,
                                            SquareCollection &squares)
{
  int changes = 0;

  const Sequence::size_type MAX_NUMBER = seq1->size();
  for (Sequence::size_type i = 0; i < MAX_NUMBER; ++i)
  {
    Square &square = (*seq1)[i];
    if ( (exclude != &square) &&
         (square.getNumber() == 0) &&
         square.belongsTo(seq2) &&
         square.forbid(b) )
    {
      TRACE( cout << "Forbid " << b
                  << " from " << square.getName()
                  << ", which shares " << seq1->getName()
                  << " and " << seq2->getName()
                  << endl; )
      squares.push_back( &square );
      ++changes;
    }
  } /* for */

  return changes;
}

}

