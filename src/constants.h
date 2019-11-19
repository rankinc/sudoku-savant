#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>
#include <istream>
#include <stdexcept>

#ifdef DEBUG
#  define TRACE(a)  a
#else
#  define TRACE(a)
#endif

#define ARRAY_SIZE(a)  (sizeof(a) / sizeof((a)[0]))

namespace sudoku
{
  typedef unsigned long  bitmask_t;
  typedef int colour_type;

  static const char COMMENT_MARKER = '#';
  static const std::string::size_type MAX_LINE_SIZE = 128;

  static const colour_type MAX_COLOURS = 9;

  class Board;
  class Square;
  class Bitmask;

  struct StringPair
  {
    std::string name;
    std::string value;
  };

  inline void setBit(bitmask_t *bits, unsigned number)
  {
    *bits |= (1 << number);
  }

  inline void clearBit(bitmask_t *bits, unsigned number)
  {
    *bits &= ~(1 << number);
  }

  inline bool __attribute__((const)) isSet(bitmask_t bits, unsigned number)
  {
    return (bits & (1 << number)) != 0;
  }

  bool validate(const Board&, Bitmask&);

  void load(Board&, const std::string&);

  const std::string makeName(const char *tag, unsigned number) __attribute__((const));

  std::string& getLine(std::istream &istr, std::string &s);

  std::string& uppercase(std::string&);

  bool getOption(std::string&, StringPair&);

  unsigned* shuffle(unsigned[], size_t);

  unsigned* generateArray(size_t, unsigned);

  inline unsigned*
  generateRandomArray(size_t size, unsigned start)
  {
    return shuffle(generateArray(size, start), size);
  }


  /*
   * Not really an auto_ptr<T> type, but just enough
   * to call the correct delete[] operator in the
   * destructor.
   */
  template<typename T>
  class auto_array_ptr
  {
  private:
    T *m_ptr;

  public:
    explicit auto_array_ptr(T *x) : m_ptr(x) {}
    ~auto_array_ptr() { delete[] m_ptr; }

    const T& operator[](int i) const { return m_ptr[i]; }
    T& operator[](int i) { return m_ptr[i]; }

    T* get() const { return m_ptr; }
  };


  class violation : public std::runtime_error
  {
  private:
    const Square *m_square;

  public:
    const Square* getSquare() const { return m_square; }

    violation(const Square *s, const std::string &msg)
      : runtime_error(msg)
      , m_square(s) {}
  };
}

#endif
