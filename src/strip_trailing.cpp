#include <cctype>

#undef isspace

#include "strip_blanks.h"

using namespace std;


/********************************************************************
 * Remove trailing whitespace
 */
string&
stripTrailing(string &s)
{
  string::size_type i = s.length();
  while ( i && isspace( static_cast<int>(s[i - 1]) ) ) --i;
  s.erase(i);

  return s;
}

