#include <cstdlib>
#include <sstream>
#include <iostream>
using namespace std;

#include "square.h"
#include "sequence.h"

extern "C"
{
#  include "support.h"
}

namespace sudoku
{

Square::~Square() {}


/*********************************************************************
 */
string
Square::makeName(const Geometry &geometry, unsigned sq)
{
  ostringstream ostr;
  ostr << _("Square") << "(" << geometry.getRowIndex(sq)
       << "," << geometry.getColumnIndex(sq)
       << ")";
  return ostr.str();
}


/*********************************************************************
 */
void
Square::State::clear(const Geometry &geometry)
{
  number = forbidden = 0;
  possibilities = geometry.getMaxNumber();
}


/*********************************************************************
 */
void
Square::State::set(const State &s)
{
  number = s.number;
  forbidden = s.forbidden;
  possibilities = s.possibilities;
}


/*********************************************************************
 */
Square::Square(const Geometry &geometry, unsigned sq)
  : m_index(sq)
  , m_name(makeName(geometry, sq))
  , m_geometry(geometry)
  , m_tag(0)
  , m_state(geometry)
  , m_row(NULL)
  , m_column(NULL)
  , m_box(NULL)
{} 


/*********************************************************************
 */
void
Square::forbidAll()
{
  m_state.forbidden = m_geometry.getNumberMask();
  m_state.possibilities = 0;
}


/*********************************************************************
 * This number is no longer to be considered "missing" from this
 * square's row, column and box. However, each of these 3 sequences
 * could still throw a violation exception. (So there is still some
 * checking happening here.)
 */
void
Square::setNumber_checkSeqs(unsigned newNumber)
{
  m_state.number = newNumber;

  getRow()->add(newNumber);
  getColumn()->add(newNumber);
  getBox()->add(newNumber);

  TRACE( if (m_state.possibilities == m_geometry.getMaxNumber() - Bitmask::countBits(m_state.forbidden)) ;
         else abort(); )
}


/*********************************************************************
 * We want to call this function when solving a grid ourselves.
 * Otherwise we'd get too much help ... ;-).
 */
void
Square::setNumber_nothrow(unsigned newNumber)
{
  m_state.number = newNumber;

  getRow()->add_nothrow(newNumber);
  getColumn()->add_nothrow(newNumber);
  getBox()->add_nothrow(newNumber);

  TRACE( if (m_state.possibilities == m_geometry.getMaxNumber() - Bitmask::countBits(m_state.forbidden)) ;
         else abort(); )
}


/*********************************************************************
 */
void
Square::copyState(const Square &s)
{
  m_state.set(s.getState());
  setNumber_checkSeqs(s.getNumber());
}


/*********************************************************************
 * Basic validation: we ensure that this square is still empty, and
 * that this number isn't forbidden to go into it. If that all checks
 * out then we start altering the board's state.
 */
bool
Square::setNumber_validator(SetterFn setter, unsigned newNumber)
{
  const bool isChanging = (getNumber() == 0);

  if (isChanging)
  {
    if ( isSet(m_state.forbidden, newNumber) )
    {
      ostringstream ostr;

      ostr << _("Setting ") << getName()
           << _(" to ") << newNumber
           << _(" would violate the board");
      throw violation(this, ostr.str());
    }

    (this->*setter)(newNumber);
  }

  return isChanging;
}


/*********************************************************************
 */
bool
Square::clearNumber()
{
  const unsigned oldNumber = getNumber();
  const bool isChanging = (oldNumber != 0);

  if (isChanging)
  {
    m_state.number = 0;

    getRow()->remove(oldNumber);
    getColumn()->remove(oldNumber);
    getBox()->remove(oldNumber);
  }

  return isChanging;
}


/*********************************************************************
 */
bool
Square::forbid_nothrow(unsigned number)
{
  const bool isChanging = !isSet(m_state.forbidden, number);

  if (isChanging)
  {
    setBit(&m_state.forbidden, number);
    --m_state.possibilities;

    TRACE( if (m_state.possibilities <= m_geometry.getMaxNumber()) ;
           else abort(); )
  }

  return isChanging;
}


/*********************************************************************
 */
bool
Square::forbid(unsigned number)
{
  const bool isChanging = forbid_nothrow(number);

  /*
   * Have we just rendered our board insoluble?
   */
  if (isChanging && (m_state.possibilities == 0) && (getNumber() == 0))
  {
    ostringstream ostr;

    ostr << _("Forbidding ") << getName()
         << _(" to be ") << number
         << _(" violates the board");
    throw violation(this, ostr.str());
  }

  return isChanging;
}


/*********************************************************************
 */
bool
Square::forbid_nothrow(bitmask_t bits)
{
  const bitmask_t oldForbidden = getForbidden();
  const bool isChanging = ((m_state.forbidden |= bits) != oldForbidden);

  if (isChanging) 
  {
    m_state.possibilities
        = m_geometry.getMaxNumber() - Bitmask::countBits(m_state.forbidden);

    TRACE( if (m_state.possibilities <= m_geometry.getMaxNumber()) ;
           else abort(); )
  }

  return isChanging;
}


/*********************************************************************
 */
bool
Square::forbid(const Bitmask &b)
{
  const bool isChanging = forbid_nothrow(b.getBits());

  /*
   * Have we just rendered our board insoluble?
   */
  if (isChanging && (m_state.possibilities == 0) && (getNumber() == 0))
  {
    ostringstream ostr;

    ostr << _("Forbidding ") << getName()
         << _(" to be ") << b
         << _(" violates the board");
    throw violation(this, ostr.str());
  }

  return isChanging;
}


/*********************************************************************
 */
bool
Square::allow(unsigned number)
{
  bool isChanging = false;

  TRACE( cout << "Missing from ["
              << (getRow()->isMissing(number) ? "row," : "")
              << (getColumn()->isMissing(number) ? "column," : "")
              << (getBox()->isMissing(number) ? "box" : "")
              << "]: Is set? "
              << (isSet(m_state.forbidden, number) ? 'Y' : 'N')
              << endl; )

  if ( getRow()->isMissing(number) &&
       getColumn()->isMissing(number) &&
       getBox()->isMissing(number) &&
       isSet(m_state.forbidden, number) )
  {
    clearBit(&m_state.forbidden, number);
    ++m_state.possibilities;
    isChanging = true;

    TRACE( if (m_state.possibilities <= m_geometry.getMaxNumber()) ;
           else abort(); )
  }

  return isChanging;
}


/*********************************************************************
 */
bool
Square::belongsTo(const Sequence *seq) const
{
  return (getRow() == seq) || (getColumn() == seq) || (getBox() == seq);
}


/*********************************************************************
 */
void
Square::push()
{
  getStack().push_back( getState() );
}


/*********************************************************************
 */
bool
Square::pop()
{
  StateStack& stack = getStack();
  bool ok;

  if ( (ok = !stack.empty()) )
  {
    m_state = stack.back();
    stack.pop_back();
  }

  return ok;
}


/*********************************************************************
 */
void
Square::clear()
{
  getState().clear(getGeometry());
  dropStack();
}


/*********************************************************************
 */
void
Square::revertNumber()
{
  if ( clearNumber() )
  {
    bitmask_t allowed = ( getBox()->getMissing().getBits() &
                          getColumn()->getMissing().getBits() &
                          getRow()->getMissing().getBits() );
    m_state.forbidden = (~allowed & m_geometry.getNumberMask());
    m_state.possibilities = Bitmask::countBits(allowed);

    TRACE( cout << "FORBIDDEN: " << Bitmask::createFromBits(m_state.forbidden)
                << endl
                << "ALLOWED:   " << Bitmask::createFromBits(allowed)
                << endl
                << "POSSIBILITIES: " << m_state.possibilities
                << endl; )
  }
}

}

