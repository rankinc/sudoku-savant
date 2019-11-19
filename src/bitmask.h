#ifndef BITMASK_H
#define BITMASK_H

#include "constants.h"

namespace sudoku
{
  class Bitmask
  {
  public:
    typedef std::size_t size_type;

  private:
    bitmask_t  m_bits;
    size_type  m_count;

    Bitmask(const bitmask_t bits, size_type count);

  public:
    Bitmask();

    void clear() { m_bits = 0; m_count = 0; }

    bitmask_t __attribute__((pure)) getBits() const { return m_bits; }
    // bitmask_t& getBits() { return m_bits; }

    std::ostream& write(std::ostream &ostr,
                        const char openBracket,
                        const char closeBracket) const;

    size_type size() const { return m_count; }
    bool empty() const { return (size() == 0); }

    bool setBit(size_type i);
    bool clearBit(size_type i);
    bool __attribute__((pure)) isSet(size_type i) const { return isSet(getBits(), i);  }
    bool __attribute__((pure)) isClear(size_type i) const { return !isSet(i); }

    size_type getBits(size_type b[], size_type max) const {
        return getBits( getBits(), b, max );
    }

    size_type getHighestBit() const {
        return getHighestBit( getBits() );
    }

    bool operator==(const Bitmask &b) const {
      return (m_bits == b.m_bits);
    }

    bool operator==(const bitmask_t bits) const {
      return (m_bits == bits);
    }

    bitmask_t operator|(const Bitmask &b) const {
      return (m_bits | b.m_bits);
    }

    bitmask_t operator&(const Bitmask &b) const {
      return (m_bits & b.m_bits);
    }

    Bitmask& operator=(const Bitmask&);
    Bitmask& operator|=(const Bitmask&);

    static Bitmask createFromBits(const bitmask_t);
    static Bitmask createNumberMask(size_type);

    static size_type countBits(bitmask_t) __attribute__((const));
    static size_type getHighestBit(bitmask_t) __attribute__((const));
    static bitmask_t __attribute__((const)) getBit(size_type i) { return (1 << i); }
    static bool __attribute__((const)) isSet(bitmask_t b, size_type i) { return (b & getBit(i)) != 0; }
    static bool __attribute__((const)) isClear(bitmask_t b, size_type i) { return !isSet(b, i); }
    static bool setBit(bitmask_t&, size_type);
    static bool clearBit(bitmask_t&, size_type);
    static size_type getBits(bitmask_t, size_type[], size_type);
  };

  class Subset
  {
  public:
    typedef Bitmask::size_type  size_type;

  private:
    Bitmask m_squares;
    Bitmask m_numbers;

  public:
    void setSquares(const Bitmask &b) { m_squares = b; }
    const Bitmask& getSquares() const { return m_squares; }
    Bitmask& getSquares() { return m_squares; }

    void setNumbers(const Bitmask &b) { m_numbers = b; }
    const Bitmask& getNumbers() const { return m_numbers; }
    Bitmask& getNumbers() { return m_numbers; }

    void addNumber(size_type n) { m_numbers.setBit(n); }
    void addSquare(size_type n) { m_squares.setBit(n); }

    size_type countSquares() const { return m_squares.size(); }
    size_type countNumbers() const { return m_numbers.size(); }

    void clear() { m_squares.clear(); m_numbers.clear(); }

    bool isClosed() const {
        return (m_squares.size() == m_numbers.size());
    }

    bool operator==(const Subset &s) const {
        return (m_squares == s.m_squares) && (m_numbers == s.m_numbers);
    }

    Subset& operator|=(const Subset &s);

    Subset(const Bitmask &n, const Bitmask &s)
      : m_squares(s)
      , m_numbers(n)
    {}
        
    Subset() {}
  };
}

namespace std
{
  ostream& operator<<(ostream&, const sudoku::Bitmask&);
}

#endif

