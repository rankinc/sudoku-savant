#include <cstring>
#include "freedom.h"
#include "sequence.h"
#include "workset.h"
#include "square.h"

#include <iostream>
using namespace std;


namespace sudoku
{

/*********************************************************************
 */
Freedom::Freedom(Sequence *s)
  : m_size(s->getMaxNumber() + 1)	
  , m_bitmask( new Bitmask[m_size] )
  , m_sequence(s)
{
  calculate();
}


Freedom::Freedom(const Freedom &f)
  : m_size(f.m_size)
  , m_bitmask( new Bitmask[m_size] )
  , m_sequence(f.m_sequence)
{
  memcpy(m_bitmask, f.m_bitmask, sizeof(Bitmask) * m_size);
}


Freedom::~Freedom()
{
  delete[] m_bitmask;
}


/*********************************************************************
 */
void
Freedom::calculate()
{
  const Sequence *sequence = getSequence();

  const SquareSequence::const_iterator endSquare = sequence->getSquares().cend();
  SquareSequence::const_iterator sqiter = sequence->getSquares().cbegin();
  unsigned sq = 0;

  TRACE( cout << "Getting freedoms for sequence "
              << sequence->getName()
              << endl; )

  while (sqiter != endSquare)
  {
    const Square *square = *sqiter;

    const bitmask_t forbiddenMask = square->getForbidden();
    bitmask_t numberMask = 1;

    for (size_type n = 1; n < size(); ++n)
    {
      numberMask <<= 1;
      if ( ((forbiddenMask & numberMask) == 0) &&
           (square->getNumber() == 0) )
      {
        m_bitmask[n].setBit(sq);
      }
    } /* for */

    ++sq;
    ++sqiter;
  } /* while */

  TRACE( cout << (*this); )
}


/*********************************************************************
 */
Freedom::size_type
Freedom::getSubsetSizeCeiling(size_type sz) const
{
  const size_type missing = getSequence()->countMissing();

  if ( sz > missing - 2 )
    sz = missing - 2;

  return sz;
}


/*********************************************************************
 */
bool
Freedom::findHiddenSubsets(unsigned highSize, SubsetCollection &subsets) const
{
  WorkSet work(highSize, *this);
  return !work.findSubsets(subsets).empty();
}


/*********************************************************************
 */
bool
Freedom::findNakedSubsets(unsigned highSize, SubsetCollection &subsets) const
{
  WorkSet work(highSize, getSequence());
  return !work.findSubsets(subsets).empty();
}

}


/*********************************************************************
 */
namespace std
{

ostream&
operator<<(ostream &ostr, const sudoku::Freedom &f)
{
  for (sudoku::Freedom::size_type i = 1; i < f.size(); ++i)
  {
    ostr << "- put " << i << " in " << f[i] << endl;
  } /* for */

  return ostr;
}

}

