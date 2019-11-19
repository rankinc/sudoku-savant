#ifndef PATTERN_H
#define PATTERN_H

#include <list>
#include "bitmask.h"

namespace sudoku
{
  class Sequence;

  struct Pattern
  {
  public:
    typedef std::list<Sequence*>  SequenceCollection;
    typedef Bitmask::size_type    size_type;

    SequenceCollection sequences;
    Bitmask positions;

    Pattern(const Bitmask &b) : positions(b) {}

    size_type __attribute__((pure)) size() const { return positions.size(); }
  };
}

#endif

