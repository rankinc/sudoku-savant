#include <iostream>
#include <sstream>
using namespace std;

#include "generator.h"
#include "selector.h"
#include "puzzle.h"
#include "board.h"
#include "date.h"

namespace sudoku
{

/*********************************************************************
 */
Puzzle&
Generator::parameterise(Puzzle &puzzle) const
{
  const unsigned subsize = getMaxHiddenSubsetSize();
  if (subsize != DEFAULT_SIZE)
  {
    puzzle.setMaxHiddenSubsetSize( subsize );
  }

  const unsigned nakedSubsetSize = getMaxNakedSubsetSize();
  if (nakedSubsetSize != DEFAULT_SIZE)
  {
    puzzle.setMaxNakedSubsetSize( nakedSubsetSize );
  }

  const unsigned patternSize = getMaxPatternSize();
  if (patternSize != DEFAULT_SIZE)
  {
    puzzle.setMaxPatternSize( patternSize );
  }

  const unsigned wings = getWings();
  if (wings != DEFAULT_WINGS)
  {
    puzzle.setWings(wings);
  }

  puzzle.setColouring( getColouring() );
  TRACE( cout << "PARAMETERS:"
              << endl
              << "- Hidden Subset Size: " << puzzle.getMaxHiddenSubsetSize()
              << endl
              << "- Naked Subset Size: " << puzzle.getMaxNakedSubsetSize()
              << endl
              << "- Pattern Size: " << puzzle.getMaxPatternSize()
              << endl
              << "- Simple Colouring: " << (puzzle.getColouring() ? 'Y' : 'N')
              << endl
              << "- Wings: " << hex << puzzle.getWings() << dec
              << endl; )

  return puzzle;
}


/*********************************************************************
 */
bool
Generator::createNewSolution(Board &board)
{
  Puzzle puzzle( RandomInit(board) );

  puzzle.guessBy( new RandomAvailableNumber() );
  bool ok = parameterise( puzzle ).solve();

  board.dropStack();
  return ok;
}


/*********************************************************************
 */
Board&
Generator::prune(Board &board)
{
  int rating = 0;

  Randomize(board);

  const Board::SquareCollection::iterator endsq = board.getSquares().end();
  Board::SquareCollection::iterator sq = board.getSquares().begin();
  while (sq != endsq)
  {
    const unsigned n = sq->getNumber();

    if (n != 0)
    {
      sq->clearNumber();

      Board workboard = board;
      Puzzle puzzle(workboard);

      if ( !parameterise(puzzle).solve() )
        sq->setNumber(n);
      else
        rating = puzzle.getRating();
    }

    ++sq;
  } /* while */

  board.setRating(rating);

  return board;
}


/*********************************************************************
 */
bool
Generator::generate(Board &board)
{
  /* Generate our ultimate solution. */
  Board solution(board.getGeometry());
  bool ok = createNewSolution(solution);

  if (ok)
  {
    Board workboard( board.getGeometry() );

    {
      Puzzle puzzle( Randomize(workboard) );

      puzzle.guessBy( new CopyFromSolution(solution, board) );
      parameterise(puzzle);

      /* "Seed" the board with three random squares. */
      const Board::SquareCollection::const_iterator endsq
                                               = solution.getSquares().cend();
      Board::SquareCollection::const_iterator sq
                                               = solution.getSquares().cbegin();
      Board::SquareCollection::size_type i = 0;
      while ((i < 3) && (sq != endsq))
      {
        (*puzzle.getGuessSelector())(sq.operator->(), 0);
        ++sq;
        ++i;
      } /* while */

      /* Now try and fill in the rest of the work-board using logic.
       * Whenever we are unable to assign a square logically, we
       * copy a square from the solution board. This ensures that
       * our new puzzle has a logical solution.
       */
      ok = puzzle.solve();
    }

    if (ok)
    {
      string str;

      /* So far, so good. But our new puzzle-board probably contains
       * squares that it doesn't need. We're going to have to prune
       * those squares away.
       */
      board = prune( (workboard = board) );
      board.setComment( generateComment(str) );
    }
  }

  return ok;
}


/*********************************************************************
 */
string&
Generator::generateComment(string &str)
{
  ostringstream ostr;

  ostr << "Generated on " << DateFormat( Date() );
  return (str = ostr.str());
}

}

