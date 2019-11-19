#include <cstdlib>
#include <cstring>
#include <map>
#include <list>
#include <sstream>
#ifdef DEBUG
#  include <iostream>
#  include <iomanip>
#endif
using namespace std;

#include "colouring.h"
#include "sequence.h"
#include "freedom.h"
#include "puzzle.h"
#include "square.h"


namespace sudoku
{

const UseColouring UseColouring::instance;

/*********************************************************************
 */
class ColourChart
{
public:
  typedef unsigned size_type;

  typedef list<Square*>  SquareCollection;

  enum PAINT_TYPE
  {
    COLOUR = 0,
    ANTICOLOUR
  };

  class PaintPot
  {
  private:
    SquareCollection m_squares[2];

  public:
    PaintPot() {}

    const SquareCollection& getSquares(int i) const { return m_squares[i]; }
    SquareCollection& getSquares(int i) { return m_squares[i]; }

    void merge(SquareCollection&, SquareCollection&);
  };

  typedef map<colour_type, PaintPot>  PaintBox;

private:
  typedef Puzzle::SequenceCollection  SequenceCollection;

  struct SquarePair
  {
    Square* square[2];

    Square* getSquare(int i) const { return square[i]; }
    Square* getColourSquare() const { return getSquare(COLOUR); }
    Square* getAntiColourSquare() const { return getSquare(ANTICOLOUR); }

    void swap() {
        Square* const tmp = square[0];
        square[0] = square[1];
        square[1] = tmp;
    }
  };

  struct Plan
  {
    bitmask_t rows;
    bitmask_t columns;
    bitmask_t boxes;

    Plan() : rows(0), columns(0), boxes(0) {}

    bool populate(const SquareCollection&);
    bool touches(bitmask_t r, bitmask_t c, bitmask_t b) const {
        return ( ((r & rows) != 0) ||
                 ((c & columns) != 0) ||
                 ((b & boxes) != 0) );
    }
    bool touches(const Plan &p) const {
        return touches(p.rows, p.columns, p.boxes);
    }
  };

  struct Page
  {
    colour_type colour;
    Plan plan[2];

    Page(colour_type c) : colour(c) {}
  };

  typedef list<Page>  ColourBook;

  size_type m_size;
  colour_type *m_chart;
  colour_type m_colour;

  PaintBox m_paintBox;

  PAINT_TYPE decideColouring(SquarePair&) const;

  void repaint(PaintBox::iterator&, colour_type, PAINT_TYPE);
  void repaint(SquareCollection &squares, colour_type colour) {
      Strategy::forEachSquare(squares, Repaint(m_chart, colour));
  }

  int eliminateExclusions(const SquareCollection&,
                          const Plan&,
                          const Plan&,
                          colour_type,
                          colour_type,
                          unsigned,
                          SquareCollection&) const;

  int checkMultiColours(const Page&,
                        const Page&,
                        PAINT_TYPE,
                        PAINT_TYPE,
                        SquareCollection&,
                        unsigned,
                        SquareCollection&) const;

  int eliminateOnMultipleColours(const ColourBook&,
                                 SquareCollection&,
                                 unsigned,
                                 SquareCollection&) const;

  class Repaint : public Strategy::SquareAction
  {
  private:
    colour_type *const m_chart;
    const colour_type m_colour;

  public:
    Repaint(colour_type *chart, colour_type colour)
      : m_chart(chart)
      , m_colour(colour) {}

    void operator()(Square *s) const {
        m_chart[ s->getSquareIndex() ] = m_colour;
    }
  };

  static int eliminateContradiction(Puzzle*,
                                    const SquareCollection&,
                                    unsigned);

  static void findHavingCandidates(const SquareCollection &squares,
                                   unsigned n,
                                   SquareCollection &cells) {
      Strategy::forEachSquare(squares,
                    Strategy::GetHavingCandidates(cells, Bitmask::getBit(n)));
  }

  static SquarePair& getSquares(const Bitmask&, Sequence*, SquarePair&);

public:
  explicit ColourChart(size_type);
  ~ColourChart() { delete[] m_chart; }

  colour_type operator[](int i) const { return m_chart[i]; }
  colour_type& operator[](int i) { return m_chart[i]; }

  PaintBox::iterator getPaintPot(colour_type i);

  bool paint(size_type, const TotalFreedom&);
  void clear();

  size_type getSize() const { return m_size; }
  colour_type getColour() const { return m_colour; }
  colour_type getNextColour() { return ++m_colour; }

  int eliminateOnColour(Puzzle*, unsigned) const;

  bool empty() const { return m_paintBox.empty(); }

  void dump(ostream&, size_type) const;

  static colour_type getColour(colour_type c, PAINT_TYPE idx) {
      return (c * (1 - 2*idx));
  }
  static colour_type getComplement(colour_type c) {
      return -c;
  }

  static int forbid(const SquareCollection&, unsigned);
};


/*********************************************************************
 */
#ifdef DEBUG
void
ColourChart::dump(ostream &ostr, size_type rowSize) const
{
  size_type column = 0;
  for (size_type i = 0; i < getSize(); ++i)
  {
    ostr << setw(3) << m_chart[i] << " ";
    if (++column == rowSize)
    {
      column = 0;
      ostr << endl;
    }
  } /* for */
}
#else
inline void
ColourChart::dump(ostream&, size_type) const {}
#endif


/*********************************************************************
 */
void
ColourChart::PaintPot::merge(SquareCollection &colours,
                             SquareCollection &antiColours)
{
  m_squares[COLOUR].splice(m_squares[COLOUR].end(), colours);
  m_squares[ANTICOLOUR].splice(m_squares[ANTICOLOUR].end(), antiColours);
}


/*********************************************************************
 */
ColourChart::SquarePair&
ColourChart::getSquares(const Bitmask &candidates,
                        Sequence *seq,
                        SquarePair &pair)
{
  Sequence::size_type idx[2];

  candidates.getBits(idx, ARRAY_SIZE(idx));

  pair.square[0] = Sequence::getSquare(idx[0], seq->getSquares());
  pair.square[1] = Sequence::getSquare(idx[1], seq->getSquares());

  TRACE( cout << "Candidates in "
              << seq->getName()
              << "#{" << pair.square[0]->getName()
              << ", " << pair.square[1]->getName()
              << "}"
              << endl; )

  return pair;
}

 
/*********************************************************************
 */
ColourChart::PAINT_TYPE
ColourChart::decideColouring(SquarePair &pair) const
{
  const colour_type colour[2] = {
      m_chart[ pair.square[COLOUR]->getSquareIndex() ],
      m_chart[ pair.square[ANTICOLOUR]->getSquareIndex() ]
  };

  const colour_type product = (colour[COLOUR] * colour[ANTICOLOUR]);
  PAINT_TYPE i = COLOUR;

  /*
   * Colours should be positive, and Anti-Colours should be negative.
   */
  if ( ((product < 0) && (colour[COLOUR] < 0)) ||
       ((product > 0) && (colour[COLOUR] > colour[ANTICOLOUR])) ||
       ((product == 0) && ((colour[COLOUR] < 0) || (colour[ANTICOLOUR] > 0))) )
  {
    i = ANTICOLOUR;
    pair.swap();
  }

  /*
   * Are we going to paint the Colour square, or the Anti-Colour square?
   */
  return ((product == 0) ^ (abs(colour[i]) > abs(colour[ANTICOLOUR - i])))
           ? COLOUR : ANTICOLOUR;
}


/*********************************************************************
 */
ColourChart::ColourChart(size_type size)
  : m_size( size )
  , m_chart( new colour_type[size] )
{
  clear();
}


/*********************************************************************
 */
ColourChart::PaintBox::iterator
ColourChart::getPaintPot(colour_type i)
{
  PaintBox::iterator pot = m_paintBox.find( abs(i) );
  if (pot == m_paintBox.end())
  {
    pair<PaintBox::iterator, bool> new_paint;

    new_paint = m_paintBox.insert(
                    PaintBox::value_type(getNextColour(), PaintPot()) );  
    if ( !new_paint.second )
    {
      ostringstream ostr;
      ostr << _("Cannot create paint pot for colour ") << getColour();
      throw runtime_error( ostr.str() );
    }

    pot = new_paint.first;
  }

  return pot;
}


/*********************************************************************
 */
void
ColourChart::clear()
{
  m_colour = 0;
  memset(m_chart, 0, m_size*sizeof(*m_chart));
  m_paintBox.clear();
}


/*********************************************************************
 */
void
ColourChart::repaint(PaintBox::iterator &pot,
                     colour_type oldColour,
                     PAINT_TYPE pidx)
{
  PaintBox::iterator deadPot = m_paintBox.find( abs(oldColour) );

  if (deadPot != m_paintBox.end())
  {
    const PAINT_TYPE oidx = (oldColour < 0 ? ANTICOLOUR : COLOUR);
    const PAINT_TYPE idx = static_cast<PAINT_TYPE>( abs( pidx - oidx ) );

    const colour_type colour = getColour(pot->first, pidx);

    SquareCollection* const squares[2] = {
        &( deadPot->second.getSquares(idx) ),
        &( deadPot->second.getSquares(1 - idx) )
    };

    repaint(*squares[pidx], colour);
    repaint(*squares[1 - pidx], getComplement(colour));

    /*
     * Empty the contents of the old pot into the new pot,
     * and then destroy the empty pot.
     */
    pot->second.merge(*squares[COLOUR], *squares[ANTICOLOUR]);
    m_paintBox.erase(deadPot);
  }
}


/*********************************************************************
 */
bool
ColourChart::paint(size_type n, const TotalFreedom &total)
{
  const auto endf = total.cend();
  auto iter = total.cbegin();

  while (iter != endf)
  {
    const Bitmask& candidates = (*iter)[n];

    /*
     * Find the sequences that have only two
     * candidates for number "n". We will paint
     * one square with a colour, and the other
     * with the corresponding anti-colour.
     */
    if (candidates.size() == 2)
    {
      SquarePair pair;

      const PAINT_TYPE pidx
              = decideColouring( getSquares(candidates,
                                            iter->getSequence(),
                                            pair) );

      Square *paintSquare = pair.getSquare(1 - pidx);

      colour_type colour = -m_chart[ paintSquare->getSquareIndex() ];
      PaintBox::iterator pot = getPaintPot(colour);

      if (colour == 0)
      {
        /*
         * This square is unpainted, so paint it with the
         * anti-colour of its partner square.
         */
        pot->second.getSquares(1 - pidx).push_back(paintSquare);
        colour = getColour(pot->first, pidx);
        m_chart[ paintSquare->getSquareIndex() ] = getComplement(colour);
      }

      Square *emptySquare = pair.getSquare(pidx);
      const colour_type oldColour = m_chart[ emptySquare->getSquareIndex() ];

      if (oldColour == 0)
      {
        TRACE( cout << "Painting " << emptySquare->getName()
                    << " with colour " << colour
                    << endl; )

        pot->second.getSquares(pidx).push_back(emptySquare);
        m_chart[ emptySquare->getSquareIndex() ] = colour;

        dump(cout, total.front().getSequence()->getMaxNumber());
      }
      else if (abs(colour) != abs(oldColour))
      {
        TRACE( cout << "Repainting " << emptySquare->getName()
                    << " from colour " << oldColour
                    << " to colour " << colour
                    << endl; )

        repaint(pot, oldColour, pidx);

        dump(cout, total.front().getSequence()->getMaxNumber());
      }
    }

    ++iter;
  } /* while */

  return !empty();
}


/*********************************************************************
 */
int
ColourChart::forbid(const SquareCollection &squares, unsigned n)
{
  int changes = 0;

  const auto ends = squares.cend();
  auto iter = squares.cbegin();

  while (iter != ends)
  {
    if ( (*iter)->forbid(n) )
    {
      TRACE( cout << "Forbidding " << (*iter)->getName()
                  << " to be " << n
                  << endl; )
      ++changes;
    }

    ++iter;
  } /* while */

  return changes;
}


/*********************************************************************
 */
bool
ColourChart::Plan::populate(const SquareCollection &squares)
{
  bool ok = true;

  for (const auto& square : squares) {
    if ( !Bitmask::setBit(rows, square->getRowIndex()) ||
         !Bitmask::setBit(columns, square->getColumnIndex()) ||
         !Bitmask::setBit(boxes, square->getBoxIndex()) ) {
      ok = false;
      break;
    }
  } /* for */

  return ok;
}


/*********************************************************************
 */
int
ColourChart::eliminateContradiction(Puzzle *puzzle,
                                    const SquareCollection &squares,
                                    unsigned n)
{
  TRACE( cout << "Impossible Colour-Colour pair" << endl; )
  puzzle->addColouring(squares, Colouring::CONTRADICTION, n);
  return forbid(squares, n);
}


/*********************************************************************
 */
int
ColourChart::eliminateExclusions(const SquareCollection &cells,
                                 const Plan &colours,
                                 const Plan &anticolours,
                                 colour_type c1,
                                 colour_type c2,
                                 unsigned n,
                                 SquareCollection &exclusions) const
{
  int changes = 0;

  const auto ends = cells.cend();
  auto sq = cells.cbegin();

  while (sq != ends)
  {
    Square *square = *sq;
    const colour_type sqColour = m_chart[ square->getSquareIndex() ];

    if ( (sqColour != c1) && (sqColour != c2) )
    {
      const bitmask_t row = Bitmask::getBit( square->getRowIndex() );
      const bitmask_t column = Bitmask::getBit( square->getColumnIndex() );
      const bitmask_t box = Bitmask::getBit( square->getBoxIndex() );

      if ( colours.touches(row, column, box) && 
           anticolours.touches(row, column, box) &&
           square->forbid(n) )
      {
        TRACE( cout << "Forbidding " << square->getName()
                    << " to be " << n
                    << endl; )

        if (sqColour == 0)
        {
          TRACE( cout << "- Unpainted square " << square->getName() << endl; )
          exclusions.push_back( square );
        }
        else
        {
          /*
           * This is a square of a different colour that is
           * in contact with a matching colour-anticolour
           * pair.
           */
          TRACE( cout << "- Squares painted with colour " << sqColour << endl; )
          PaintBox::const_iterator pot = m_paintBox.find( abs(sqColour) );
          const PAINT_TYPE idx = (sqColour < 0 ? ANTICOLOUR : COLOUR);

          const SquareCollection &deadSquares = pot->second.getSquares(idx);
          copy(deadSquares.begin(), deadSquares.end(),
               back_insert_iterator<SquareCollection>(exclusions));
          changes += forbid(deadSquares, n);
        }

        ++changes;
      }
    }

    ++sq;
  } /* while */

  return changes;
}


/*********************************************************************
 */
int
ColourChart::checkMultiColours(const Page &page1,
                               const Page &page2,
                               PAINT_TYPE idx1,
                               PAINT_TYPE idx2,
                               SquareCollection &cells,
                               unsigned n,
                               SquareCollection &exclusions) const
{
  int changes = 0;

  /*
   * If two squares of different colours share a row, column or
   * box then we know that they cannot both be "True". Hence at
   * least one of their complementary colours must be "True".
   * This in turn means that any square that shares rows, columns
   * or boxes with BOTH complementary colours cannot have this
   * number as a candidate.
   */
  if ( page1.plan[idx1].touches( page2.plan[idx2] ) )
  {
    changes = eliminateExclusions(cells,
                                  page1.plan[1 - idx1],
                                  page2.plan[1 - idx2],
                                  getComplement(getColour(page1.colour, idx1)),
                                  getComplement(getColour(page2.colour, idx2)),
                                  n,
                                  exclusions);
  }

  return changes;
}


/*********************************************************************
 */
int
ColourChart::eliminateOnMultipleColours(const ColourBook &book,
                                        SquareCollection &cells,
                                        unsigned n,
                                        SquareCollection &exclusions) const
{
  TRACE( cout << "USING MULTI-COLOURING" << endl; )

  int changes = 0;

  const auto endp = book.cend();
  auto page1 = book.cbegin();

  while (page1 != endp)
  {
    decltype(page1) page2 = page1;

    while (++page2 != endp)
    {
      TRACE( cout << "** COMPARING COLOUR " << page1->colour
                  << " WITH COLOUR " << page2->colour
                  << endl; )

      changes = checkMultiColours(*page1, *page2,
                                  COLOUR, COLOUR,
                                  cells, n, exclusions);
      if (changes != 0)
      {
        TRACE( cout << "** COLOUR " << page1->colour
                    << " TOUCHES COLOUR " << page2->colour
                    << endl; )
        break;
      }

      changes = checkMultiColours(*page1, *page2,
                                  COLOUR, ANTICOLOUR,
                                  cells, n, exclusions);
      if (changes != 0)
      {
        TRACE( cout << "** COLOUR " << page1->colour
                    << " TOUCHES ANTI-COLOUR " << -page2->colour
                    << endl; )
        break;
      }

      changes = checkMultiColours(*page1, *page2,
                                  ANTICOLOUR, COLOUR,
                                  cells, n, exclusions);
      if (changes != 0)
      {
        TRACE( cout << "** ANTI-COLOUR " << -page1->colour
                    << " TOUCHES COLOUR " << page2->colour
                    << endl; )
        break;
      }

      changes = checkMultiColours(*page1, *page2,
                                  ANTICOLOUR, ANTICOLOUR,
                                  cells, n, exclusions);
      if (changes != 0)
      {
        TRACE( cout << "** ANTI-COLOUR " << -page1->colour
                    << " TOUCHES ANTI-COLOUR " << -page2->colour
                    << endl; )
        break;
      }
    } /* while */

    if (changes != 0)
      break;

    ++page1;
  } /* while */

  TRACE( cout << "MULTI-COLOURING DONE(changes=" << changes << ")" << endl; )
  return changes;
}

                                        
/*********************************************************************
 */
int
ColourChart::eliminateOnColour(Puzzle *puzzle, unsigned n) const
{
  int totalChanges = 0;

  SquareCollection cells;
  SquareCollection exclusions;
  ColourBook book;

  const auto endp = m_paintBox.cend();
  auto pot = m_paintBox.cbegin();

  /*
   * Check for two identically coloured squares in the
   * same sequence: this is an illegal configuration.
   */
  while (pot != endp)
  {
    int changes = 0;

    Page page(pot->first);

    if ( !page.plan[COLOUR].populate( pot->second.getSquares(COLOUR) ) )
    {
      changes = eliminateContradiction(puzzle,
                                       pot->second.getSquares(COLOUR), n);
    }

    /*
     * We can't eliminate on both Colour and Anti-Colour,
     * and so we only bother checking the Anti-Colour if
     * the Colour squares didn't tell us anything.
     */
    if ( (changes == 0) &&
         !page.plan[ANTICOLOUR].populate( pot->second.getSquares(ANTICOLOUR) ) )
    {
      changes = eliminateContradiction(puzzle,
                                       pot->second.getSquares(ANTICOLOUR), n);
    }

    /*
     * Still no luck? Then try an "exclusion" argument
     * instead of finding a contradictory colour. Any
     * square that shares sequences with both a colour
     * and its anti-colour cannot include our candidate.
     */
    if (changes == 0)
    {
      if ( cells.empty() )
        findHavingCandidates(puzzle->getSquares(), n, cells);

      changes = eliminateExclusions(cells,
                                    page.plan[COLOUR],
                                    page.plan[ANTICOLOUR],
                                    pot->first,
                                    -pot->first,
                                    n,
                                    exclusions);

      if ((changes == 0) && puzzle->useMultiColouring())
      {
        /*
         * We will need this information for when we
         * apply the "multi-colouring" strategy.
         */
        book.push_back(page);
      }
    }

    totalChanges += changes;

    ++pot;
  } /* while */

  if ( !exclusions.empty() )
  {
    TRACE( cout << "Shares sequences with Colour-AntiColour pair" << endl; )
    puzzle->addColouring(exclusions, Colouring::EXCLUSION, n);
  }
  else if ( (totalChanges == 0) && (book.size() > 1) )
  {
    /*
     * We still haven't found anything, so now we try to look at
     * combinations of colours to see if we can learn something.
     * There's obviously no point trying to consider "multiple
     * colours" unless we have more than one colour in the first place!
     */
    totalChanges = eliminateOnMultipleColours(book, cells, n, exclusions);

    if ( !exclusions.empty() )
    {
      TRACE( cout << "Caught between two different Colours" << endl; )
      puzzle->addColouring(exclusions, Colouring::MULTICOLOURING, n);
    }
  }

  return totalChanges;
}


/*********************************************************************
 */
bool
UseColouring::isAllEnabled(const Puzzle *puzzle) const
{
  return puzzle->useColouring();
}


/*********************************************************************
 */
int
UseColouring::checkAll(Puzzle *puzzle, const TotalFreedom &total) const
{
  TRACE( cout << "USING COLOURING" << endl; )

  int changes = 0;

  const Puzzle::size_type MAX_NUMBER = puzzle->getMaxNumber();
  ColourChart chart(MAX_NUMBER * MAX_NUMBER);

  for (Puzzle::size_type n = 1; n <= MAX_NUMBER; ++n)
  {
    TRACE( cout << "COLOURING CANDIDATES FOR NUMBER " << n << ":" << endl; )

    /*
     * Colour all squares that have this number
     * as a candidate, where that candidate appears
     * only twice in any given sequence.
     */
    if ( chart.paint(n, total) )
    {
      changes = chart.eliminateOnColour(puzzle, n);
      if (changes != 0)
        break;

      /*
       * Reset the chart for the next number ...
       */
      chart.clear();
    }
  } /* for */

  TRACE( cout << "COLOURING DONE(changes=" << changes << ")" << endl; )

  return changes;
}

}

