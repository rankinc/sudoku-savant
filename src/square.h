#ifndef SQUARE_H
#define SQUARE_H

#include <string>
#include <list>

#include "constants.h"
#include "geometry.h"

namespace sudoku
{
  class Sequence;
  class Bitmask;

  class Square
  {
  public:
    class FIXED
    {
    private:
      unsigned m_number;

    public:
      explicit FIXED(unsigned n) : m_number(n) {}
      unsigned getNumber() const { return m_number; }
    };

  private:
    typedef void(Square::*SetterFn)(unsigned);

    struct State
    {
      bitmask_t forbidden;
      unsigned  possibilities;
      unsigned  number;

      State(const Geometry &g)
        : forbidden(0)
        , possibilities(g.getMaxNumber())
        , number(0)
      {}

      void clear(const Geometry&);
      void set(const State&);
    };

    typedef std::list<State>  StateStack;

    unsigned m_index;
    std::string m_name;

    const Geometry &m_geometry;
    mutable unsigned m_tag;

    State m_state;
    StateStack m_stack;

    Sequence *m_row;
    Sequence *m_column;
    Sequence *m_box;

    const State& getState() const { return m_state; }
    State& getState() { return m_state; }
    StateStack& getStack() { return m_stack; }

    void setRow(Sequence *sequence) { m_row = sequence; }
    void setColumn(Sequence *sequence) { m_column = sequence; }
    void setBox(Sequence *sequence) { m_box = sequence; }

    void forbidAll();
    void copyState(const Square &s);

    bool setNumber_validator(SetterFn, unsigned);
    void setNumber_nothrow(unsigned newNumber);

    static std::string makeName(const Geometry&, unsigned sq) __attribute__((pure));

  public:
    const std::string& getName() const { return m_name; }
    unsigned __attribute__((pure)) getSquareIndex() const { return m_index; }

    unsigned __attribute__((pure)) getNumber() const { return m_state.number; }
    void setNumber_checkSeqs(unsigned newNumber);
    bool setNumber(unsigned newNumber) {
        return setNumber_validator(&Square::setNumber_checkSeqs, newNumber);
    }
    bool setNumber_player(unsigned newNumber) {
        return setNumber_validator(&Square::setNumber_nothrow, newNumber);
    }
    bool clearNumber();
    void revertNumber();

    const Geometry& __attribute__((pure)) getGeometry() const { return m_geometry; }

    unsigned __attribute__((pure)) getMaxNumber() const {
        return getGeometry().getMaxNumber();
    }

    bitmask_t __attribute__((pure)) getNumberMask() const {
        return getGeometry().getNumberMask();
    }

    unsigned __attribute__((pure)) getRowIndex() const {
        return getGeometry().getRowIndex(getSquareIndex());
    }
    unsigned __attribute__((pure)) getColumnIndex() const {
        return getGeometry().getColumnIndex(getSquareIndex());
    }
    unsigned __attribute__((pure)) getBoxIndex() const {
        return getGeometry().getBoxIndex(getSquareIndex());
    }

    unsigned getTag() const { return m_tag; }
    void setTag(unsigned n) { m_tag = n; }
    void setTag(unsigned n) const { m_tag = n; }

    const Sequence* __attribute__((pure)) getRow() const { return m_row; }
    Sequence* __attribute__((pure)) getRow() { return m_row; }

    const Sequence* __attribute__((pure)) getColumn() const { return m_column; }
    Sequence* __attribute__((pure)) getColumn() { return m_column; }

    const Sequence* __attribute__((pure)) getBox() const { return m_box; }
    Sequence* __attribute__((pure)) getBox() { return m_box; }

    unsigned __attribute__((pure)) getPossibilities() const { return m_state.possibilities; }
    bitmask_t __attribute__((pure)) getForbidden() const { return m_state.forbidden; }

    bool forbid_nothrow(unsigned number);
    bool forbid_nothrow(bitmask_t);
    bool forbid(unsigned number);
    bool forbid(const Bitmask&);
    bool allow(unsigned number);

    bool belongsTo(const Sequence*) const __attribute__((pure));

    void dropStack() { m_stack.clear(); }
    void clear();

    void push();
    bool pop();

    Square(const Geometry&, unsigned i);
    ~Square();

    Square& operator=(unsigned n) {
        setNumber(n);
        return *this;
    }

    Square& operator=(const FIXED &f) {
        setNumber(f.getNumber());
        forbidAll();
        return *this;
    }

    friend class Board;
  };


  class FewestPossibilitiesFirst
  {
  public:
    bool operator()(const Square *s1, const Square *s2) const __attribute__((pure)) {
      return (s1->getPossibilities() < s2->getPossibilities());
    }
  };
}

#endif
