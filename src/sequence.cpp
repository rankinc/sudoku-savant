#include <algorithm>
using namespace std;

#include "sequence.h"
#include "square.h"
#include "solution.h"
#include "actions.h"

extern "C"
{
#  include "support.h"
}

namespace sudoku
{

/*********************************************************************
 */
Sequence::Sequence(const Geometry &geometry)
  : m_squares(geometry.getMaxNumber())
  , m_missing(Bitmask::createNumberMask(geometry.getMaxNumber()))
  , m_type(Geometry::UNKNOWN)
{}

Sequence::~Sequence() {}


/*********************************************************************
 */
void
Sequence::setProperties(Type t, const string &name)
{
  setName(name);
  setType(t);
}


/*********************************************************************
 */
void
Sequence::forAllSquares(SquareFunc f, unsigned number)
{
  for_each(getSquares().begin(),
           getSquares().end(),
           BoolAct<Square>(f, number));
}


/*********************************************************************
 */
bool
Sequence::add(unsigned number)
{
  const bool isChanging = m_missing.clearBit(number);

  if (isChanging)
  {
    forAllSquares(&Square::forbid, number);
  }

  return isChanging;
}


/*********************************************************************
 */
bool
Sequence::add_nothrow(unsigned number)
{
  const bool isChanging = m_missing.clearBit(number);

  if (isChanging)
  {
    forAllSquares(&Square::forbid_nothrow, number);
  }

  return isChanging;
}


/*********************************************************************
 */
bool
Sequence::remove(unsigned number)
{
  const bool isChanging = m_missing.setBit(number);

  if (isChanging)
  {
    forAllSquares(&Square::allow, number);
  }

  return isChanging;
}


/*********************************************************************
 */
void
Sequence::push()
{
  getStack().push_back( getMissing() );
}


/*********************************************************************
 */
bool
Sequence::pop()
{
  StateStack& stack = getStack();
  bool ok;

  if ( (ok = !stack.empty()) )
  {
    m_missing = stack.back();
    stack.pop_back();
  }

  return ok;
}


/*********************************************************************
 */
void
Sequence::clear()
{
  m_missing = Bitmask::createNumberMask(m_squares.size());
  dropStack();
}


/*********************************************************************
 */
Sequence::size_type
Sequence::getIndex() const
{
  const Square& square0 = (*this)[0];

  switch(getType())
  {
  case Geometry::ROW:
    return square0.getRowIndex();

  case Geometry::COLUMN:
    return square0.getColumnIndex();

  default:
    break;
  };

  return square0.getBoxIndex();
}


/*********************************************************************
 */
Sequence*
Sequence::findCrossSequence(const Bitmask &bitmask) const
{
  Sequence *sequence = NULL;

  struct CrossRef
  {
    Sequence *a;
    Sequence *b;

    CrossRef(const Sequence *seq, Square *s)
    {
      if ( (a = s->getRow()) == seq )
        a = s->getBox();

      if ( (b = s->getColumn()) == seq )
        b = s->getBox();
    }

    void update(const CrossRef &ref)
    {
      if ((a != ref.a) && (a != ref.b))
        a = NULL;

      if ((b != ref.a) && (b != ref.b))
        b = NULL;
    }

    Sequence* __attribute__((pure)) getCrossSequence() const { return ((a != NULL) ? a : b); }
  };

  if (bitmask.size() > 1)
  {
    unsigned i = bitmask.getHighestBit();

    CrossRef crossRef(this, getSquares()[i]);

    do
    {
      if (bitmask.isSet(--i))
      {
        crossRef.update( CrossRef(this, getSquares()[i]) );
      }
    }
    while (i != 0);

    sequence = crossRef.getCrossSequence();
  }

  return sequence;
}

/*********************************************************************
 */
Square*
Sequence::getSquare(size_type i, SquareSequence &sseq)
{
  const size_type size = sseq.size();

  if ((size != 0) && (i >= size))
    i = size - 1;

  return sseq[i];
}

}


namespace std
{

/*********************************************************************
 */
ostream&
operator<<(ostream &ostr, const sudoku::Sequence &s)
{
  const sudoku::SquareSequence::const_iterator end = s.getSquares().cend();
  sudoku::SquareSequence::const_iterator iter = s.getSquares().cbegin();
  unsigned i = 0;

  while (iter != end)
  {
    ostr << (*iter)->getName() << ':'
         << i << ':'
         << sudoku::Bitmask::createFromBits( (*iter)->getForbidden() )
         << endl;
    ++i;
    ++iter;
  } /* while */

  return ostr;
}


/*********************************************************************
 */
istream&
operator>>(istream &istr, sudoku::Sequence &s)
{
  unsigned u;
  char c;

  for (sudoku::Sequence::size_type i = 0; i < s.size(); ++i)
  {
    istr >> ws >> u;

    if ( istr && (u != 0) )
    {
      s[i] = sudoku::Square::FIXED(u);
    }

    istr.clear();
    istr >> ws >> c;
  } /* for */

  if ( !istr.eof() )
  {
    throw runtime_error(_("Too many numbers!"));
  }

  return istr;
}

}

