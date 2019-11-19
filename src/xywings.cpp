#include <iostream>
using namespace std;

#include "xywings.h"
#include "puzzle.h"
#include "freedom.h"
#include "sequence.h"
#include "square.h"


namespace sudoku
{

const FindXYWings FindXYWings::instance;

/*********************************************************************
 */
struct _XYWing
{
  typedef Step::SquareCollection  SquareCollection;
  const bitmask_t NUMBER_MASK;

  Square *sq[3];
  bitmask_t sharing[3];
  bitmask_t number;

  Sequence::size_type stem;
  Sequence::size_type br1;
  Sequence::size_type br2;

  _XYWing(bitmask_t mask) : NUMBER_MASK(mask) {}

  bool hasCyclicCandidates() const {
      /*
       * We know that precisely two numbers are forbidden
       * on each square in our list, so when we XOR the
       * "forbidden" flag-sets on these three squares together,
       * any XY-XZ-YZ cycle will neatly cancel itself out.
       * (Because there are 2 Xs, 2 Ys and 2 Zs, and nothing else.)
       */
      return (sq[0]->getForbidden() ^
              sq[1]->getForbidden() ^
              sq[2]->getForbidden()) == NUMBER_MASK;
  }

  Square *getStem() const { return sq[stem]; }
  Square *getBranch1() const { return sq[br1]; }
  Square *getBranch2() const { return sq[br2]; }
  unsigned getNumber() const { return Bitmask::getHighestBit(number); }

  void setCommonSequences(Sequence::size_type i, Sequence::size_type j) {
      sharing[3-(i+j)] = MultiSequenceStrategy::getCommonSequences(sq[i], sq[j]);
  }

  bool doesExist();
  int eliminate(SquareCollection&) const;

  static int forbidAtIntersection(Sequence *s1,
                                  Sequence *s2,
                                  const Bitmask &b,
                                  SquareCollection &exclusions)
  {
      return MultiSequenceStrategy::forbidAtIntersection(
                                               s1, s2, NULL, b, exclusions);
  }
};


/*********************************************************************
 */
int
_XYWing::eliminate(SquareCollection &exclusions) const
{
  Square *const branch1 = getBranch1();
  Square *const branch2 = getBranch2();

  Sequence* const seq[][3] = { { branch1->getRow(),
                                 branch1->getColumn(),
                                 branch1->getBox() },
                               { branch2->getRow(),
                                 branch2->getColumn(),
                                 branch2->getBox() } }; 

  const Sequence::Type typeA
          = static_cast<Sequence::Type>(Bitmask::getHighestBit( sharing[br1] ));
  const Sequence::Type typeB
          = static_cast<Sequence::Type>(Bitmask::getHighestBit( sharing[br2] ));

  const Bitmask n(Bitmask::createFromBits(number)); 
  return forbidAtIntersection(seq[0][typeA],
                              seq[1][typeB],
                              n,
                              exclusions)
         + forbidAtIntersection(seq[1][typeA],
                                seq[0][typeB],
                                n,
                                exclusions);
}


/*********************************************************************
 */
bool
_XYWing::doesExist()
{
  const bool exists = ((sharing[0] & sharing[1] & sharing[2]) == 0) &&
                      ((sharing[0] | sharing[1]) != 0) &&
                      ((sharing[0] | sharing[2]) != 0) &&
                      ((sharing[1] | sharing[2]) != 0);
  if (exists)
  {
    stem = 0;
    br1 = 1;
    br2 = 2;

    if (sharing[1] == 0)
    {
      br1 = 0;
      stem = 1;
    }
    if (sharing[2] == 0)
    {
      br2 = 0;
      stem = 2;
    }

    number = ~(sq[br1]->getForbidden() | sq[br2]->getForbidden()) & NUMBER_MASK;
  }

  return exists;
}


/*********************************************************************
 */
bool
FindXYWings::isAllEnabled(const Puzzle *puzzle) const
{
  return puzzle->useXYWings();
}


/*********************************************************************
 */
int
FindXYWings::checkAll(Puzzle *puzzle, const TotalFreedom&) const
{
  TRACE( cout << "CHECKING FOR XY-WINGS" << endl; )

  int changes = 0;

  SquareCollection exclusions;
  SquareCollection squares;

  forEachSquare(puzzle->getSquares(), GetWithPossibilities(squares, 2));
  if ( !squares.empty() )
  {
    TRACE( cout << "FOUND " << squares.size()
                << " SQUARES THAT HAVE 2 POSSIBLE CANDIDATES"
                << endl; )

    _XYWing xywing( squares.front()->getNumberMask() );

    const auto ends = squares.end();
    auto sq1 = squares.begin();

    while ((sq1 != ends) && (changes == 0))
    {
      xywing.sq[0] = *sq1;

      SquareCollection::iterator sq2 = sq1;
      while ((++sq2 != ends) && (changes == 0))
      {
        xywing.sq[1] = *sq2;

        xywing.setCommonSequences(0, 1);

        SquareCollection::iterator sq3 = sq2;
        while (++sq3 != ends)
        {
          xywing.sq[2] = *sq3;

          /*
           * We know that precisely two numbers are forbidden
           * on each square in our list, so when we XOR the
           * "forbidden" flag-sets on these three squares together,
           * any XY-XZ-YZ cycle will neatly cancel itself out.
           * (Because there are 2 Xs, 2 Ys and 2 Zs, and nothing else.)
           */
          if ( xywing.hasCyclicCandidates() )
          {
            xywing.setCommonSequences(0, 2);
            xywing.setCommonSequences(1, 2);

            /*
             * These three squares must not all be in
             * the same row, column or box. Also, each
             * square must align with at least one of
             * the other two to form the XY-Wing.
             */
            if ( xywing.doesExist() )
            {
              changes = xywing.eliminate(exclusions);
              if (changes != 0)
              {
                /*
                 * Changing the board invalidates all the assumptions
                 * that this algorithm is based upon, so we MUST stop!
                 */
                TRACE( cout << "XY-WING FOUND FOR {"
                            << xywing.getStem()->getName()
                            << ", "
                            << xywing.getBranch1()->getName()
                            << ", "
                            << xywing.getBranch2()->getName()
                            << "}"
                            << endl; )
                puzzle->addXYWing( xywing.getStem(),
                                   xywing.getBranch1(),
                                   xywing.getBranch2(),
                                   xywing.getNumber(),
                                   exclusions );
                break;
              }
            }
          }
        } /* while */
      } /* while */

      ++sq1;
    } /* while */
  }

  TRACE( cout << "DONE(changes=" << changes << ")" << endl; )

  return changes;
}

}

