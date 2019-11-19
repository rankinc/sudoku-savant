#include <cctype>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <iostream>
#include <algorithm>

#include "constants.h"
#include "strip_blanks.h"
using namespace std;


namespace sudoku
{

const string
makeName(const char *tag, unsigned number)
{
  ostringstream ostr;
  ostr << tag << number;
  return ostr.str();
}


string&
getLine(istream &istr, string &s)
{
  char buffer[MAX_LINE_SIZE];

  s.clear();

  while ( istr.getline(buffer, sizeof(buffer)) )
  {
    /*
     * Delete any comment lines ...
     */
    char *const comment = strchr(buffer, COMMENT_MARKER);
    if ( comment ) *comment = '\0';

    /*
     * Ignore empty lines ...
     */
    if ( *buffer )
    {
        stripBlanks( (s = buffer) );
        if ( !s.empty() )
          break;
    }
  } /* while */

  return s;
}


string&
uppercase(string &s)
{
  transform(s.begin(), s.end(), s.begin(),
            static_cast<int(*)(int)>(toupper));
  return s;
}


bool
getOption(string &line, StringPair &option)
{
  const string::size_type delim = uppercase(line).find(':', 0);
  bool isFound;

  if ( (isFound = (delim != string::npos)) )
  {
    option.name.assign( line.substr(0, delim) );
    option.value.assign( line.substr(delim + 1, string::npos) );

    stripBlanks(option.name);
    stripBlanks(option.value);
  }

  return isFound;
}


unsigned*
shuffle(unsigned array[], size_t size)
{
  if (size > 1)
  {
    for (size_t i = 0; i < size - 1; ++i)
    {
      const size_t j = i + (random() % (size - i));

      if (j != i)
      {
        const unsigned tmp = array[i];
        array[i] = array[j];
        array[j] = tmp;
      }
    } /* for */
  }

  return array;
}


unsigned*
generateArray(size_t size, unsigned start)
{
  unsigned *array = new unsigned[size];

  for (unsigned i = 0; i < size; ++i)
    array[i] = start++;

  return array;
}

}
