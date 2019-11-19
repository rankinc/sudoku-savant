#ifndef DATE_H
#define DATE_H

#include <ctime>
#include <ostream>
using std::time_t;
using std::tm;

namespace sudoku
{
  class Date
  {
  private:
    time_t m_time;
    struct tm m_date;

  public:
    Date();

    const time_t& getTime() const { return m_time; }
    const struct tm& getDate() const { return m_date; }
  };

  class DateFormat
  {
  private:
    Date        m_date;
    const char *m_format;
    size_t      m_size;
    char       *m_buffer;

  public:
    DateFormat(const Date &d,
               const char *format=DEFAULT,
               size_t size=DEFAULT_SIZE);
    ~DateFormat();
    const char *getDate() const;

    static const char DEFAULT[];
    static const size_t DEFAULT_SIZE = 24;
  };
}

namespace std
{
  inline ostream&
  operator<<(ostream &ostr, const sudoku::DateFormat &d) {
      return (ostr << d.getDate());
  }
}

#endif
