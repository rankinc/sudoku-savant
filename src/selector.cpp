#include "selector.h"
#include "board.h"
#include "square.h"

namespace sudoku
{

/*********************************************************************
 */
Selector::~Selector() {}


/*********************************************************************
 */
unsigned
FirstAvailableNumber::operator()(const Square *square, unsigned n) const
{
  const bitmask_t bits = square->getForbidden();

  if (n == 0)
    n = square->getMaxNumber();

  bitmask_t mask = (1 << n);

  while ( (n != 0) && ((bits & mask) != 0) )
  {
    mask >>= 1;
    --n;
  } /* while */

  return n;
}


/*********************************************************************
 */
unsigned
RandomAvailableNumber::operator()(const Square *square, unsigned n) const
{
  const size_t size = square->getMaxNumber();

  auto_array_ptr<unsigned> a( generateRandomArray(size, 1) );
  unsigned *array = a.get();

  const bitmask_t bits = square->getForbidden();
  n = 0;

  for (unsigned i = 0; i < size; ++i)
  {
    const unsigned j = array[i];    
    if ( (bits & (1 << j)) == 0 )
    {
      n = j;
      break;
    }
  } /* for */

  return n;
}


/*********************************************************************
 */
unsigned
CopyFromSolution::operator()(const Square *square, unsigned n) const
{
  const unsigned rowIndex = square->getRowIndex();
  const unsigned columnIndex = square->getColumnIndex();

  n = getSolution()[rowIndex][columnIndex].getNumber();
  getBoard()[rowIndex][columnIndex].setNumber(n);

  return n;
}

}

