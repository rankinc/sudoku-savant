#include <iostream>
using namespace std;

#include "strategy.h"
#include "square.h"


namespace sudoku
{

/*********************************************************************
 */
Strategy::~Strategy() {}


/*********************************************************************
 */
int
Strategy::update(Puzzle*, const TotalFreedom&) const
{
  return 0;
}


/*********************************************************************
 */
bool
Strategy::isAllEnabled(const Puzzle*) const
{
  return true;
}


/*********************************************************************
 */
int
Strategy::apply(Puzzle *puzzle, const Freedom &freedom) const
{
  if ( !isEnabled(puzzle, freedom) )
    return 0;

  return check(puzzle, freedom);
}


int
Strategy::apply(Puzzle *puzzle, const TotalFreedom &total) const
{
  if ( !isAllEnabled(puzzle) )
    return 0;

  return checkAll(puzzle, total);
}


/*********************************************************************
 */
Strategy::SquareGetter::~SquareGetter() {}


/*********************************************************************
 */
Strategy::GetWithPossibilities::GetWithPossibilities(SquareCollection &squares,
                                                     unsigned possibilities)
  : SquareGetter(squares)
  , m_possibilities(possibilities)
{}


void
Strategy::GetWithPossibilities::operator()(Square *square) const
{
  if (square->getPossibilities() == m_possibilities)
  {
    getSquares().push_back(square);
  }
}


/*********************************************************************
 */
Strategy::GetHavingCandidates::GetHavingCandidates(SquareCollection &squares,
                                                   bitmask_t candidates)
  : SquareGetter(squares)
  , m_candidates(candidates)
{}


void
Strategy::GetHavingCandidates::operator()(Square *square) const
{
  if ( (square->getForbidden() & m_candidates) == 0 )
  {
    getSquares().push_back(square);
  }
}


/*********************************************************************
 */
void
Strategy::forEachSquare(const SquareCollection &allSquares,
                        const SquareAction &action)
{
  do_for_each(allSquares.begin(), allSquares.end(), action);
}

}

