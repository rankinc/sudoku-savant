#ifndef STRATEGY_H
#define STRATEGY_H

#include "subsets.h"
#include "constants.h"
#include "actions.h"

namespace sudoku
{
  class Square;
  class Puzzle;
  class Freedom;

  typedef std::list<Freedom>  TotalFreedom;

  class Strategy
  {
  private:
    const bool m_isExpensive;

    virtual bool isEnabled(const Puzzle*, const Freedom&) const = 0;
    virtual int check(Puzzle*, const Freedom&) const = 0;

    virtual bool isAllEnabled(const Puzzle*) const;
    virtual int checkAll(Puzzle*, const TotalFreedom&) const = 0;

  protected:
    Strategy(bool expensive) : m_isExpensive(expensive) {}
    virtual ~Strategy();

  public:
    typedef std::list<Square*>  SquareCollection;

    bool isExpensive() const { return m_isExpensive; }

    virtual int update(Puzzle*, const TotalFreedom&) const;

    int apply(Puzzle*, const Freedom&) const;
    int apply(Puzzle*, const TotalFreedom&) const;

    int operator()(Puzzle *puzzle, const Freedom &freedom) const {
        return apply(puzzle, freedom);
    }

    int operator()(Puzzle *puzzle, const TotalFreedom &total) const {
        return apply(puzzle, total);
    }


    typedef RefAction<Square>  SquareAction;

    class SquareGetter : public SquareAction
    {
    private:
      SquareCollection& m_squares;

    public:
      SquareCollection& getSquares() const { return m_squares; }
 
      SquareGetter(SquareCollection &s) : m_squares(s) {}
      ~SquareGetter();
    };

    class GetWithPossibilities : public SquareGetter
    {
    private:
      const unsigned m_possibilities;

    public:
      GetWithPossibilities(SquareCollection&, unsigned);
      void operator()(Square*) const;
    };

    class GetHavingCandidates : public SquareGetter
    {
    private:
      const bitmask_t m_candidates;

    public:
      GetHavingCandidates(SquareCollection&, bitmask_t);
      void operator()(Square*) const;
    };

    static void forEachSquare(const SquareCollection&, const SquareAction&);
  };
}

#endif

