#include <iostream>
using namespace std;

#include "xyzwings.h"
#include "puzzle.h"
#include "sequence.h"
#include "square.h"


namespace sudoku
{

const FindXYZWings FindXYZWings::instance;

/*********************************************************************
 */
class _XYZWing
{
public:
  typedef Step::SquareCollection  SquareCollection;

private:
  Square *m_stem;
  Square *m_branch1;
  Square *m_branch2;

  bitmask_t sharingSBr1;
  bitmask_t sharingSBr2;
  bitmask_t sharingBr1Br2;

  bitmask_t xyz;
  bitmask_t yz;
  bitmask_t number;
  
public:
  void setStem(Square *s) {
      m_stem = s;
      xyz = m_stem->getForbidden();
  }

  Square* __attribute__((pure)) getStem() const { return m_stem; }

  void setBranch1(Square *s) {
      m_branch1 = s;
      yz = m_branch1->getForbidden();
  }

  Square* __attribute__((pure)) getBranch1() const { return m_branch1; }

  void setBranch2(Square *s) {
      m_branch2 = s;
  }

  Square* __attribute__((pure)) getBranch2() const { return m_branch2; }

  bool __attribute__((pure)) isValidBranch1() const {
      return ((xyz & yz) == xyz);
  }

  bool __attribute__((pure)) isValidBranch2() const {
      return (yz & m_branch2->getForbidden()) == xyz;
  }

  void setCommonSequences() {
      sharingSBr1 = getCommonSequences(m_stem, m_branch1);
      sharingSBr2 = getCommonSequences(m_stem, m_branch2);
      sharingBr1Br2 = getCommonSequences(m_branch1, m_branch2);
  }

  void setNumber() {
      number = ~(xyz | yz | m_branch2->getForbidden())
                & m_stem->getNumberMask();
  }

  unsigned __attribute__((pure)) getNumber() const {
      return Bitmask::getHighestBit(number);
  }

  bool doesExist() const __attribute__((pure));
  int eliminate(SquareCollection&) const;

  static int getCommonSequences(Square *sq1, Square *sq2) {
      return MultiSequenceStrategy::getCommonSequences(sq1, sq2);
  }

  static int forbidAtIntersection(Sequence *s1,
                                  Sequence *s2,
                                  const Square *exclude,
                                  const Bitmask &b,
                                  SquareCollection &exclusions) {
      return MultiSequenceStrategy::forbidAtIntersection(
                                              s1, s2, exclude, b, exclusions);
  }
};


/*********************************************************************
 */
bool
_XYZWing::doesExist() const
{
  return (sharingBr1Br2 == 0) && (sharingSBr1 != 0) && (sharingSBr2 != 0);
}


/*********************************************************************
 */
int
_XYZWing::eliminate(SquareCollection &exclusions) const
{
  Sequence* const seq[][3] = { { m_branch1->getRow(),
                                 m_branch1->getColumn(),
                                 m_branch1->getBox() },
                               { m_branch2->getRow(),
                                 m_branch2->getColumn(),
                                 m_branch2->getBox() } }; 

  const Sequence::Type typeA
          = static_cast<Sequence::Type>(Bitmask::getHighestBit( sharingSBr1 ));
  const Sequence::Type typeB
          = static_cast<Sequence::Type>(Bitmask::getHighestBit( sharingSBr2 ));

  return forbidAtIntersection(seq[0][typeA],
                              seq[1][typeB],
                              m_stem,
                              Bitmask::createFromBits(number),
                              exclusions);
}


/*********************************************************************
 */
bool
FindXYZWings::isAllEnabled(const Puzzle *puzzle) const
{
  return puzzle->useXYZWings();
}


/*********************************************************************
 */
int
FindXYZWings::checkAll(Puzzle *puzzle, const TotalFreedom&) const
{
  TRACE( cout << "CHECKING FOR XYZ-WINGS" << endl; )

  int changes = 0;

  SquareCollection exclusions;
  SquareCollection tripleCandidates;

  forEachSquare(puzzle->getSquares(),
                GetWithPossibilities(tripleCandidates, 3));
  if ( !tripleCandidates.empty() )
  {
    TRACE( cout << "FOUND " << tripleCandidates.size()
                << " SQUARES THAT HAVE 3 POSSIBLE CANDIDATES"
                << endl; )

    SquareCollection doubleCandidates;

    forEachSquare(puzzle->getSquares(),
                  GetWithPossibilities(doubleCandidates, 2));
    if ( !doubleCandidates.empty() )
    {
      TRACE( cout << "FOUND " << doubleCandidates.size()
                  << " SQUARES THAT HAVE 2 POSSIBLE CANDIDATES"
                  << endl; )

      _XYZWing xyzwing;

      const SquareCollection::iterator endd = doubleCandidates.end();
      const SquareCollection::iterator endt = tripleCandidates.end();
      SquareCollection::iterator st = tripleCandidates.begin();

      while ((st != endt) && (changes == 0))
      {
        xyzwing.setStem(*st);

        SquareCollection::iterator br1 = doubleCandidates.begin();
        while ((br1 != endd) && (changes == 0))
        {
          xyzwing.setBranch1(*br1);

          /*
           * An XYZ-Wing consists of three squares, and unlike
           * an XY-Wing pattern, we know which square must be
           * the stem because it's the only one that has three
           * candidates. Each branch's "forbidden" flag-set
           * must be a subset of the stem's.
           */
          if ( xyzwing.isValidBranch1() )
          {
            SquareCollection::iterator br2 = br1;
            while (++br2 != endd)
            {
              xyzwing.setBranch2(*br2);

              /*
               * To form an XYZ-Wing, both branches' "forbidden"
               * flag-sets must combine to give the stem's flag-set.
               */
              if ( !xyzwing.isValidBranch2() )
                continue;

              xyzwing.setCommonSequences();

              if ( xyzwing.doesExist() )
              {
                TRACE( cout << "POSSIBLE XYZ-WING: {"
                            << xyzwing.getStem()->getName() << ", "
                            << xyzwing.getBranch1()->getName() << ", "
                            << xyzwing.getBranch2()->getName() << "}"
                            << endl; )

                xyzwing.setNumber();

                changes = xyzwing.eliminate(exclusions);
                if (changes != 0)
                {
                  /*
                   * Changing the board invalidates all the assumptions
                   * that this algorithm is based upon, so we MUST stop!
                   */
                  TRACE( cout << "FOUND!" << endl; )
                  puzzle->addXYZWing(xyzwing.getStem(),
                                     xyzwing.getBranch1(),
                                     xyzwing.getBranch2(),
                                     xyzwing.getNumber(),
                                     exclusions);
                  break;
                }
              }
            } /* while */
          }

          ++br1;
        } /* while */

        ++st;
      } /* while */
    }
  }

  TRACE( cout << "DONE(changes=" << changes << ")" << endl; )

  return changes;
}

}

