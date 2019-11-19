#include "date.h"
using namespace std;

namespace sudoku
{

Date::Date()
  : m_time(time(&m_time))
{
  localtime_r(&m_time, &m_date);
}


const char DateFormat::DEFAULT[] = "%Y-%b-%d %T";

DateFormat::DateFormat(const Date &d, const char *format, size_t s)
  : m_date(d)
  , m_format(format)
  , m_size(s)
  , m_buffer( new char[s] )
{}

DateFormat::~DateFormat()
{
  delete[] m_buffer;
}

const char*
DateFormat::getDate() const
{
  strftime(m_buffer, m_size, m_format, &(m_date.getDate()));
  return m_buffer;
}

}
