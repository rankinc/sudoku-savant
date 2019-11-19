#include <iostream>
#include "bitmask.h"
#include "geometry.h"


namespace std
{
  ostream&
  operator<<(ostream &ostr, const sudoku::Bitmask &bitmask)
  {
    return bitmask.write(ostr, '[', ']');
  }
}


namespace sudoku
{

/*********************************************************************
 */
Bitmask::size_type
Bitmask::countBits(bitmask_t bits)
{
  size_type count = 0;

  while (bits != 0)
  {
    if ((bits & 1) != 0)
      ++count;
    bits >>= 1;
  } /* while */

  return count;
}


/*********************************************************************
 */
Bitmask::size_type
Bitmask::getBits(bitmask_t bits, size_type idx[], size_type max)
{
  size_type *p = idx;
  size_type n = 0;

  while ((bits != 0) && (max != 0))
  {
    if ((bits & 1) != 0)
    {
      *p = n;
      ++p;
      --max;
    }

    ++n;
    bits >>= 1;
  } /* while */

  return (p - idx);
}


/*********************************************************************
 */
bool
Bitmask::setBit(bitmask_t &bits, size_type i)
{
  const bitmask_t mask = getBit(i);
  const bool isChanging = ((bits & mask) == 0);

  if (isChanging)
  {
    bits |= mask;
  }

  return isChanging;
}


/*********************************************************************
 */
bool
Bitmask::clearBit(bitmask_t &bits, size_type i)
{
  const bitmask_t mask = getBit(i);
  const bool isChanging = ((bits & mask) != 0);

  if (isChanging)
  {
    bits &= ~mask;
  }

  return isChanging;
}


/*********************************************************************
 */
Bitmask::Bitmask(const bitmask_t bits, size_type count)
  : m_bits(bits)
  , m_count(count)
{}


Bitmask::Bitmask()
  : m_bits(0)
  , m_count(0)
{}


/*********************************************************************
 */
Bitmask
Bitmask::createFromBits(const bitmask_t bits) {
  return Bitmask(bits, countBits(bits));
}


Bitmask
Bitmask::createNumberMask(size_type maxNumber) {
  return Bitmask(Geometry::getNumberMask(maxNumber), maxNumber);
}


/*********************************************************************
 */
bool
Bitmask::setBit(size_type i)
{
  const bool isChanging = setBit(m_bits, i);
  if (isChanging)
  {
    ++m_count;
  }

  return isChanging;
}


/*********************************************************************
 */
bool
Bitmask::clearBit(size_type i)
{
  const bool isChanging = clearBit(m_bits, i);
  if (isChanging)
  {
    --m_count;
  }

  return isChanging;
}


/*********************************************************************
 */
Bitmask::size_type
Bitmask::getHighestBit(bitmask_t bits)
{
  size_type i = 0;

  while ((bits >>= 1) != 0)
    ++i;

  return i;
}


/*********************************************************************
 */
std::ostream&
Bitmask::write(std::ostream &ostr,
               const char openBracket,
               const char closeBracket) const
{
  ostr << openBracket;

  bitmask_t bits = getBits();
  if (bits != 0)
  {
    size_type i = 0;

    do
    {
      if ((bits & 1) != 0)
      {
        ostr << i;

        if ((bits & static_cast<bitmask_t>(~1)) != 0)
          ostr << ',';
      }

      ++i;
      bits >>= 1;
    }
    while (bits != 0);
  }

  ostr << closeBracket;

  return ostr;
}


/*********************************************************************
 */
Bitmask&
Bitmask::operator=(const Bitmask &b)
{
  if (this != &b)
  {
    m_bits = b.m_bits;
    m_count = b.m_count;
  }

  return *this;
}


/*********************************************************************
 */
Bitmask&
Bitmask::operator|=(const Bitmask &b)
{
  if (this != &b)
  {
    m_count = countBits( (m_bits |= b.m_bits) );
  }

  return *this;
}


/*********************************************************************
 */
Subset&
Subset::operator|=(const Subset &s)
{
  if (this != &s)
  {
    m_squares |= s.m_squares;
    m_numbers |= s.m_numbers;
  }

  return *this;
}

}

