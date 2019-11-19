#ifndef SELECTOR_H
#define SELECTOR_H

namespace sudoku
{
  class Square;
  class Board;

  class Selector
  {
  public:
    virtual unsigned operator()(const Square*, unsigned) const = 0;
    virtual ~Selector();
  };

  class FirstAvailableNumber : public Selector
  {
  public:
    unsigned operator()(const Square*, unsigned) const;
  };

  class RandomAvailableNumber : public Selector
  {
  public:
    unsigned operator()(const Square*, unsigned) const;
  };

  class CopyFromSolution : public Selector
  {
  private:
    const Board& m_solution;
    Board &m_board;

    const Board& __attribute__((pure)) getSolution() const { return m_solution; }
    Board& __attribute__((pure)) getBoard() const { return m_board; }

  public:
    CopyFromSolution(const Board &s, Board &b)
      : m_solution(s)
      , m_board(b) {}

    unsigned operator()(const Square*, unsigned) const;
  };
}

#endif

