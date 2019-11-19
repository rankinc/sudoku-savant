#ifndef BOARD_H
#define BOARD_H

#include <list>
#include <vector>
#include <stdexcept>

#include "square.h"
#include "sequence.h"
#include "geometry.h"
#include "actions.h"


namespace sudoku
{
  class Board
  {
  public:
    typedef Geometry::size_type    size_type;
    typedef std::list<Square>      SquareCollection;
    typedef std::vector<Sequence>  SequenceCollection;

    static const int UNKNOWN_RATING = -1;
    static const size_type DEFAULT_SIZE
                 = Geometry::DEFAULT_MAX_NUMBER * Geometry::DEFAULT_MAX_NUMBER;

    typedef Action<Square>    SquareAction;
    typedef Action<Sequence>  SequenceAction;

    class Exporter
    {
    private:
      const Board& m_board;

    public:
      explicit Exporter(const Board &b) : m_board(b) {}
      const Board& __attribute__((pure)) getBoard() const { return m_board; }
    };

    class geometry_violation : public std::runtime_error
    {
    public:
      geometry_violation(const std::string &msg) : runtime_error(msg) {}
    };

  private:
    Geometry m_geometry;
    SquareCollection m_squares;
    SequenceCollection m_sequences;

    int m_rating;
    std::string m_comment;

    template<class T>
    class Push : public Action<T>
    {
    public:
      void operator()(T &x) const { x.push(); }
    };

    template<class T>
    class SetTag : public std::unary_function<T, void>
    {
    private:
      unsigned *const m_array;
      size_t i;

    public:
      SetTag(unsigned *a) : m_array(a), i(0) {}
      void operator()(T &x) { x.setTag(m_array[i++]); }
    };

    class ByTag
    {
    public:
      bool __attribute__((pure)) operator()(const Square &s1, const Square &s2) const {
          return (s1.getTag() < s2.getTag());
      }
    };

    void createTopology();
    void copyNumbers(const SquareCollection&);

    void pushSequences();
    bool popSequences();
    void pushSquares();
    bool popSquares();

    struct Difficulty
    {
      int leastScore;
      const char *name;
    };

    static const Difficulty RATING[];

  public:
    const Geometry& __attribute__((pure)) getGeometry() const { return m_geometry; }

    size_type __attribute__((pure)) getMaxNumber() const { return getGeometry().getMaxNumber(); }
    size_type __attribute__((pure)) getBoxWidth() const { return getGeometry().getBoxWidth(); }
    size_type __attribute__((pure)) getBoxHeight() const { return getGeometry().getBoxHeight(); }

    size_type __attribute__((pure)) getBoxesPerRow() const {
        return getGeometry().getBoxesPerRow();
    }

    size_type __attribute__((pure)) getBoxesPerColumn() const {
        return getGeometry().getBoxesPerColumn();
    }

    bitmask_t __attribute__((pure)) getNumberMask() const { return getGeometry().getNumberMask(); }

    int __attribute__((pure)) getRating() const { return m_rating; }
    void setRating(int r) { m_rating = r; }

    const std::string& __attribute__((pure)) getComment() const { return m_comment; }
    void setComment(const std::string &c) { m_comment = c; }

    const SquareCollection& __attribute__((pure)) getSquares() const { return m_squares; }
    SquareCollection& __attribute__((pure)) getSquares() { return m_squares; }

    const SequenceCollection& __attribute__((pure)) getSequences() const { return m_sequences; }
    SequenceCollection& __attribute__((pure)) getSequences() { return m_sequences; }

    const Sequence& __attribute__((pure)) getSequence(unsigned number) const {
        return m_sequences[number];
    }
    Sequence& __attribute__((pure)) getSequence(unsigned number) { return m_sequences[number]; }

    Sequence& __attribute__((pure)) getRow(unsigned number) { return getSequence(number); }
    const Sequence& __attribute__((pure)) getRow(unsigned number) const {
        return getSequence(number);
    }

    Sequence& __attribute__((pure)) getColumn(unsigned number) {
        return getSequence(getMaxNumber() + number);
    }
    const Sequence& __attribute__((pure)) getColumn(unsigned number) const {
        return getSequence(getMaxNumber() + number);
    }

    const Sequence& __attribute__((pure)) getSequence(Sequence::Type type, unsigned number) const {
        return getSequence(type*getMaxNumber() + number);
    }
    Sequence& __attribute__((pure)) getSequence(Sequence::Type type, unsigned number) {
        return getSequence(type*getMaxNumber() + number);
    }

    const Sequence& __attribute__((pure)) getBox(unsigned number) const {
        return getSequence(Geometry::BOX, number);
    }
    Sequence& __attribute__((pure)) getBox(unsigned number) {
        return getSequence(Geometry::BOX, number);
    }

    Sequence& __attribute__((pure)) getBox(unsigned r, unsigned c) {
        return getBox(r*getBoxesPerRow() + c);
    }
    const Sequence& __attribute__((pure)) getBox(unsigned r, unsigned c) const {
        return getBox(r*getBoxesPerRow() + c);
    }

    Sequence& __attribute__((pure)) getBoxOf(unsigned row, unsigned column) {
        return getBox( getGeometry().getBoxNumber(row, column) );
    }
    const Sequence& __attribute__((pure)) getBoxOf(unsigned row, unsigned column) const {
        return getBox( getGeometry().getBoxNumber(row, column) );
    }

    const Sequence& __attribute__((pure)) operator[](unsigned i) const { return getRow(i); }
    Sequence& __attribute__((pure)) operator[](unsigned i) { return getRow(i); }

    void doAllSequences(const SequenceAction&);
    void doAllSquares(const SquareAction&);

    void push();
    bool pop();

    void dropStack();
    void clear();

    const Square& __attribute__((pure)) find(const Square &s) const {
        return getRow(s.getRowIndex())[s.getColumnIndex()];
    }
    Square& __attribute__((pure)) find(const Square &s) {
        return getRow(s.getRowIndex())[s.getColumnIndex()];
    }
    Square& __attribute__((pure)) find(unsigned i) {
        const Geometry& g = getGeometry();
        return getRow(g.getRowIndex(i))[g.getColumnIndex(i)];
    }

    size_type __attribute__((pure)) size() const { return getSquares().size(); }

    explicit Board(const Geometry&);
    Board(const Board&);
    ~Board();

    Board& operator=(const Board&);

    void dump(std::ostream&, const std::string&) const;

    static const char* getRating(int r) __attribute__((const));
    static const char* getRatingByIndex(int r) __attribute__((const));
    static int getRatingIndex(int r) __attribute__((const));

    static void randomize(SquareCollection&);
    static const std::string makeBoxName(const Geometry&, unsigned i) __attribute__((pure));
  };

  Board& Randomize(Board&);
  Board& RandomInit(Board&);
}

namespace std
{
  istream& operator>>(istream&, sudoku::Board&);
  ostream& operator<<(ostream&, const sudoku::Board&);
  ostream& operator<<(ostream&, const sudoku::Board::Exporter&);
}

#endif
