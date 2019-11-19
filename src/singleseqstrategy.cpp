#include "singleseqstrategy.h"
#include "puzzle.h"


namespace sudoku
{

/*********************************************************************
 */
bool
SingleSequenceStrategy::isEnabled(const Puzzle*, const Freedom&) const
{
  return true;
}


/*********************************************************************
 */
int
SingleSequenceStrategy::checkAll(Puzzle *puzzle,
                                 const TotalFreedom &total) const
{
  return puzzle->reprocessSequenceStrategy(*this, total);
}

}

