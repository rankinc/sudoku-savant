#include "note.h"

namespace sudoku
{

/*********************************************************************
 */
Note::Note()
  : m_marks(0)
  , m_colour(0)
{
  memset(m_mark, 0, sizeof(m_mark));
}


/********************************************************************
 */
void
Note::clear()
{
  memset(m_mark, 0, sizeof(m_mark));
  m_marks = 0;
  m_colour = 0;
}


/********************************************************************
 */
bool
Note::setMark(size_type idx, unsigned n)
{
  const bool isChanged = (m_mark[idx] == 0) && Bitmask::setBit(m_marks, n);
  if (isChanged)
  {
    m_mark[idx] = n;
  }

  return isChanged;
}


/********************************************************************
 */
bool
Note::clearMarkAt(size_type idx)
{
  const unsigned n = m_mark[idx];
  const bool isChanging = (n != 0);

  if (isChanging)
  {
    Bitmask::clearBit(m_marks, n);
    m_mark[idx] = 0;
  }

  return isChanging;
}

}

