#include <cstring>
#include "freedom.h"
#include "board.h"
using namespace std;


namespace sudoku
{

/*********************************************************************
 */
bool
validate(const Board &board, Bitmask &invalids)
{
  const Board::size_type MAX_NUMBER = board.getMaxNumber();

  auto_array_ptr<Bitmask> position(new Bitmask[MAX_NUMBER + 1]);
  auto_array_ptr<unsigned> count(new unsigned[MAX_NUMBER + 1]);

  memset(count.get(), 0, sizeof(count[0]) * (MAX_NUMBER + 1));

  for (Board::size_type i = 0; i < MAX_NUMBER; ++i)
  {
    const Freedom f( const_cast<Sequence*>(&(board.getRow(i))) );
    const Sequence *sequence = f.getSequence();

    for (Freedom::size_type j = MAX_NUMBER; j != 0; --j)
    {
      if ( sequence->isMissing(j) )
      {
        position[j] |= f[j];
        ++(count[j]);
      }
    } /* for */
  } /* for */

  /*
   * Now check that there are enough positions on this
   * board to place all of the numbers that are missing.
   * This MUST be true for every valid board!
   */
  for (Board::size_type i = MAX_NUMBER; i != 0; --i)
  {
    if ( count[i] != position[i].size() )
      invalids.setBit(i);
  } /* for */

  return invalids.empty();
}

}

