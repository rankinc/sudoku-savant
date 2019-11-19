#ifndef STRIP_BLANKS_H
#define STRIP_BLANKS_H

#include <string>

std::string& stripLeading(std::string&);
std::string& stripTrailing(std::string&);

inline std::string&
stripBlanks(std::string &s)
{
  return stripLeading( stripTrailing(s) );
}

#endif

