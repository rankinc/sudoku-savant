#include <cctype>
#include <string>
using namespace std;

#include "board.h"

extern "C"
{
#  include "support.h"
}


namespace sudoku
{

/********************************************************************
 */
void
load(sudoku::Board &board, const string &str)
{
  if ( board.size() != str.size() )
    throw runtime_error(_("Board size differs from input string"));

  board.clear();

  string::size_type i = 0;
  sudoku::Board::SquareCollection::iterator sq = board.getSquares().begin();
  while (sq != board.getSquares().end())
  {
    const char s = str[i++];
    int n = 0;

    if ( isdigit(static_cast<unsigned char>(s)) )
      n = (s - '0');

    if (n != 0)
      *sq = sudoku::Square::FIXED(n);

    ++sq;
  } /* while */ 
}

}
