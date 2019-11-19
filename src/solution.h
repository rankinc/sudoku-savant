#ifndef SOLUTION_H
#define SOLUTION_H

#include <list>
#include <ostream>
#include "bitmask.h"

namespace sudoku
{
  class Square;
  class Sequence;
  class Pattern;

  struct Step
  {
    enum Tag
    {
      NONE = 0,
      PROPOSAL,
      VIOLATION,
      SOLUTION
    };

    typedef std::list<Square*>  SquareCollection;

    std::string message;
    unsigned squareIndex;
    unsigned number;
    Tag tag;

    explicit Step(const std::string &m, Tag t)
      : message(m)
      , squareIndex(0)
      , number(0)
      , tag(t) {}
    explicit Step(unsigned i=0, unsigned n=0)
      : squareIndex(i)
      , number(n)
      , tag(NONE) {}
    Step(const Square *s, unsigned n);
    ~Step();

    static std::ostream& showSquares(std::ostream&,
                                     const Sequence*,
                                     const Bitmask&); 
    static std::ostream& showSquares(std::ostream&, const SquareCollection&);
  };

  struct Violation : public Step
  {
    explicit Violation(const std::string &msg) : Step(msg, VIOLATION) {}
  };

  struct Solution : public Step
  {
    explicit Solution(const std::string &msg) : Step(msg, SOLUTION) {}
  };

  struct ForcedSquare : public Step
  {
    ForcedSquare(const Square*, unsigned n);
  };

  struct Guesswork : public Step
  {
    Guesswork(const Square*, unsigned n);
  };

  struct CompletedSequence : public Step
  {
  protected:
    CompletedSequence(const Square *square, unsigned n) : Step(square, n) {}

  public:
    CompletedSequence(const Square*, unsigned n, const Sequence*);
  };

  struct NumberSubset : public Step
  {
    NumberSubset(const Sequence*, const Bitmask&, const Bitmask&);
  };

  struct MultiSequencePattern : public Step
  {
    MultiSequencePattern(const Pattern&, unsigned n);
  };

  struct XYWing : public Step
  {
    XYWing(const Square*, const Square*, const Square*,
           unsigned, const SquareCollection&);
  };

  struct XYZWing : public Step
  {
    XYZWing(const Square*, const Square*, const Square*,
            unsigned, const SquareCollection&);
  };

  struct CrossSequenceConstraint : public Step
  {
    CrossSequenceConstraint(const Sequence*,
                            const Sequence*,
                            unsigned,
                            const SquareCollection&);
  };

  struct Colouring : public Step
  {
    enum Type
    {
      CONTRADICTION,
      EXCLUSION,
      MULTICOLOURING
    };

    Colouring(const SquareCollection&, Type type, unsigned);
  };
}


namespace std
{
  ostream& operator<<(ostream&, const sudoku::Step&);
}

#endif

