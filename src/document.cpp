#include <ctime>
#include <cstring>
#include <sstream>
#include <iomanip>
using namespace std;

#include "document.h"
#include "geometry.h"
#include "solution.h"
#include "actions.h"
#include "generator.h"

extern "C"
{
#  include "support.h"
}

namespace sudoku
{

/*********************************************************************
 */
class Write : public Document::SquareRefAction
{
private:
  ostream& m_ostr;
  const char *m_tag;

public:
  Write(ostream &ostr, const char *tag) : m_ostr(ostr), m_tag(tag) {}

  void operator()(Square *s) const {
      m_ostr << m_tag << ": "
             << s->getSquareIndex()
             << ","
             << s->getNumber()
             << endl;
  }
};


/*********************************************************************
 */
class WriteNote : public Document::NoteAction
{
private:
  ostream& m_ostr;

public:
  explicit WriteNote(ostream &ostr) : m_ostr(ostr) {}

  void operator()(Note &note, Note::size_type nidx) const {
      if ( note.hasMarks() )
      {
          m_ostr << "Note: " << nidx;

          for (Note::size_type i = 0;
               i < Note::MAX_MARKS;
               ++i)
          {
            const unsigned n = static_cast<const Note&>(note)[i];
            if ( n != 0 )
              m_ostr << "," << n;
          } /* for */

          m_ostr << endl;
      }

      if ( note.hasColour() )
      {
          m_ostr << "Colour: " << nidx
                 << "," << note.getColour()
                 << endl;
      }
  }
};


/*********************************************************************
 */
class ClearNote : public Document::NoteAction
{
public:
  void operator()(Note &n, Note::size_type) const {
      return n.clear();
  }
};


/*********************************************************************
 */
void
Document::do_for_each_move(const SquareRefAction &action) const
{
  do_for_each(m_moves.getMoves().begin(),
              m_moves.getMoves().end(),
              action);
}


/*********************************************************************
 */
void
Document::do_for_each_hint(const SquareRefAction &action) const
{
  do_for_each(m_givenHints.begin(), m_givenHints.end(), action);
}


/*********************************************************************
 */
void
Document::throw_illegal_option(const char *tag, const Square &square)
{
  string msg(tag);
  throw runtime_error( msg.append(": ").append(square.getName()) );
}


/*********************************************************************
 */
Document::Document()
  : m_original( NULL )
  , m_board( new Board( Geometry() ) )
  , m_hasLogical(false)
  , m_hint( m_allHints.end() )
  , m_hintsAreStale(true)
  , m_notebook( new NoteBook( m_board->getGeometry() ) )
  , m_maxHiddenSubsetSize( Generator::DEFAULT_SIZE )
  , m_maxNakedSubsetSize( Generator::DEFAULT_SIZE )
  , m_maxPatternSize( Generator::DEFAULT_SIZE )
  , m_colouring( Puzzle::USE_MULTICOLOURING )
  , m_wings(Puzzle::USE_XY_WINGS | Puzzle::USE_XYZ_WINGS)
{}


/*********************************************************************
 */
Document::~Document()
{
  delete m_original;
  delete m_board;
  delete m_notebook;
}


/*********************************************************************
 */
Board&
Document::recreate(const Geometry &geometry)
{
  m_moves.clear();
  m_givenHints.clear();

  m_hasLogical = false;

  delete m_original;
  m_original = NULL;

  delete m_board;
  m_board = new Board(geometry);

  delete m_notebook;
  m_notebook = new NoteBook( m_board->getGeometry() );

  const Geometry::size_type newHiddenSubsetLimit = getSubsetSizeLimit();
  if (m_maxHiddenSubsetSize > newHiddenSubsetLimit)
    m_maxHiddenSubsetSize = newHiddenSubsetLimit;

  const Geometry::size_type newNakedSubsetLimit = getSubsetSizeLimit();
  if (m_maxNakedSubsetSize > newNakedSubsetLimit)
    m_maxNakedSubsetSize = newNakedSubsetLimit;

  const Geometry::size_type newPatternLimit = getPatternSizeLimit();
  if (m_maxPatternSize > newPatternLimit)
    m_maxPatternSize = newPatternLimit;

  return *m_board;
}


/*********************************************************************
 */
void
Document::finalise()
{
  staleHints();
  validate(m_board);

  delete m_original;
  m_original = new Board( *m_board );

  /*
   * We are solving the original board, so
   * it's a good thing we just created it.
   */
  solve();
}


/*********************************************************************
 */
void
Document::reset()
{
  m_notebook->clear();

  m_moves.clear();
  m_givenHints.clear();
  staleHints();

  if (m_original != NULL)
  {
    delete m_board;
    m_board = new Board(*m_original);

    resetSolution();
  }
}


/*********************************************************************
 */
bool
Document::resetSolution()
{
  const bool isReset = (m_original != NULL);

  if (isReset)
  {
    staleHints();
    solve();
  }

  return isReset;
}


/*********************************************************************
 */
void
Document::generate(int target)
{
  const Geometry geometry( getGeometry() );

  Generator generator(getMaxHiddenSubsetSize(),
                      getMaxNakedSubsetSize(),
                      getMaxPatternSize(),
                      getColouring(),
                      getWings());

  Board& board = recreate( geometry );
  do
  {
    board.clear();
    generator.generate(board);
  }
  while (Board::getRatingIndex( board.getRating() ) < target);

  finalise();
}


/*********************************************************************
 */
void
Document::loadPlayState(istream &istr)
{
  const Board::SquareCollection::size_type
                                  maxSquares = getBoard()->getSquares().size();

  StepCollection  hints;
  StepCollection  moves;

  StringPair option;
  string line;

  while ( !getLine(istr, line).empty() )
  {
    TRACE( cout << "Line: [" << line << "]" << endl; )

    if ( getOption(line, option) )
    {
      bool failed = true;

      StepCollection *collect = NULL;

      if (option.name.compare("SQUARE") == 0)
        collect = &moves;
      else if (option.name.compare("HINT") == 0)
        collect = &hints;
      else if (option.name.compare("NOTE") == 0)
      {
        istringstream is(option.value);
        failed = getNoteBook()->readMarks(is);
      }
      else if (option.name.compare("COLOUR") == 0)
      {
        istringstream is(option.value);
        failed = getNoteBook()->readColour(is);
      }

      if (collect != NULL)
      {
        istringstream is(option.value);
        unsigned squareIndex, number;
        char c;

        is >> squareIndex >> ws >> c >> ws >> number;
        if (is && (squareIndex < maxSquares) && (number != 0))
        {
          collect->push_back( Step(squareIndex, number) );
          failed = false;
        }
      }

      if (failed)
      {
        string msg( _("Invalid option [") );
        throw runtime_error(msg.append(line).append("]"));
      }
    }
  } /* while */

  /*
   * Now load the hints into the board, followed by the moves.
   * We need to load things in this order, regardless of the
   * order they appear in the input file, so that the player
   * isn't warned prematurely of any errors s/he might have
   * made.
   */
  StepCollection::const_iterator end = hints.cend();
  StepCollection::const_iterator step = hints.cbegin();

  while (step != end)
  {
    Square &square = getBoard()->find(step->squareIndex);
    if (square.getNumber() != 0)
      throw_illegal_option("Hint", square);

    addHint(square, step->number);
    ++step;
  } /* while */

  end = moves.end();
  step = moves.begin();

  while (step != end)
  {
    Square &square = getBoard()->find(step->squareIndex);
    if (square.getNumber() != 0)
      throw_illegal_option("Move", square);

    addMove(square, step->number);
    ++step;
  } /* while */

  /*
   * And now for the notes: quietly erase any note
   * for a filled square. 
   */
  NoteCollection::size_type i = getNoteBook()->size();
  while (i != 0)
  {
    if (getBoard()->find(--i).getNumber() != 0)
      (*getNoteBook())[i].clear();
  } /* while */
}


/*********************************************************************
 */
void
Document::load(istream &istr)
{
  Geometry geometry;

  istr >> geometry;
  istr >> recreate(geometry);
  finalise();

  /*
   * Now that we've finalised the original puzzle,
   * let's reload any moves which may have been
   * made already ...
   */
  loadPlayState(istr);
}


/*********************************************************************
 */
void
Document::savePuzzle(ostream &ostr) const
{
  if (m_original != NULL)
  {
    ostr << Board::Exporter(*m_original);
  }
}


/*********************************************************************
 */
void
Document::savePlay(ostream &ostr) const
{
  savePuzzle(ostr);
  ostr << endl;

  do_for_each_hint( Write(ostr, "Hint") );
  do_for_each_move( Write(ostr, "Square") );
  m_notebook->do_for_each( WriteNote(ostr) );
}

 
/*********************************************************************
 */
bool
Document::solve(Board &board,
                BoardCollection &solutions,
                StepCollection *solutionLog)
{
  Puzzle puzzle(board);
  bool logical;

  TRACE( cout << "DOCUMENT:: SOLVING" << endl; )

  solutions.clear();

  puzzle.setMaxHiddenSubsetSize( getMaxHiddenSubsetSize() );
  puzzle.setMaxNakedSubsetSize( getMaxNakedSubsetSize() );
  puzzle.setMaxPatternSize( getMaxPatternSize() );
  puzzle.setColouring( getColouring() );
  puzzle.setWings( getWings() );

  if ( (logical = puzzle.solve()) )
  {
    solutions.push_back(board);
  }
  else
  {
    TRACE( cout << "DOCUMENT::  FINDING ALL SOLUTIONS" << endl; )
    puzzle.findAllSolutions( solutions, UPPER_SOLUTION_LIMIT );
  }

  if (solutionLog != NULL)
    puzzle.getSolution(*solutionLog);

  TRACE( cout << "DOCUMENT:: Logical solution? "
              << (logical ? "Y" : "N")
              << endl; )
  return logical;
}


/*********************************************************************
 */
bool
Document::solve()
{
  Board board( *m_original );
  m_hasLogical = solve(board, m_solutions, &m_solutionLog);
  getBoard()->setRating( board.getRating() );
  m_original->setRating( board.getRating() );
  return m_hasLogical;
}


/*********************************************************************
 */
void
Document::validate(const Board *board)
{
  Bitmask invalids;

  if ( !sudoku::validate(*board, invalids) )
  {
    ostringstream ostr;
    ostr << _("Impossible to solve for ")
         << invalids;
    throw runtime_error( ostr.str() );
  }
}


/*********************************************************************
 */
bool
Document::validate(const Board *b, BoardCollection &solutions)
{
  validate(b);
  Board board(*b);
  return solve(board, solutions);
}


/*********************************************************************
 */
const char*
Document::getRating(const BoardCollection &solutions)
{
  BoardCollection::const_iterator first = solutions.cbegin();
  const Board *board
               = ((first != solutions.cend()) ? first.operator->() : NULL);
  return (board != NULL ? board->getRating( board->getRating() ) : "");
}


/*********************************************************************
 */
string&
Document::generateFileName(string &name) const
{
  ostringstream ostr;
  struct tm date;
  time_t now;

  now = time(NULL);
  localtime_r(&now, &date);

  ostr << "Generated-" << (1900 + date.tm_year)
                       << setfill('0')
                       << setw(2) << (1 + date.tm_mon)
                       << setw(2) << date.tm_mday
                       << setw(2) << date.tm_hour
                       << setw(2) << date.tm_min
                       << setw(2) << date.tm_sec
                       << "-"
                       << getRating();

  return (name = ostr.str());
}


/*********************************************************************
 */
const string&
Document::getStatus(string &str, bool hasLogical, size_type count)
{
  if (hasLogical)
  {
    str = _("Unique logical solution");
  }
  else
  {
    ostringstream ostr;

    switch (count)
    {
    case 1:
      ostr << _("Unique solution (with some guesswork)");
      break;

    case UPPER_SOLUTION_LIMIT:
      ostr << _("At least ") << count << _(" solutions");
      break;

    default:
      ostr << count << _(" solutions");
      break;
    } /* switch */

    str = ostr.str();
  }

  return str;
}


/*********************************************************************
 */
bool
Document::isComplete(const Board *board)
{
  bool complete = true;

  const SquareCollection::const_iterator end = board->getSquares().cend();
  SquareCollection::const_iterator square = board->getSquares().cbegin();

  while (square != end)
  {
    if (square->getNumber() == 0)
    {
      complete = false;
      break;
    }

    ++square;
  } /* while */

  return complete;
}


/*********************************************************************
 */
void
Document::addHint(Square &square, unsigned number)
{
  square = Square::FIXED(number);
  m_givenHints.push_back( &square );
  clearMarks(square);
}


/*********************************************************************
 */
void
Document::addMove(Square &square, unsigned number)
{
  /*
   * We want to allow our player to make SOME mistakes!
   * This function will not alert us if we accidentally
   * make it impossible to place any number in a particular
   * square.
   */
  square.setNumber_player( number );

  m_moves.add(&square);
  clearMarks(square);
  staleHints();
}


/*********************************************************************
 */
Square*
Document::removeLatestMove()
{
  Square *square = m_moves.getLatest();

  if (square != NULL)
  {
    square->revertNumber();
    m_moves.removeLatest();
    staleHints();
  }

  return square;
}

/*********************************************************************
 */
const Hint*
Document::getHint()
{
  const Hint *hint = NULL;

  if ( hintsAreStale() )
  {
    generateHints();
  }

  if (m_hint != m_allHints.end())
  {
    hint = m_hint.operator->();
    ++m_hint;
  }

  return hint;
}


/*********************************************************************
 * We can't include our bad guesses amongst the hints, so we must
 * delete our missteps between the initial proposal and the resulting
 * violation. Remember to keep the "proposing" and "violation"
 * comments, for reference.
 */
bool
Document::deleteMistakes(HintCollection &hints,
                         HintCollection::iterator start,
                         HintCollection::iterator &hint)
{
  bool done = true;

  while (hint != hints.end())
  {
    const Step::Tag tag = hint->tag;

    if ( tag == Step::VIOLATION )
    {
      /*
       * This set of moves was all a mistake, so erase everything
       * from the given "start" up to (but not including) the
       * "violation" comment.
       */
      TRACE( cout << "** erasing [" << start->message
                  << "] to [" << hint->message
                  << "]"
                  << endl; )
      hints.erase(start, hint);
      (--start)->number = 0;
      done = false;
      break;
    }

    if ( (tag == Step::PROPOSAL) || (tag == Step::SOLUTION) )
    {
      /*
       * We have found a proposal, so we'll kick off a recursive
       * scan from our current position to look for the next
       * violation ...
       */
      ++hint;
      if ( deleteMistakes(hints, hint, hint) )
        break;
    }

    ++hint;
  } /* while */

  return done;
}


/*********************************************************************
 */
void
Document::deleteMistakes(HintCollection &hints)
{
  HintCollection::iterator first = hints.begin();
  deleteMistakes(hints, first, first);

  --first;
  if (first->tag == Step::VIOLATION)
    hints.erase(first);
}


/*********************************************************************
 */
void
Document::generateHints()
{
  try
  {
    Board board( *m_board );
    BoardCollection solutions;

    solve(board, solutions, &m_allHints);

    if (solutions.empty())
    {
      m_allHints.clear();
      m_allHints.push_back(
              Solution(_("Cannot complete the puzzle from this position.")) );
    }
    else
    {
      deleteMistakes(m_allHints);
    }
  }
  catch(const violation &e)
  {
    m_allHints.clear();

    string str(e.what());
    str.append( _(": NO SOLUTION") );
    m_allHints.push_back( Solution(str) );
  }

  m_hint = m_allHints.begin();
  freshHints();
}


/*********************************************************************
 */
void
Document::clearMarks(const Square &square)
{
  m_notebook->getNoteFor(square).clear();
}


/*********************************************************************
 */
bool
Document::isErrorMove(const Board &solution, const Square &square)
{
  const unsigned number = square.getNumber();
  return ( (number != 0) && (solution.find(square).getNumber() != number) );
}


bool
Document::isErrorMove(const Square &square) const
{
  return ( hasUniqueSolution() && isErrorMove(getUniqueSolution(), square) );
}


/*********************************************************************
 */
Document::NoteBook::NoteBook(const Geometry &g)
  : m_geometry(g)
  , m_notes(g.getMaxNumber() * g.getMaxNumber(), Note())
{}


void
Document::NoteBook::do_for_each(const NoteAction &action)
{
  for (Note::size_type nidx = 0; nidx < m_notes.size(); ++nidx)
  {
    action(m_notes[nidx], nidx);
  } /* for */
}


void
Document::NoteBook::clear()
{
  do_for_each(ClearNote());
}


bool
Document::NoteBook::readMarks(istream &istr)
{
  bool failed = true;
  unsigned squareIndex;

  istr >> squareIndex;
  if (istr && (squareIndex < m_notes.size()))
  {
    Note::size_type idx = 0;
    unsigned mark;
    char c;

    failed = false;

    /*
     * Keep reading in note-marks until we either
     * run out of marks or run out of space to
     * store them ...
     */
    do
    {
      istr >> c >> ws >> mark;
      if ( !istr )
        break;

      /*
       * Check that the mark is within the correct range.
       */
      if ( (mark == 0) || (mark > getGeometry().getMaxNumber()) )
      {
        failed = true;
        break;
      }

      if ( m_notes[squareIndex].setMark(idx, mark) )
        ++idx;
    }
    while (idx < Note::MAX_MARKS);
  }

  return failed;
}


bool
Document::NoteBook::readColour(istream &istr)
{
  bool failed = true;
  unsigned squareIndex;

  istr >> squareIndex;
  if (istr && (squareIndex < m_notes.size()))
  {
    colour_type colour;
    char c;

    istr >> c >> ws >> colour;

    if ( istr && (colour != 0) && (colour < MAX_COLOURS) )
    {
      m_notes[squareIndex].setColour(colour);
      failed = false;
    }
  }

  return failed;
}

}

