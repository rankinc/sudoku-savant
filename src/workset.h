#ifndef WORKSET_H
#define WORKSET_H

#include <vector>

#include "subsets.h"
#include "bitmask.h"

namespace sudoku
{
  class Freedom;
  class Sequence;

  typedef std::list<const Freedom*> WorkFreedom;

  class WorkSet
  {
  public:
    typedef std::vector<SubsetCollection>  WorkCollection;
    typedef WorkCollection::size_type  size_type;

  private:
    WorkCollection m_work;
    size_type      m_setSize;

    void add(unsigned number, const Bitmask&);
    void promote(const Subset&);

    static void add(SubsetCollection&, const Subset&);
    static bool merge(Subset&, const Subset&);

  public:
    WorkSet(size_type subsetSize, const Freedom&);
    WorkSet(size_type subsetSize, const Sequence*);
    WorkSet(size_type subsetSize, unsigned, const WorkFreedom&);
    ~WorkSet();

    const SubsetCollection& operator[](int i) const {
        return m_work[ getIndex(i) ];
    }

    SubsetCollection& operator[](int i) {
        return m_work[ getIndex(i) ];
    }

    void add(const Subset&);
    SubsetCollection& findSubsets(SubsetCollection&);

    size_type size() const __attribute__((pure)) { return m_work.size(); }
    size_type getSetSize() const __attribute__((pure)) { return m_setSize; }

    static size_type getIndex(size_type s) __attribute__((const)) { return (s - 2); }
  };
}

#endif
