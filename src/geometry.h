#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <climits>
#include <iostream>
#include "constants.h"

namespace sudoku
{
  class Geometry
  {
  public:
    typedef unsigned  size_type;

    enum Unit
    {
      UNKNOWN = -1,
      ROW,
      COLUMN,
      BOX
    };

    static const size_type MAX_BOX_SIZE = (sizeof(size_type)*CHAR_BIT - 1);

    static const size_type DEFAULT_WIDTH = 3;
    static const size_type DEFAULT_HEIGHT = DEFAULT_WIDTH;
    static const size_type DEFAULT_MAX_NUMBER = DEFAULT_WIDTH * DEFAULT_HEIGHT;

  private:
    size_type m_maxNumber;
    size_type m_boxWidth;
    size_type m_boxHeight;

    static size_type verify(size_type);

  public:
    Geometry(size_type boxWidth=DEFAULT_WIDTH,
             size_type boxHeight=DEFAULT_HEIGHT);

    void setBoxWidth(size_type w);
    void setBoxHeight(size_type h);

    size_type __attribute__((pure)) getMaxNumber() const { return m_maxNumber; }
    size_type __attribute__((pure)) getSubsetSizeLimit() const { return (m_maxNumber / 2); }
    size_type __attribute__((pure)) getPatternSizeLimit() const { return (m_maxNumber / 2); }

    size_type __attribute__((pure)) getBoxWidth() const { return m_boxWidth; }
    size_type __attribute__((pure)) getBoxHeight() const { return m_boxHeight; }
    size_type __attribute__((pure)) getBoardWidth() const { return getMaxNumber(); }
    size_type __attribute__((pure)) getBoxesPerRow() const { return getBoxHeight(); }
    size_type __attribute__((pure)) getBoxesPerColumn() const { return getBoxWidth(); }

    unsigned __attribute__((pure)) getRowIndex(unsigned sq) const { return (sq / getMaxNumber()); }
    unsigned __attribute__((pure)) getColumnIndex(unsigned sq) const { return (sq % getMaxNumber()); }
    unsigned __attribute__((pure)) getBoxIndex(unsigned sq) const {
        return getBoxNumber( getRowIndex(sq), getColumnIndex(sq) );
    }

    unsigned __attribute__((pure)) getBoxNumber(unsigned row, unsigned column) const {
        return ( ((row / getBoxHeight()) * getBoxesPerRow()) +
                 (column / getBoxWidth()) );
    }

    bitmask_t __attribute__((pure)) getNumberMask() const { return getNumberMask( getMaxNumber() ); }
    bitmask_t __attribute__((pure)) getSquareMask() const { return getSquareMask( getMaxNumber() ); }

    bool operator==(const Geometry&) const __attribute__((pure));
    bool  __attribute__((pure)) operator!=(const Geometry &g) const { return !operator==(g); }

    static size_type __attribute__((const)) getMaxNumber(size_type w, size_type h) { return w * h; }
    static bitmask_t __attribute__((const)) getNumberMask(unsigned n) { return (2 << n) - 2; }
    static bitmask_t __attribute__((const)) getSquareMask(unsigned n) { return (1 << n) - 1; }
  };
}

namespace std
{
  istream& operator>>(istream&, sudoku::Geometry&);
  ostream& operator<<(ostream&, const sudoku::Geometry&);
}

#endif

