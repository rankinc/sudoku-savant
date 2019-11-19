#ifndef GENERATOR_H
#define GENERATOR_H

#include <string>
#include <climits>

#include "puzzle.h"

namespace sudoku
{
  class Board;

  class Generator
  {
  public:
    typedef Puzzle::ColouringOption  ColouringOption;

    static const unsigned DEFAULT_SIZE = Puzzle::DEFAULT_SIZE;
    static const unsigned DEFAULT_WINGS = UINT_MAX;
    static const ColouringOption DEFAULT_COLOURING = Puzzle::USE_MULTICOLOURING;

  private:
    unsigned        m_maxHiddenSubsetSize;
    unsigned        m_maxNakedSubsetSize;
    unsigned        m_maxPatternSize;
    ColouringOption m_colouring;
    unsigned        m_wings;

    bool createNewSolution(Board&);
    Board& prune(Board&);

  public:
    explicit Generator(unsigned ssz=DEFAULT_SIZE,
                       unsigned nsz=DEFAULT_SIZE,
                       unsigned psz=DEFAULT_SIZE,
                       ColouringOption c=DEFAULT_COLOURING,
                       unsigned w=DEFAULT_WINGS)
      : m_maxHiddenSubsetSize(ssz)
      , m_maxNakedSubsetSize(nsz)
      , m_maxPatternSize(psz)
      , m_colouring(c)
      , m_wings(w)
    {}

    Puzzle& parameterise(Puzzle&) const;

    void setMaxHiddenSubsetSize(unsigned sz) { m_maxHiddenSubsetSize = sz; }
    unsigned __attribute__((pure)) getMaxHiddenSubsetSize() const { return m_maxHiddenSubsetSize; }

    void setMaxNakedSubsetSize(unsigned sz) { m_maxNakedSubsetSize = sz; }
    unsigned __attribute__((pure)) getMaxNakedSubsetSize() const { return m_maxNakedSubsetSize; }

    void setMaxPatternSize(unsigned sz) { m_maxPatternSize = sz; }
    unsigned __attribute__((pure)) getMaxPatternSize() const { return m_maxPatternSize; }

    void setWings(unsigned w) { m_wings = w; }
    unsigned __attribute__((pure)) getWings() const { return m_wings; }

    void setColouring(ColouringOption c) { m_colouring = c; }
    ColouringOption __attribute__((pure)) getColouring() const { return m_colouring; }

    bool generate(Board&);

    static std::string& generateComment(std::string&);
  };
}

#endif
