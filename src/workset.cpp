#include <algorithm>
using namespace std;

#include "workset.h"
#include "freedom.h"
#include "sequence.h"
#include "square.h"

namespace sudoku
{

/*********************************************************************
 */
WorkSet::WorkSet(size_type setSize, const Freedom &freedom)
  : m_work(setSize, SubsetCollection())
  , m_setSize(setSize)
{
  /*
   * We want to know which squares each of the 1..N
   * numbers are allowed to go into. Only the numbers
   * which haven't been placed are of interest.
   */
  for (Freedom::size_type n = freedom.size() - 1; n != 0; --n)
  {
    add(n, freedom[n]);
  } /* for */
}


/*********************************************************************
 */
WorkSet::WorkSet(size_type setSize, const Sequence *sequence)
  : m_work(setSize, SubsetCollection())
  , m_setSize(setSize)
{
  const bitmask_t NUMBER_MASK = sequence->getNumberMask();

  /*
   * We want to know which numbers are candidates for the
   * squares in this sequence. Only the squares which have
   * no number in are of interest.
   */
  for (Sequence::size_type i = 0; i < sequence->size(); ++i) {
    const Square *square = sequence->getSquare(i);

    if ( square->getNumber() == 0 ) {
      add(i, Bitmask::createFromBits( ~(square->getForbidden()) & NUMBER_MASK ) );
    }
  } /* for */
}


/*********************************************************************
 */
WorkSet::WorkSet(size_type setSize, unsigned n, const WorkFreedom &work)
  : m_work(setSize, SubsetCollection())
  , m_setSize(setSize)
{
  /*
   * We want to know which sequences can place the given number
   * "n" into a certain number of positions.
   */
  const auto endw = work.cend();
  auto iterw = work.cbegin();

  while (iterw != endw)
  {
    const Freedom &freedom = **iterw;

    /*
     * We have separate collections for numbers which appear
     * in two positions, in three positions, four positions etc...
     */
    add(freedom.getSequence()->getIndex(), freedom[n]);

    ++iterw;
  } /* while */
}


/*********************************************************************
 */
WorkSet::~WorkSet()
{}


/*********************************************************************
 */
void
WorkSet::add(unsigned number, const Bitmask &positions)
{
  const size_type count = positions.size();

  if ((count > 1) && (count <= getSetSize()))
  {
    add( (*this)[count],
         Subset(Bitmask::createFromBits(Bitmask::getBit(number)), positions) );
  }
}


/*********************************************************************
 */
void
WorkSet::promote(const Subset &g)
{
  const size_type maxIndex = getIndex(getSetSize());

  TRACE( cout << "Promoting " << g.getNumbers()
              << "#" << g.getSquares()
              << endl; )

  for (size_type i = getIndex(g.getSquares().size() + 1);
       i <= maxIndex;
       ++i)
  {
    const auto endg = m_work[i].end();
    auto iterg = m_work[i].begin();

    while (iterg != endg)
    {
      add( (Subset(g) |= (*iterg)) );
      ++iterg;
    } /* while */
  } /* for */

  TRACE( cout << "- done promoting" << endl; )
}


/*********************************************************************
 */
void
WorkSet::add(SubsetCollection &group, const Subset &subset)
{
  for (auto& s : group) {
    if (merge(s, subset)) {
      TRACE( cout << "Merged: " << s.getNumbers()
                  << "#" << s.getSquares()
                  << endl; )
      return;
    }
  } /* for */

  /*
   * Our new subset isn't a fragment of one of our
   * existing subsets, so add it to the group ...
   */
  group.push_back(subset);

  TRACE( cout << "New: " << group.back().getNumbers()
              << "#"
              << group.back().getSquares() << endl; )
}


/*********************************************************************
 */
bool
WorkSet::merge(Subset &s1, const Subset &s2)
{
  bool contains;

  if ( (contains = (s1.getSquares() == (s1.getSquares() | s2.getSquares()))) )
    s1.getNumbers() |= s2.getNumbers();

  return contains;
}


/*********************************************************************
 */
void
WorkSet::add(const Subset &s)
{
  const size_type cardinality = s.countSquares();

  if (cardinality <= getSetSize())
  {
    add(m_work[getIndex(cardinality)], s);
  }
}


/*********************************************************************
 */
SubsetCollection&
WorkSet::findSubsets(SubsetCollection &subsets)
{
  Subset subset;

  bitmask_t filledSquares = 0;

  for (unsigned subsize = 2; subsize <= getSetSize(); ++subsize)
  {
    TRACE( cout << "-- SUBSETS OF " << subsize << " NUMBERS" << endl; )

    SubsetCollection& workGroups = (*this)[subsize];

    const SubsetCollection::const_iterator end = workGroups.cend();
    SubsetCollection::const_iterator group1 = workGroups.cbegin();

    while (group1 != end)
    {
      /*
       * We only want to include each square in one subset,
       * so if we've already put this square in a subset of
       * (say) 2 elements then we don't want to bother looking
       * for it any any subsets of 3 or more elements.
       */
      if ( ((filledSquares & group1->getSquares().getBits()) == 0) )
      {
        if ( group1->isClosed() )
        {
          TRACE( cout << "A: Found subset: "
                      << group1->getNumbers()
                      << " in "
                      << group1->getSquares()
                      << endl; )
          filledSquares |= group1->getSquares().getBits();
          subsets.push_back(*group1);
        }

        /*
         * Include this work-group into higher
         * collections of work-groups. (Assuming
         * that there are any.)
         */
        promote(*group1);

        SubsetCollection::const_iterator group2 = group1;

        while (++group2 != end)
        {
          (subset = *group1) |= *group2;

          const Subset::size_type cardinality = subset.countSquares();
          if (cardinality == subsize)
          {
            if ( subset.isClosed() )
            {
              TRACE( cout << "B: Found subset: "
                          << subset.getNumbers() 
                          << " in "
                          << subset.getSquares()
                          << endl; )
              filledSquares |= subset.getSquares().getBits();
              subsets.push_back(subset);
            }
          }
          else if (cardinality <= getSetSize())
          {
            add(subset);
          }
        } /* while */
      }

      ++group1;
    } /* while */

    /*
     * We're done with this group, so reduce
     * memory footprint and delete it now ...
     */
    workGroups.clear();
  } /* for */

  return subsets;
}

}

