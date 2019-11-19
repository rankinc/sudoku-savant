#ifndef FREEDOM_H
#define FREEDOM_H

#include <cstdlib>
#include <ostream>

#include "subsets.h"
#include "bitmask.h"

namespace sudoku
{
  class Sequence;

  class Freedom
  {
  public:
    typedef unsigned  size_type;

  private:
    const size_type m_size;
    Bitmask *m_bitmask;

    Sequence *m_sequence;

    void calculate();

  public:
    explicit Freedom(Sequence*);
    Freedom(const Freedom&);
    ~Freedom();

    const Bitmask& __attribute__((pure)) operator[](size_type i) const {
        TRACE( if (i < size()) ; else abort();  )
        return m_bitmask[i];
    }
    Bitmask& __attribute__((pure)) operator[](size_type i) {
        TRACE( if (i < size()) ; else abort();  )
        return m_bitmask[i];
    }

    size_type __attribute__((pure)) size() const { return m_size; }
    Sequence* __attribute__((pure)) getSequence() const { return m_sequence; }

    size_type getSubsetSizeCeiling(size_type) const __attribute__((pure));

    bool findHiddenSubsets(unsigned, SubsetCollection&) const;
    bool findNakedSubsets(unsigned, SubsetCollection&) const;
  };
}

namespace std
{
  ostream& operator<<(ostream&, const sudoku::Freedom&);
}

#endif
