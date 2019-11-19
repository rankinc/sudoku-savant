#include <iostream>
#include <sstream>
#include <algorithm>

#include "displaygrid.h"
#include "actions.h"
#include "board.h"
using namespace std;

extern "C"
{
#  include "support.h"
}

namespace std
{

istream&
operator>>(istream &istr, sudoku::Board &b)
{
  const sudoku::Geometry::size_type rows = b.getGeometry().getMaxNumber();
  sudoku::Geometry::size_type i = 0;
  string line;

  /*
   * Read valid lines until we run out ...
   */
  while ( (i < rows) && !sudoku::getLine(istr, line).empty() )
  {
    TRACE( cout << "SEQ: [" << line << "]" << endl; )

    istringstream is(line);
    is >> b[i];
    ++i;
  } /* while */

  return istr;
}

ostream&
operator<<(ostream &ostr, const sudoku::Board &b)
{
  return ostr << sudoku::DisplayGrid(b);
}


ostream&
operator<<(ostream &ostr, const sudoku::Board::Exporter &exporter)
{
  const sudoku::Board &b = exporter.getBoard();
  const string &comment = b.getComment();
  
  if ( !comment.empty() )
  {
    ostr << "# " << comment << endl;
  }

  return ostr << "# Rating: " << b.getRating(b.getRating())
              << endl
              << "#"
              << endl
              << "Geometry: " << b.getGeometry()
              << endl
              << b;
}

}


namespace sudoku
{

const Board::Difficulty Board::RATING[] = {
  {   0, _("Easy") },
  {  60, _("Moderate") },
  {  80, _("Difficult") },
  { 120, _("Stinker") },
  { 170, _("Nightmare") },
  { 500, _("Obscene") }
};

/*********************************************************************
 */
Board::~Board()
{}


/*********************************************************************
 */
void
Board::doAllSequences(const SequenceAction &action)
{
  do_for_each(getSequences().begin(), getSequences().end(), action);
}


/*********************************************************************
 */
void
Board::doAllSquares(const SquareAction &action)
{
  do_for_each(getSquares().begin(), getSquares().end(), action);
}


/*********************************************************************
 */
void
Board::pushSquares()
{
  doAllSquares( Push<Square>() );
}


/*********************************************************************
 */
void
Board::pushSequences()
{
  doAllSequences( Push<Sequence>() );
}


/*********************************************************************
 */
void
Board::push()
{
  pushSequences();
  pushSquares();
}


/*********************************************************************
 */
bool
Board::popSquares()
{
  const SquareCollection::iterator end = getSquares().end();
  SquareCollection::iterator square = getSquares().begin();
  bool ok = false;

  while ((square != end) && (ok = square->pop()))
  {
    ++square;
  } /* while */

  return ok;
}


/*********************************************************************
 */
bool
Board::popSequences()
{
  const SequenceCollection::iterator end = getSequences().end();
  SequenceCollection::iterator seq = getSequences().begin();
  bool ok = false;

  while ((seq != end) && (ok = seq->pop()))
  {
    ++seq;
  } /* while */

  return ok;
}


/*********************************************************************
 */
bool
Board::pop()
{
  return (popSequences() && popSquares());
}


/*********************************************************************
 */
void
Board::dropStack()
{
  doAllSequences( DropStack<Sequence>() );
  doAllSquares( DropStack<Square>() );
}


/*********************************************************************
 */
void
Board::clear()
{
  doAllSequences( Clear<Sequence>() );
  doAllSquares( Clear<Square>() );
  setRating(UNKNOWN_RATING);
}


/*********************************************************************
 */
const string
Board::makeBoxName(const Geometry &geometry, unsigned i)
{
  ostringstream ostr;
  ostr << _("Box(") << (i / geometry.getBoxHeight())
                    << ","
                    << (i % geometry.getBoxesPerRow())
                    << ")";
  return ostr.str();
}


/*********************************************************************
 */
void
Board::createTopology()
{
  const Geometry::size_type MAX_NUMBER = getMaxNumber();

  /*
   * There are MAX_NUMBER * MAX_NUMBER squares,
   * and 3 * MAX_NUMBER sequences.
   */
  for (Geometry::size_type i = 0; i < MAX_NUMBER; ++i)
  {
    getRow(i).setProperties( Geometry::ROW, makeName(_("Row "), i) );
    getColumn(i).setProperties( Geometry::COLUMN, makeName(_("Column "), i) );
    getBox(i).setProperties( Geometry::BOX, makeBoxName(getGeometry(), i) );
  } /* for */

  for (Geometry::size_type r = 0; r < MAX_NUMBER; ++r)
  {
    for (Geometry::size_type c = 0; c < MAX_NUMBER; ++c)
    {
       Square square( getGeometry(), r * MAX_NUMBER + c );

       Sequence &row = getRow(r);
       Sequence &column = getColumn(c);
       Sequence &box = getBoxOf(r,c);

       square.setRow( &row );
       square.setColumn( &column );
       square.setBox( &box );

       getSquares().push_back( square );

       Square *sq = &(getSquares().back());

       /*
        * We know which row, column and box this square
        * belongs to. Now we need to position the square
        * correctly within the row, column and box.
        */
       row.set(c, sq);
       column.set(r, sq);
       box.set(((r % getBoxHeight()) * getBoxWidth()) +
                (c % getBoxWidth()), sq);
    } /* for */
  } /* for */
}


/*********************************************************************
 */
void
Board::copyNumbers(const SquareCollection &squares)
{
  /*
   * Copy the board's numbers across ...
   */
  for (const auto& square : squares) {
    if (square.getNumber() != 0) {
      find(square).copyState(square);
    }
  } /* for */
}


/*********************************************************************
 */
Board::Board(const Geometry &geometry)
  : m_geometry(geometry)
  , m_sequences(3 * geometry.getMaxNumber(), Sequence(geometry))
  , m_rating(UNKNOWN_RATING)
{
  createTopology();
}


/*********************************************************************
 */
Board::Board(const Board &board)
  : m_geometry(board.getGeometry())
  , m_sequences(3 * m_geometry.getMaxNumber(), Sequence(m_geometry))
  , m_rating(board.m_rating)
  , m_comment(board.m_comment)
{
  createTopology();
  copyNumbers(board.getSquares());
}


/*********************************************************************
 */
Board&
Board::operator=(const Board &board)
{
  if (this != &board)
  {
    /*
     * First, we do some sanity-checking.
     */
    if (getGeometry() != board.getGeometry())
    {
      ostringstream ostr;
      ostr << _("Cannot assign board with geometry ")
           << board.getGeometry()
           << _(" to one with geometry ")
           << getGeometry();
      throw geometry_violation(ostr.str());
    }

    /*
     * Now throw our original state away, and
     * reset the board with the other board's
     * numbers ...
     */
    clear();
    copyNumbers(board.getSquares());
    setRating(board.getRating());
  }

  return *this;
}


/*********************************************************************
 */
void
Board::dump(ostream &ostr, const string &tag) const
{
  ostr << tag << ':' << endl << *this << endl;

  for (const auto& square : getSquares()) {
    ostr << square.getName() << ": ["
         << square.getNumber() << "] - Forbids "
         << Bitmask::createFromBits(square.getForbidden())
         << " with " << square.getPossibilities() << " possibilities."
         << endl;
  } /* for */

  for (const auto& sequence : getSequences()) {
    ostr << sequence.getName() << ": Missing "
         << sequence.getMissing()
         << endl;
  } /* for */
}


/*********************************************************************
 */
void
Board::randomize(SquareCollection &squares)
{
  /*
   * We want to shuffle our list of squares into
   * a uniformly random order. To do this, we will
   * shuffle the numbers [0..MAXSQUARES-1], assign
   * these numbers sequentially to the squares' tag
   * fields, and then sort the squares into ascending
   * order by their tags.
   */
  auto_array_ptr<unsigned> array(generateRandomArray(squares.size(), 0));

  for_each(squares.begin(), squares.end(), SetTag<Square>(array.get()));
  squares.sort( ByTag() );
}


/*********************************************************************
 */
Board&
Randomize(Board &board)
{
  Board::randomize(board.getSquares());
  return board;
}


/*********************************************************************
 */
Board&
RandomInit(Board &board)
{
  Board::randomize(board.getSquares());

  /*
   * We can assign the first row directly,
   * by creating a random ordering of the
   * numbers [1..MAX_NUMBER].
   */
  Sequence &row = board.getRow(0);

  auto_array_ptr<unsigned> a( generateRandomArray( row.size(), 1 ) );
  unsigned *const array = a.get();

  for (Sequence::size_type i = 0; i < row.size(); ++i)
    row[i] = array[i];

  return board;
}


/*********************************************************************
 */
int
Board::getRatingIndex(int r)
{
  int index = -1;

  for (unsigned i = 0; i < ARRAY_SIZE(RATING); ++i)
  {
    if (r < RATING[i].leastScore)
      break;

    ++index;
  } /* for */

  return index;
}


/*********************************************************************
 */
const char*
Board::getRating(int r)
{
  const int index = getRatingIndex(r);
  return (index != -1 ? gettext(RATING[index].name) : _("Unknown"));
}


/*********************************************************************
 */
const char*
Board::getRatingByIndex(int r)
{
  const char *rating = NULL;

  if ((r >= 0) && (static_cast<unsigned>(r) < ARRAY_SIZE(RATING)))
  {
    rating = gettext(RATING[r].name);
  }

  return rating;
}

}

