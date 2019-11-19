#include <stdexcept>
#include <iostream>
using namespace std;

#include "displaygrid.h"
#include "geometry.h"
#include "square.h"
#include "board.h"

namespace sudoku
{

DisplayGrid::DisplayGrid(const Board &board)
  : m_geometry(board.getGeometry())
  , m_grid( new unsigned[board.getMaxNumber() * board.getMaxNumber()] )
{
  load(board);
}


DisplayGrid::~DisplayGrid()
{
  delete[] m_grid;
}


void
DisplayGrid::load(const Board &board)
{
  const Board::SquareCollection::const_iterator end = board.getSquares().cend();
  Board::SquareCollection::const_iterator square = board.getSquares().cbegin(); 

  while (square != end)
  {
    m_grid[square->getRowIndex() * getMaxNumber() + square->getColumnIndex()]
                                                      = square->getNumber();

    ++square;
  } /* while */
}


const unsigned*
DisplayGrid::operator[](int i) const
{
  if ((i < 0) || (static_cast<unsigned>(i) >= m_geometry.getMaxNumber()))
    throw runtime_error("Index out of range");

  return &(this->m_grid[i * m_geometry.getMaxNumber()]);
}

}


namespace std
{

ostream&
operator<<(ostream &ostr, const sudoku::DisplayGrid &grid)
{
  sudoku::DisplayGrid::size_type i = 0;

  for (;;)
  {
    sudoku::DisplayGrid::size_type j = 0;

    for (;;)
    {
      ostr << grid[i][j];

      if (++j == grid.getMaxNumber())
        break;

      ostr << ',';

      if ((j % grid.getBoxesPerColumn()) == 0)
        ostr << ' ';

    } /* for */

    ostr << endl;

    if (++i == grid.getMaxNumber())
      break;

    if ((i % grid.getBoxesPerRow()) == 0)
      ostr << endl;

  } /* for */

  return ostr;
}

}

