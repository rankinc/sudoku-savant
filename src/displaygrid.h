#ifndef DISPLAYGRID_H
#define DISPLAYGRID_H

#include "geometry.h"

namespace sudoku
{
  class Board;

  class DisplayGrid
  {
  public:
    typedef Geometry::size_type  size_type;

  private:
    const Geometry &m_geometry;
    unsigned *m_grid;

    void load(const Board&);

  public:
    DisplayGrid(const Board&);
    ~DisplayGrid();

    size_type getMaxNumber() const {
      return m_geometry.getMaxNumber();
    }

    size_type getBoxesPerRow() const {
      return m_geometry.getBoxesPerRow();
    }

    size_type getBoxesPerColumn() const {
      return m_geometry.getBoxesPerColumn();
    }

    const unsigned* operator[](int i) const;
  };
}


namespace std
{
  ostream& operator<<(ostream&, const sudoku::DisplayGrid&);
}

#endif
