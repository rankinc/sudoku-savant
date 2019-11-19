#include <cctype>

#undef isspace

#include "strip_blanks.h"

using namespace std;


/********************************************************************
 * Remove leading whitespace
 */
string&
stripLeading(string &s)
{
  const string::size_type len = s.length();
  string::size_type i = 0;
  while ( (i < len) && isspace( static_cast<int>(s[i]) ) ) ++i;
  s.erase(0, i);

  return s;
}

