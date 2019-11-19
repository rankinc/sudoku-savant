#include <cstring>
#include <iterator>
#include <iostream>
#include <sstream>

#include "puzzle.h"
#include "board.h"
#include "freedom.h"
#include "square.h"
#include "solution.h"
#include "actions.h"
#include "selector.h"
#include "completesequences.h"
#include "crosssequences.h"
#include "hiddensubsets.h"
#include "nakedsubsets.h"
#include "multiseqpatterns.h"
#include "xywings.h"
#include "xyzwings.h"
#include "pattern.h"
#include "colouring.h"
using namespace std;


namespace sudoku
{

/*********************************************************************
 */
Puzzle::State::State(unsigned hsz, unsigned lhsz,
                     unsigned nsz, unsigned lnsz,
                     unsigned psz, unsigned lpsz)
  : hiddenSubsetSize(hsz)
  , largestHiddenSubsetSize(lhsz)
  , nakedSubsetSize(nsz)
  , largestNakedSubsetSize(lnsz)
  , patternSize(psz)
  , largestPatternSize(lpsz)
{}


/*********************************************************************
 */
Puzzle::Puzzle(Board &board)
  : m_board(board)
  , m_hiddenSubsetSizeLimit( board.getGeometry().getSubsetSizeLimit() )
  , m_maxHiddenSubsetSize(m_hiddenSubsetSizeLimit)
  , m_nakedSubsetSizeLimit( board.getGeometry().getSubsetSizeLimit() )
  , m_maxNakedSubsetSize(m_nakedSubsetSizeLimit)
  , m_patternSizeLimit( board.getGeometry().getPatternSizeLimit() )
  , m_maxPatternSize(m_patternSizeLimit)
  , m_colouring( USE_MULTICOLOURING )
  , m_wings( USE_XY_WINGS | USE_XYZ_WINGS )
  , m_forcedSquareScore(DEFAULT_FORCED_SQUARE_SCORE)
  , m_completedSequenceScore(DEFAULT_COMPLETED_SEQUENCE_SCORE)
  , m_crossSequenceConstraintScore(DEFAULT_CROSS_SEQUENCE_CONSTRAINT_SCORE)
  , m_hiddenSubsetScore(DEFAULT_NUMBER_SUBSET_SCORE)
  , m_nakedSubsetScore(DEFAULT_NUMBER_SUBSET_SCORE)
  , m_patternScore(DEFAULT_PATTERN_SCORE)
  , m_xywingScore(DEFAULT_XYWING_SCORE)
  , m_xyzwingScore(DEFAULT_XYZWING_SCORE)
  , m_colouringScore(DEFAULT_COLOURING_SCORE)
  , m_guessworkScore(DEFAULT_GUESSWORK_SCORE)
  , m_guessSelector(NULL)
{
  Board::SquareCollection::iterator square = board.getSquares().begin();
  const Board::SquareCollection::iterator endSquare = board.getSquares().end();

  /*
   * We want to keep a list of all squares that haven't
   * yet been assigned a number ...
   */
  while (square != endSquare)
  {
    if (square->getNumber() == 0)
    {
      m_squares.push_back( square.operator->() );
    }

    ++square;
  } /* while */

  Board::SequenceCollection::iterator sequence = board.getSequences().begin();
  const Board::SequenceCollection::iterator endSequence = board.getSequences().end();

  /*
   * We also want to keep a list of every sequence that
   * has at least one hole in it ...
   */
  while (sequence != endSequence)
  {
    if (sequence->countMissing() != 0)
    {
      m_sequences.push_back( sequence.operator->() );
    }

    ++sequence;
  } /* while */

  /*
   * These are the strategies that we will use
   * on our sequences ...
   */
  m_strategies.push_back( &FindCompleteSequences::instance );

  /*
   * And these are the strategies that we are keeping in reserve ...
   */
  m_newStrategies.push_back( &FindCrossSequenceConstraints::instance );
  m_newStrategies.push_back( &FindHiddenSubsets::instance );
  m_newStrategies.push_back( &FindNakedSubsets::instance );
  m_newStrategies.push_back( &FindMultiSequencePatterns::instance );
  m_newStrategies.push_back( &UseColouring::instance );
  m_newStrategies.push_back( &FindXYWings::instance );
  m_newStrategies.push_back( &FindXYZWings::instance );

  /*
   * And this is the top of our "undo" stack ...
   */
  getStack().push_back( State() );
}


/*********************************************************************
 */
Puzzle::~Puzzle()
{
  delete m_guessSelector;
}


/*********************************************************************
 */
#ifdef DEBUG
void
Puzzle::dumpBoard(ostream &ostr, const string &tag) const
{
  getBoard().dump(ostr, tag);
}
#else
inline void
Puzzle::dumpBoard(ostream&, const string&) const {}
#endif


/*********************************************************************
 */
void
Puzzle::completeSequence(SequenceCollection::iterator iter)
{
  getState().fullSequences.push_back( *iter );
  getSequences().erase(iter);
}


/*********************************************************************
 */
void
Puzzle::completeSquare(SquareCollection::iterator iter)
{
  getState().fullSquares.push_back( *iter );
  getSquares().erase(iter);
}


/*********************************************************************
 */
void
Puzzle::push()
{
  getStack().push_back( State( getHiddenSubsetSize(),
                               getLargestHiddenSubsetSize(),
                               getNakedSubsetSize(),
                               getLargestNakedSubsetSize(),
                               getPatternSize(),
                               getLargestPatternSize() ) );
  getBoard().push();

  dumpBoard(cout, "SAVED");
}


/*********************************************************************
 */
bool
Puzzle::pop()
{
  bool ok = false;

  if ( !completed() )
  {
    /*
     * The sequences and squares that we completed since the last
     * "push" must now be restored ...
     */
    State& state = getState();
    getSequences().splice(getSequences().begin(), state.fullSequences);
    getSquares().splice(getSquares().begin(), state.fullSquares);
    getStack().pop_back();

    /*
     * Now restore the board itself ...
     */
    ok = getBoard().pop();

    dumpBoard(cout, "RESTORED");
  }

  return ok;
}


/*********************************************************************
 */
Puzzle::size_type
Puzzle::getMaxNumber() const
{
  return getBoard().getMaxNumber();
}


/*********************************************************************
 */
void
Puzzle::addForcedSquare(const Square *square, unsigned n)
{
  m_solution.push_back( ForcedSquare(square, n) );
  m_rating.addScore( getForcedSquareScore() );
}


/*********************************************************************
 */
int
Puzzle::setForcedSquares()
{
  TRACE( cout << "FORCING SQUARES" << endl; )

  int changes = 0;

  SquareCollection::iterator sqiter = getSquares().begin();
  const SquareCollection::iterator end = getSquares().end();

  while (sqiter != end)
  {
    bool killSquare = false;

    Square *square = *sqiter;

    if (square->getNumber() != 0)
    {
      killSquare = true;
    }
    else if (square->getPossibilities() == 1)
    {
      const bitmask_t forbidden = square->getForbidden();
      bitmask_t mask = 1;

      for (unsigned n = 1; n <= getMaxNumber(); ++n)
      {
        if ((forbidden & (mask <<= 1)) == 0)
        {
          addForcedSquare(square, n);

          square->setNumber(n);
          ++changes;

          killSquare = true;
          break;
        }
      } /* for */
    }

    if (killSquare)
    {
      completeSquare(sqiter--);
    }

    ++sqiter;
  } /* while */

  return changes;
}


/*********************************************************************
 */
void
Puzzle::addCompletedSequence(const Square *square,
                             const Sequence *sequence,
                             unsigned n)
{
  m_solution.push_back( CompletedSequence(square, n, sequence) );
  m_rating.addScore( getCompletedSequenceScore() );
}


/*********************************************************************
 */
void
Puzzle::addHiddenSubset(const Sequence *sequence,
                        const Subset &subset)
{
  m_solution.push_back( NumberSubset(sequence,
                                     subset.getNumbers(),
                                     subset.getSquares()) );
  m_rating.addScore( getHiddenSubsetScore() * (subset.countSquares() - 1) );
}


/*********************************************************************
 */
void
Puzzle::addNakedSubset(const Sequence *sequence,
                       const Subset &subset)
{
  m_solution.push_back( NumberSubset(sequence,
                                     subset.getSquares(),
                                     subset.getNumbers()) );
  m_rating.addScore( getNakedSubsetScore() * (subset.countSquares() - 1) );
}


/*********************************************************************
 */
void
Puzzle::addMultiSequencePattern(const Pattern &pattern, unsigned n)
{
  m_solution.push_back( MultiSequencePattern(pattern, n) );
  m_rating.addScore(
               getMultiSequencePatternScore() * (pattern.size() - 1) );
}


/*********************************************************************
 */
void
Puzzle::addCrossSequenceConstraint(const Sequence *sequence,
                                   const Sequence *cross,
                                   unsigned n,
                                   const SquareCollection &exclusions)
{
  m_solution.push_back(
               CrossSequenceConstraint(sequence, cross, n, exclusions) );
  m_rating.addScore( getCrossSequenceConstraintScore() );
}


/*********************************************************************
 */
void
Puzzle::addXYWing(const Square *stem,
                  const Square *branch1,
                  const Square *branch2,
                  unsigned number,
                  const SquareCollection &exclusions)
{
  m_solution.push_back( XYWing(stem, branch1, branch2, number, exclusions) );
  m_rating.addScore( getXYWingScore() );
}


/*********************************************************************
 */
void
Puzzle::addXYZWing(const Square *stem,
                   const Square *branch1,
                   const Square *branch2,
                   unsigned number,
                   const SquareCollection &exclusions)
{
  m_solution.push_back( XYZWing(stem, branch1, branch2, number, exclusions) );
  m_rating.addScore( getXYZWingScore() );
}


/*********************************************************************
 */
void
Puzzle::addColouring(const SquareCollection &squares,
                     Colouring::Type type,
                     unsigned n)
{
  m_solution.push_back( Colouring(squares, type, n) );
  m_rating.addScore( getColouringScore()
                       + ((type / 2) * COLOUR_SCORE_INCREMENT) );
}


/*********************************************************************
 */
void
Puzzle::addGuesswork(const Square *square, unsigned n)
{
  m_solution.push_back( Guesswork(square, n) );
  m_rating.addScore( getGuessworkScore() );
}


/*********************************************************************
 */
int
Puzzle::processSequenceStrategies(TotalFreedom &total)
{
  int changes = 0;

  TRACE( cout << "PROCESSING EXISTING SEQUENCE STRATEGIES" << endl; )

  SequenceCollection::iterator iter = getSequences().begin();
  const SequenceCollection::iterator end = getSequences().end();

  while (iter != end)
  {
    Sequence *sequence = *iter;

    if (sequence->countMissing() != 0)
    {
      total.push_back( Freedom(sequence) );
      changes += processSingleSequenceStrategy(total.back());
    }

    if (sequence->countMissing() == 0)
    {
      completeSequence(iter--);
    }

    ++iter;
  } /* while */

  if (changes == 0)
  {
    TRACE( cout << "NOTHING FROM SINGLE-SEQUENCE STRATEGIES - TRY MULTIS"
                << endl; )

    changes = processMultipleSequenceStrategies(total);
  }

  TRACE( cout << "EXISTING SEQUENCE STRATEGIES DONE(changes="
              << changes
              << ")"
              << endl; )

  return changes;
}


/*********************************************************************
 */
int
Puzzle::processSingleSequenceStrategy(const Freedom &freedom)
{
  int changes = 0;

  const StrategyCollection::const_iterator end = getStrategies().cend();
  StrategyCollection::const_iterator strategy = getStrategies().cbegin();

  while (strategy != end)
  {
    if ( (changes = (*strategy)->apply(this, freedom)) != 0 )
      break;

    ++strategy;
  } /* while */

  return changes;
}


/*********************************************************************
 */
int
Puzzle::processMultipleSequenceStrategies(const TotalFreedom &total)
{
  int changes = 0;

  const StrategyCollection::const_iterator end = getStrategies().cend();
  StrategyCollection::const_iterator iter = getStrategies().cbegin();

  while (iter != end)
  {
    const MultiSequenceStrategy *strategy
                 = dynamic_cast<const MultiSequenceStrategy*>( *iter );
    if ( (strategy != NULL) &&
         ((changes = strategy->apply(this, total)) != 0) )
      break;

    ++iter;
  } /* while */

  return changes;
}


/*********************************************************************
 */
int
Puzzle::reprocessSequenceStrategy(const Strategy &strategy,
                                  const TotalFreedom &total)
{
  int changes = 0;

  TotalFreedom::const_iterator freedom = total.cbegin();
  const TotalFreedom::const_iterator end = total.cend();

  while (freedom != end)
  {
    changes = strategy(this, *freedom);
    if (changes != 0)
      break;

    ++freedom;
  } /* while */

  return changes;
}


/*********************************************************************
 */
int
Puzzle::includeNewStrategy(const TotalFreedom &total)
{
  int changes = 0;

  TRACE( cout << "TRYING OUT NEW STRATEGY" << endl; )

  if ( !getNewStrategies().empty() )
  {
    const StrategyCollection::iterator end = getNewStrategies().end();
    StrategyCollection::iterator iter = getNewStrategies().begin();

    while (iter != end)
    {
      const Strategy *strategy = *iter;

      changes = strategy->apply(this, total);

      /*
       * If the new strategy can't find anything at
       * its initial strength then maybe we need to
       * "fortify" its algorithm?
       */
      if (changes == 0)
        changes = strategy->update(this, total);

      if (changes != 0)
      {
        if ( !strategy->isExpensive() )
        {
          TRACE( cout << "Added new sequence strategy" << endl; )

          getStrategies().push_back(strategy);
          getNewStrategies().erase(iter);
        }

        break;
      }

      ++iter;
    } /* while */
  }

  TRACE( cout << "NEW STRATEGY ATTEMPT COMPLETE" << endl; )

  return changes;
}


/*********************************************************************
 */
int
Puzzle::updateStrategies(const TotalFreedom &total)
{
  const StrategyCollection::const_iterator end = getStrategies().cend();
  StrategyCollection::const_iterator strategy = getStrategies().cbegin();
  int changes = 0;

  TRACE( cout << "UPDATING STRATEGIES" << endl; )

  while (strategy != end)
  {
    if ((changes = (*strategy)->update(this, total)) != 0)
      break;

    ++strategy;
  } /* while */

  TRACE( cout << "UPDATE COMPLETE: changes=" << changes << endl; )

  return changes;
}


/*********************************************************************
 */
void
Puzzle::setMaxHiddenSubsetSize(unsigned s)
{
  if ((s == DEFAULT_SIZE) || (s > getHiddenSubsetSizeLimit()))
    s = getHiddenSubsetSizeLimit();

  m_maxHiddenSubsetSize = s;
  setHiddenSubsetSize( getHiddenSubsetSize() );
}


/*********************************************************************
 */
void
Puzzle::setHiddenSubsetSize(unsigned s)
{
  if (s > getMaxHiddenSubsetSize())
    s = getMaxHiddenSubsetSize();
  getState().hiddenSubsetSize = s;
}


/*********************************************************************
 */
void
Puzzle::setMaxNakedSubsetSize(unsigned s)
{
  if ((s == DEFAULT_SIZE) || (s > getNakedSubsetSizeLimit()))
    s = getNakedSubsetSizeLimit();

  m_maxNakedSubsetSize = s;
  setNakedSubsetSize( getNakedSubsetSize() );
}


/*********************************************************************
 */
void
Puzzle::setNakedSubsetSize(unsigned s)
{
  if (s > getMaxNakedSubsetSize())
    s = getMaxNakedSubsetSize();
  getState().nakedSubsetSize = s;
}


/*********************************************************************
 */
void
Puzzle::setMaxPatternSize(unsigned s)
{
  if ((s == DEFAULT_SIZE) || (s > getPatternSizeLimit()))
    s = getPatternSizeLimit();

  m_maxPatternSize = s;
  setPatternSize( getPatternSize() );
}


/*********************************************************************
 */
void
Puzzle::setPatternSize(unsigned s)
{
  if (s > getMaxPatternSize())
    s = getMaxPatternSize();
  getState().patternSize = s;
}


/*********************************************************************
 */
int
Puzzle::processSequences()
{
  TotalFreedom total;
  int changes = processSequenceStrategies(total);

  if (changes == 0)
  {
    /*
     * Our single-sequence strategies didn't help;
     * so try "strengthening" them, and using
     * multiple-sequence strategies.
     */
    changes = updateStrategies(total);

    if (changes == 0)
    {
      /*
       * Still no good. We need new strategies ...
       */
      changes = includeNewStrategy(total);
    }
  }

  return changes;
}


/*********************************************************************
 */
bool
Puzzle::solveByDeduction()
{
  /*
   * We have two strategies, and one is much cheaper
   * than the other. So we use the cheaper one until
   * it can no longer help us, and then we try using
   * the more expensive one. If that helps us to
   * progress then we switch back to using the cheaper
   * one again. With any luck, we'll have completed
   * the puzzle by the time both strategies fail us.
   */
  do
  {
    while (setForcedSquares() != 0) {}
  }
  while ( !getSquares().empty() &&
          (processSequences() != 0) &&
          !getSequences().empty() );

  return (getSquares().empty() || getSequences().empty());
}


/*********************************************************************
 */
bool
Puzzle::guess(Square *square, unsigned n)
{
  bool ok = false;

  TRACE( cout << "Guessing " << square->getName() << " is " << n << endl; )
  try
  {
    if ( square->forbid_nothrow(n) )
    {
      if (square->getPossibilities() != 0)
        push();

      square->setNumber_checkSeqs(n);
      addGuesswork(square, n);
      ok = true;
    }
  }
  catch(const violation&)
  {
    pop();
  }

  return ok;
}


/*********************************************************************
 */
bool
Puzzle::solveByBruteForce(const Selector &selectNumber)
{
  TRACE( cout << "GOING FOR BRUTE FORCE" << endl; )

  getSquares().sort( FewestPossibilitiesFirst() );

  const SquareCollection::iterator end = getSquares().end();
  SquareCollection::iterator sq = getSquares().begin();

  while (sq != end)
  {
    Square *square = *sq;

    if ((square->getNumber() != 0) || (square->getPossibilities() == 0))
    {
      completeSquare(sq--);
    }
    else
    {
      unsigned n = 0;

      /*
       * The number selector is an iterator of sorts:
       * We start with zero, and carry on until it
       * returns zero again.
       */
      while ( (n = selectNumber(square, n)) != 0 )
      {
        /* Guessing a square means that we haven't
         * finished "puzzling" yet ...
         */
        if ( guess(square, n) )
          return false;
      } /* while */
    }

    ++sq;
  } /* while */

  /*
   * We must have solved the puzzle, so we're done.
   */
  return true;
}


/*********************************************************************
 */
bool
Puzzle::solve()
{
  if ( completed() )
    return false;

  TRACE( cout << "BEGIN SOLVING" << endl; )

  bool isComplete = false;

  while ( !isComplete )
  {
    try
    {
      isComplete = solveByDeduction();
    }
    catch(const violation &e)
    {
      TRACE( cout << "VIOLATION: " << e.what() << endl; )

      ostringstream ostr;
      ostr << _("VIOLATION at ")
           << e.getSquare()->getName()
           << _(": Reversing most recent proposal");

      m_solution.push_back( Violation(ostr.str()) );
      isComplete = false;

      // Back-track to next position ...
      if ( !pop() )
        break;
    }

    if ( isComplete )
    {
      TRACE( cout << "COMPLETE" << endl; )
      break;
    }

    if ( m_guessSelector == NULL )
      break;

    isComplete = solveByBruteForce( *m_guessSelector );
  } /* while */

  if (isComplete)
  {
    m_board.setRating( getRating() );
  }

  TRACE( cout << "SOLVE EXITING: " << (isComplete ? "T" : "F") << endl; )
  return isComplete;
}


/*********************************************************************
 */
BoardCollection::size_type
Puzzle::findAllSolutions(BoardCollection &solutions,
                         BoardCollection::size_type maximum)
{
  const Selector *oldSelector = guessBy( new FirstAvailableNumber() );
  try
  {
    /*
     * "Zero maxmimum" is used to mean "Find every solution".
     */
    if (maximum == 0)
      --maximum;

    while ( (solutions.size() != maximum) && solve() )
    {
      solutions.push_back( getBoard() );

      ostringstream ostr;
      ostr << _("FOUND SOLUTION ") << solutions.size();
      m_solution.push_back( Solution(ostr.str()) );

      if ( !pop() )
        break;
    } /* while */

    delete guessBy(oldSelector);
  }
  catch(...)
  {
    delete guessBy(oldSelector);
  }

  return solutions.size();
}


/*********************************************************************
 */
void
Puzzle::getSolution(StepCollection &steps)
{
  steps = m_solution;
  m_solution.clear();
}


/*********************************************************************
 */
ostream&
Puzzle::showSolution(ostream &ostr) const
{
  ostream_iterator<StepCollection::value_type> citer(ostr, "\n");
  copy(getSolution().begin(), getSolution().end(), citer);
  return ostr;
}


/*********************************************************************
 */
int
Puzzle::getRating() const
{
  /*
   * Every time we placed a number, we increased our score.
   * Renormalise our score to be equivalent to a (3x3) sudoku
   * board.
   */
  return (m_rating.getScore() * Board::DEFAULT_SIZE) / getBoard().size();
}


/*********************************************************************
 */
Puzzle::SequenceCollection&
Puzzle::populate(SequenceCollection &seqs,
                 Geometry::Unit unit,
                 bitmask_t seqbits)
{
  const size_type MAX_NUMBER = getMaxNumber();

  auto_array_ptr<Sequence::size_type>
                                seqptr( new Sequence::size_type[MAX_NUMBER] );
  Sequence::size_type *sequence = seqptr.get();

  memset(sequence, 0, MAX_NUMBER * sizeof(sequence[0]));
  const size_type size = Bitmask::getBits(seqbits, sequence, MAX_NUMBER);

  for (size_type i = 0; i < size; ++i)
  {
    seqs.push_back( &(getBoard().getSequence(unit, sequence[i])) );
  } /* for */

  return seqs;
}

}

