#ifndef SEQUENCE_H
#define SEQUENCE_H

#include <iostream>
#include <string>
#include <vector>
#include <list>

#include "bitmask.h"
#include "geometry.h"

namespace sudoku
{
  class Square;
  typedef std::vector<Square*>  SquareSequence;

  class Sequence
  {
  public:
    typedef SquareSequence::size_type  size_type;
    typedef Geometry::Unit  Type;

  private:
    typedef Bitmask  State;
    typedef std::list<State>  StateStack;
    typedef bool(Square::*SquareFunc)(unsigned);

    SquareSequence m_squares;
    std::string m_name;

    Bitmask m_missing;
    StateStack m_stack;

    Type m_type;

    const StateStack& getStack() const { return m_stack; }
    StateStack& getStack() { return m_stack; }

    void set(size_type i, Square *sq) { m_squares[i] = sq; }

    void setName(const std::string &name) { m_name = name; }
    void setType(Type t) { m_type = t; }

    void forAllSquares(SquareFunc, unsigned);

  public:
    Sequence(const Geometry&);
    ~Sequence();

    Type __attribute__((pure)) getType() const { return m_type; }

    void setProperties(Type t, const std::string&);

    const std::string& getName() const { return m_name; }

    const SquareSequence& getSquares() const { return m_squares; }
    SquareSequence& getSquares() { return m_squares; }

    const Square* getSquare(size_type i) const {
        return getSquare(i, const_cast<SquareSequence&>(m_squares));
    }
    Square* getSquare(size_type i) {
        return getSquare(i, m_squares);
    }

    const Bitmask& __attribute__((pure)) getMissing() const { return m_missing; }
    Bitmask& __attribute__((pure)) getMissing() { return m_missing; }

    void push();
    bool pop();

    bool add(unsigned number);
    bool add_nothrow(unsigned number);
    bool remove(unsigned number);
    bool __attribute__((pure)) isMissing(unsigned number) const { return getMissing().isSet(number); }
    size_type __attribute__((pure)) countMissing() const { return getMissing().size(); }

    Sequence* findCrossSequence(const Bitmask&) const;

    Sequence* getCrossSequenceAt(size_type i);

    size_type __attribute__((pure)) size() const { return m_squares.size(); }
    size_type __attribute__((pure)) getMaxNumber() const { return size(); }
    bitmask_t __attribute__((pure)) getNumberMask() const { return Geometry::getNumberMask(size()); }
    bitmask_t __attribute__((pure)) getSquareMask() const { return Geometry::getSquareMask(size()); }

    void dropStack() { m_stack.clear(); }
    void clear();

    const Square& __attribute__((pure)) operator[](size_type i) const { return *getSquare(i); }
    Square& __attribute__((pure)) operator[](size_type i) { return *getSquare(i); }

    size_type getIndex() const __attribute__((pure));

    static Square* getSquare(size_type, SquareSequence&) __attribute__((pure));

    friend class Board;
  };
}

namespace std
{
  ostream& operator<<(ostream&, const sudoku::Sequence&);
  istream& operator>>(istream&, sudoku::Sequence&);
}

#endif
