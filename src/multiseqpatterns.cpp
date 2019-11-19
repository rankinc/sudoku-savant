#include <iostream>
#include <climits>
using namespace std;

#include "multiseqpatterns.h"
#include "workset.h"
#include "freedom.h"
#include "puzzle.h"
#include "pattern.h"
#include "sequence.h"
#include "square.h"


namespace sudoku
{

const FindMultiSequencePatterns FindMultiSequencePatterns::instance;

/*********************************************************************
 */
bool
FindMultiSequencePatterns::isAllEnabled(const Puzzle *puzzle) const
{
  return (puzzle->getPatternSize() != 0);
}


/*********************************************************************
 */
void
FindMultiSequencePatterns::eliminateReflections(SubsetCollection &rows,
                                                SubsetCollection &columns)
{
  SubsetCollection::iterator citer = columns.begin();

  while (citer != columns.end())
  {
    SubsetCollection::iterator riter = rows.begin();

    while (riter != rows.end())
    {
      if ( (riter->getSquares() == citer->getNumbers()) &&
           (riter->getNumbers() == citer->getSquares()) )
      {
        TRACE( cout << "Pattern of Rows " << riter->getNumbers()
                    << " and Columns " << riter->getSquares()
                    << " is already complete"
                    << endl; )

        SubsetCollection::iterator deadIter = citer--;
        columns.erase(deadIter);
        rows.erase(riter);
        break;
      }

      ++riter;
    } /* while */

    ++citer;
  } /* while */
}


/*********************************************************************
 */
int
FindMultiSequencePatterns::enforceSymmetry(Puzzle *puzzle,
                                           unsigned number,
                                           const Subset &subset,
                                           const WorkFreedom &work)
{
  int changes = 0;

  const WorkFreedom::const_iterator endw = work.cend();
  const Bitmask& seqlist = subset.getSquares();

  Pattern pattern(subset.getNumbers());

  TRACE( cout << "Enforcing complementary pattern of positions "
              << pattern.positions
              << " for " << number
              << endl; )

  /*
   * Our given number is only permitted in certain
   * positions in our complementary sequences, so
   * forbid it from going anywhere else ...
   */
  WorkFreedom::const_iterator witer = work.cbegin();
  while (witer != endw)
  {
    Sequence *seq = (*witer)->getSequence();

    if ( seqlist.isSet(seq->getIndex()) )
    {
      pattern.sequences.push_back(seq);

      const Sequence::size_type MAX_NUMBER = seq->size();
      for (Sequence::size_type i = 0; i < MAX_NUMBER; ++i)
      {
        if ( pattern.positions.isClear(i) &&
             forbid((*seq)[i], number) )
        {
          TRACE( Square &square = (*seq)[i];
                 if (square.getNumber() != 0) {
                   cout << square.getName() << " is already "
                        << square.getNumber() << endl;
                   abort();
                 } )
          ++changes;
        }
      } /* for */
    }

    ++witer;
  } /* while */

  if (changes != 0)
  {
    puzzle->addMultiSequencePattern(pattern, number);

    /*
     * We need to keep track of the biggest pattern
     * that we've found so far ...
     */
    if (pattern.size() > puzzle->getLargestPatternSize())
      puzzle->setLargestPatternSize(pattern.size());
  }

  return changes;
}


/*********************************************************************
 */
unsigned
FindMultiSequencePatterns::getSmallestPatternSize(const SubsetCollection &s)
{
  unsigned size = UINT_MAX;

  if ( !s.empty() )
    size = s.front().countSquares();

  return size;
}


/*********************************************************************
 */
unsigned
FindMultiSequencePatterns::getLargestPatternSize(const SubsetCollection &s)
{
  unsigned size = 0;

  if ( !s.empty() )
    size = s.back().countSquares();

  return size;
}


/*********************************************************************
 */
int
FindMultiSequencePatterns::checkPatterns(Puzzle *puzzle,
                                         unsigned highPatternSize, 
                                         const TotalFreedom &total)
{
  TRACE( cout << "CHECKING FOR MULTI-SEQUENCE PATTERNS("
              << highPatternSize
              << ")"
              << endl; )

  WorkFreedom incomplete[2];
  int changes = 0;

  {
    /*
     * Sort our remaining Freedoms into Rows and Columns.
     */
    const TotalFreedom::const_iterator end = total.cend();
    TotalFreedom::const_iterator iter = total.cbegin();

    while (iter != end)
    {
      const Freedom &f = *iter;
      const Sequence::Type seqtype = f.getSequence()->getType();

      if (seqtype != Geometry::BOX)
        incomplete[seqtype].push_back(&f);

      ++iter;
    } /* while */
  }

  SubsetCollection rowSubsets;
  SubsetCollection columnSubsets;

  const unsigned MAX_NUMBER = incomplete[Geometry::ROW].front()->size() - 1;

  unsigned n = MAX_NUMBER;
  while ( (n != 0) && (changes == 0) )
  {
    TRACE( cout << "SEARCHING FOR PATTERNS IN " << n << endl; )

    WorkSet workRow(highPatternSize, n, incomplete[Geometry::ROW]);
    WorkSet workColumn(highPatternSize, n, incomplete[Geometry::COLUMN]);

    /*
     * The patterns we are looking for are being able to place
     * N copies of any particular number in one of the same N
     * positions in N different rows. And in the completed grid,
     * those numbers will occupy N other positions in N distinct
     * columns too. So we can reuse our existing "subset" logic  
     * to search for those N positions in N rows/columns.
     *
     * The INTERESTING case is when we find N positions in N rows,
     * but DON'T find the complementary positions when we check the
     * columns. (Or vice versa.)
     */
    eliminateReflections( workRow.findSubsets(rowSubsets),
                          workColumn.findSubsets(columnSubsets) );

    /*
     * We want to enforce any patterns in order of increasing
     * size. Unfortunately, all we know at the moment is that
     * some are row patterns and some are column patterns.
     */
    const unsigned maxSize = max( getLargestPatternSize( rowSubsets ),
                                  getLargestPatternSize( columnSubsets ) );
    if (maxSize != 0)
    {
      const SubsetCollection::const_iterator endr = rowSubsets.cend();
      const SubsetCollection::const_iterator endc = columnSubsets.cend();

      SubsetCollection::const_iterator rowPattern = rowSubsets.cbegin();
      SubsetCollection::const_iterator columnPattern = columnSubsets.cbegin();

      unsigned size = min( getSmallestPatternSize( rowSubsets ),
                           getSmallestPatternSize( columnSubsets ) );
      while ((size <= maxSize) && (changes == 0))
      {
        /*
         * Process all row patterns of the current size ...
         */
        while ( (changes == 0) &&
                (rowPattern != endr) &&
                (rowPattern->countSquares() == size) )
        {
          TRACE( cout << "SUBSET OF " << size << " ROWS FOR " << n << endl; )
          changes = enforceSymmetry(puzzle,
                                    n,
                                    *rowPattern,
                                    incomplete[Geometry::COLUMN]);
          ++rowPattern;
        } /* while */

        /*
         * Now process all column patterns that are also
         * of the current size ...
         */
        while ( (changes == 0) &&
                (columnPattern != endc) &&
                (columnPattern->countSquares() == size) )
        {
          TRACE( cout << "SUBSET OF " << size << " COLUMNS FOR " << n << endl; )
          changes = enforceSymmetry(puzzle,
                                    n,
                                    *columnPattern,
                                    incomplete[Geometry::ROW]);
          ++columnPattern;
        } /* while */

        /*
         * And now consider patterns of the next size up ...
         */
        ++size;
      } /* while */

      columnSubsets.clear();
      rowSubsets.clear();
    }

    --n;
  } /* while */

  return changes;
}


/*********************************************************************
 */
int
FindMultiSequencePatterns::checkAll(Puzzle *puzzle,
                                    const TotalFreedom &total) const
{
  return checkPatterns(puzzle, puzzle->getPatternSize(), total);
}


/*********************************************************************
 */
int
FindMultiSequencePatterns::update(Puzzle *puzzle,
                                  const TotalFreedom &total) const
{
  TRACE( cout << "UPDATING MULTI-SEQUENCE PATTERNS FROM "
              << puzzle->getPatternSize()
              << " SEQUENCES"
              << endl; )

  const unsigned maxPatternSize = puzzle->getMaxPatternSize();
  int changes = 0;

  if (puzzle->getPatternSize() < maxPatternSize)
  {
    changes = checkPatterns(puzzle, maxPatternSize, total);

    /*
     * Adjust the algorithm to search for patterns no larger
     * than the largest that we have found so far ...
     */
    if (changes != 0)
    {
      puzzle->setPatternSize( puzzle->getLargestPatternSize() );
      TRACE( cout << "MULTI-SEQUENCE PATTERNS UPDATED TO "
                  << puzzle->getPatternSize()
                  << " SEQUENCES"
                  << endl; )
    }
  }

  return changes;
}


/*********************************************************************
 */
bool
FindMultiSequencePatterns::forbid(Square &square, unsigned number)
{
  const bool forbidden = (square.getNumber() == 0) && square.forbid(number);

  TRACE( if (forbidden) {
           cout << "Forbidding " << square.getName()
                << " to be " << number
                << endl;
         } )

  return forbidden;
}

}

