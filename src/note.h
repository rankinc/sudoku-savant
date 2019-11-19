#ifndef NOTE_H
#define NOTE_H

#include <cstring>

#include "bitmask.h"
#include "constants.h"

namespace sudoku
{
  class Note
  {
  public:
    typedef unsigned  size_type;

    static const size_type MAX_MARKS = 4;

  private:
    unsigned m_mark[MAX_MARKS];
    bitmask_t m_marks;
    colour_type m_colour;

  public:
    Note();
    bool __attribute__((pure)) hasMarkAt(size_type i) const { return (m_mark[i] != 0); } 
    bool __attribute__((pure)) hasMark(unsigned n) const { return Bitmask::isSet(m_marks, n) ; }
    bool __attribute__((pure)) hasMarks() const { return (m_marks != 0); }
    unsigned getMarkAt(size_type i) const { return m_mark[i]; }
    bool setMark(size_type, unsigned);
    bool clearMarkAt(size_type);

    colour_type __attribute__((pure)) getColour() const { return m_colour; }
    void setColour(colour_type c) { m_colour = c; }
    void clearColour() { setColour(0); }
    bool __attribute__((pure)) hasColour() const { return (m_colour != 0); }

    void clear();

    unsigned __attribute__((pure)) operator[](int i) const { return m_mark[i]; }
    unsigned& __attribute__((pure)) operator[](int i) { return m_mark[i]; }
  };
}

#endif

