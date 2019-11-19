#include <sstream>
using namespace std;

#include "solution.h"
#include "pattern.h"
#include "sequence.h"
#include "square.h"

extern "C"
{
#  include "support.h"
}

namespace std
{

/*********************************************************************
 */
ostream&
operator<<(ostream &ostr, const sudoku::Step &step)
{
  return (ostr << step.message);
}

}


namespace sudoku
{

/*********************************************************************
 */
Step::Step(const Square *s, unsigned n)
  : squareIndex(s->getSquareIndex())
  , number(n)
  , tag(NONE)
{}

Step::~Step() {}


/*********************************************************************
 */
ostream&
Step::showSquares(ostream &ostr,
                  const Sequence *sequence,
                  const Bitmask &squares)
{
  unsigned i = squares.getHighestBit();

  ostr << (*sequence)[i].getName();

  while (i != 0)
  {
    if (squares.isSet(--i))
      ostr << _(", ") << (*sequence)[i].getName();
  } /* while */

  return ostr;
}


/*********************************************************************
 */
ostream&
Step::showSquares(ostream &ostr, const SquareCollection &squares)
{
  const SquareCollection::const_iterator ends = squares.cend();
  SquareCollection::const_iterator sq = squares.cbegin();

  ostr << (*sq)->getName();
  while (++sq != ends)
  {
    ostr << _(", ") << (*sq)->getName();
  } /* while */

  return ostr;
}


/*********************************************************************
 */
ForcedSquare::ForcedSquare(const Square *square, unsigned n)
  : Step(square, n)
{
  ostringstream ostr;
  ostr << _("Forced to set ")
       << square->getName()
       << _(" to ")
       << number
       << _(".");

  message = ostr.str();
}


/*********************************************************************
 */
Guesswork::Guesswork(const Square *square, unsigned n)
  : Step(square, n)
{
  ostringstream ostr;
  ostr << _("Proposing setting ")
       << square->getName()
       << _(" to ")
       << number
       << _(".");
  tag = PROPOSAL;

  message = ostr.str();
}


/*********************************************************************
 */
CompletedSequence::CompletedSequence(const Square *square,
                                     unsigned n,
                                     const Sequence *sequence)
  : Step(square, n)
{
  ostringstream ostr;
  ostr << _("Number ")
       << number
       << _(" can only go in ")
       << square->getName()
       << _(" in ")
       << sequence->getName()
       << _(".");

  message = ostr.str();
}


/*********************************************************************
 */
NumberSubset::NumberSubset(const Sequence *sequence,
                           const Bitmask &numbers,
                           const Bitmask &squares)
{
  ostringstream ostr;

  ostr << _("Numbers ") << numbers
       << _(" must go in {");

  showSquares(ostr, sequence, squares);
  
  ostr << _("} in ") << sequence->getName() << _(".");

  message = ostr.str();
}


/*********************************************************************
 */
CrossSequenceConstraint::CrossSequenceConstraint(
                                           const Sequence *sequence1,
                                           const Sequence *sequence2,
                                           unsigned n,
                                           const SquareCollection &exclusions)
{
  ostringstream ostr;

  ostr << _("{");
  showSquares(ostr, exclusions);
  ostr << _("} cannot be ")
       << n
       << _(" in ")
       << sequence2->getName()
       << _(" because ")
       << n
       << _(" is locked into ")
       << sequence1->getName()
       << _(".");

  message = ostr.str();
}


/*********************************************************************
 */
MultiSequencePattern::MultiSequencePattern(const Pattern &pattern, unsigned n)
{
  ostringstream ostr;

  switch( pattern.size() )
  {
  case 2:
    ostr << _("X-Wing");
    break;

  case 3:
    ostr << _("Swordfish");
    break;

  case 4:
    ostr << _("Jellyfish");
    break;

  case 5:
    ostr << _("Starfish");
    break;

  default:
    ostr << pattern.size() << "x" << pattern.size() << _(" fish");
    break;
  } /* switch */

  const Pattern::SequenceCollection::const_iterator ends
                                               = pattern.sequences.cend();
  Pattern::SequenceCollection::const_iterator seq = pattern.sequences.cbegin();

  ostr << _(": Number ") << n
       << _(" limited to positions ");
  pattern.positions.write(ostr, '{', '}');

  ostr << _(" in {") << (*seq)->getName();

  while(++seq != ends)
  {
    ostr << _(", ") << (*seq)->getName();
  } /* while */

  ostr << _("}");

  message = ostr.str();
}


/*********************************************************************
 */
XYWing::XYWing(const Square *stem,
               const Square *branch1,
               const Square *branch2,
               unsigned n,
               const SquareCollection &exclusions)
{
  ostringstream ostr;
  ostr << _("XY-Wing at ") << stem->getName()
       << _(", branching to ")
       << branch1->getName()
       << _(" and ")
       << branch2->getName()
       << _(": {");

  showSquares(ostr, exclusions);

  ostr << _("} cannot be ")
       << n
       << _(".");

  message = ostr.str();
}


/*********************************************************************
 */
XYZWing::XYZWing(const Square *stem,
                 const Square *branch1,
                 const Square *branch2,
                 unsigned n,
                 const SquareCollection &exclusions)
{
  ostringstream ostr;
  ostr << _("XYZ-Wing at ") << stem->getName()
       << _(", branching to ")
       << branch1->getName()
       << _(" and ")
       << branch2->getName()
       << _(": {");

  showSquares(ostr, exclusions);

  ostr << _("} cannot be ")
       << n
       << _(".");

  message = ostr.str();
}


/*********************************************************************
 */
Colouring::Colouring(const SquareCollection &squares, Type type, unsigned n)
{
  ostringstream ostr;

  if (type == MULTICOLOURING)
  {
    ostr << _("Multi-Colouring: {");
  }
  else
  {
    ostr << _("Simple Colouring(")
         << (type == CONTRADICTION ? _("Contradiction") : _("Exclusion"))
         << _("): {");
  }

  showSquares(ostr, squares);

  ostr << _("} cannot be ") << n << _(".");
 
  message = ostr.str();
}
 
}

