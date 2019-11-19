#ifndef PUZZLE_H
#define PUZZLE_H

#include <ostream>
#include <list>

extern "C"
{
#  include "support.h"
}

#include "strategy.h"
#include "geometry.h"
#include "solution.h"

namespace sudoku
{
  class Board;
  class Square;
  class Sequence;
  class Freedom;
  class Subset;
  class Bitmask;
  class SingleSequenceStrategy;
  class FindHiddenSubsets;
  class FindNakedSubsets;
  class FindMultiSequencePatterns;
  class Selector;
  class Pattern;

  struct Step;
  typedef std::list<Step>  StepCollection;

  typedef std::list<const Strategy*>  StrategyCollection;

  typedef std::list<Board>  BoardCollection;

  class Puzzle
  {
  public:
    typedef std::list<Sequence*>  SequenceCollection;
    typedef std::list<Square*>    SquareCollection;
    typedef unsigned              size_type;

    enum ColouringOption
    {
      NO_COLOURING = 0,
      USE_SIMPLE_COLOURING,
      USE_MULTICOLOURING
    };

    static const int DEFAULT_FORCED_SQUARE_SCORE = 1;
    static const int DEFAULT_COMPLETED_SEQUENCE_SCORE = 1;
    static const int DEFAULT_CROSS_SEQUENCE_CONSTRAINT_SCORE = 3;
    static const int DEFAULT_NUMBER_SUBSET_SCORE = 7;
    static const int DEFAULT_PATTERN_SCORE = 20;
    static const int DEFAULT_XYWING_SCORE = 50;
    static const int DEFAULT_XYZWING_SCORE = 50;
    static const int DEFAULT_COLOURING_SCORE = 40;
    static const int COLOUR_SCORE_INCREMENT = 10;
    static const int DEFAULT_GUESSWORK_SCORE = 100;

    static const unsigned DEFAULT_SIZE = 1;
    static const unsigned USE_XY_WINGS = 1;
    static const unsigned USE_XYZ_WINGS = 2;

  private:
    struct State
    {
      SquareCollection    fullSquares;
      SequenceCollection  fullSequences;

      unsigned hiddenSubsetSize;
      unsigned largestHiddenSubsetSize;

      unsigned nakedSubsetSize;
      unsigned largestNakedSubsetSize;

      unsigned patternSize;
      unsigned largestPatternSize;

      explicit State(unsigned sz=2,
                     unsigned lsz=DEFAULT_SIZE,
                     unsigned nsz=2,
                     unsigned lnsz=DEFAULT_SIZE,
                     unsigned psz=2,
                     unsigned lpsz=DEFAULT_SIZE);
    };

    class Rating
    {
    private:
      int m_score;

    public:
      Rating() : m_score(0) {}

      void addScore(int s) { m_score += s; }
      int getScore() const { return m_score; }
    };

    typedef std::list<State>  StateStack;

    Board&              m_board;

    SquareCollection    m_squares;
    SequenceCollection  m_sequences;
    StateStack          m_stack;

    StrategyCollection  m_strategies;
    StrategyCollection  m_newStrategies;

    const unsigned      m_hiddenSubsetSizeLimit;
    unsigned            m_maxHiddenSubsetSize;

    const unsigned      m_nakedSubsetSizeLimit;
    unsigned            m_maxNakedSubsetSize;

    const unsigned      m_patternSizeLimit;
    unsigned            m_maxPatternSize;

    ColouringOption     m_colouring;
    unsigned            m_wings;

    StepCollection      m_solution;
    Rating              m_rating;

    int                 m_forcedSquareScore;
    int                 m_completedSequenceScore;
    int                 m_crossSequenceConstraintScore;
    int                 m_hiddenSubsetScore;
    int                 m_nakedSubsetScore;
    int                 m_patternScore;
    int                 m_xywingScore;
    int                 m_xyzwingScore;
    int                 m_colouringScore;
    int                 m_guessworkScore;

    const Selector*     m_guessSelector;

    /* The default copy-constructor would not
     * handle all the object references correctly,
     * so disallow copy construction by making the
     * constructor private.
     */
    Puzzle(const Puzzle&);

    void completeSquare(SquareCollection::iterator);
    void completeSequence(SequenceCollection::iterator);

    const Board& getBoard() const { return m_board; }
    Board& getBoard() { return m_board; }

    const StateStack& getStack() const { return m_stack; }
    StateStack& getStack() { return m_stack; }

    const State& getState() const { return getStack().back(); }
    State& getState() { return getStack().back(); }

    bool guess(Square*, unsigned);

    void push();
    bool pop();

    /*
     * These are used by strategies that use sequences.
     */
    int processSequenceStrategies(TotalFreedom&);
    int processSingleSequenceStrategy(const Freedom&);
    int processMultipleSequenceStrategies(const TotalFreedom&);
    int reprocessSequenceStrategy(const Strategy&, const TotalFreedom&);

    int includeNewStrategy(const TotalFreedom&);
    int updateStrategies(const TotalFreedom&);
    int processSequences();

    void setLargestHiddenSubsetSize(unsigned s) {
        getState().largestHiddenSubsetSize = s;
    }

    void setLargestNakedSubsetSize(unsigned s) {
        getState().largestNakedSubsetSize = s;
    }

    void setLargestPatternSize(unsigned s) {
        getState().largestPatternSize = s;
    }

    /*
     * This is a strategy that doesn't use sequences.
     */
    int setForcedSquares();

  public:
    explicit Puzzle(Board &board);
    ~Puzzle();

    const Selector* getGuessSelector() const { return m_guessSelector; }
    const Selector* guessBy(const Selector *s) {
        const Selector *old = m_guessSelector;
        m_guessSelector = s;
        return old;
    }

    size_type getMaxNumber() const __attribute__((pure));

    const SquareCollection& getSquares() const { return m_squares; }
    SquareCollection& getSquares() { return m_squares; }

    const SequenceCollection& getSequences() const { return m_sequences; }
    SequenceCollection& getSequences() { return m_sequences; }

    const StepCollection& getSolution() const { return m_solution; }
    StepCollection& getSolution() { return m_solution; }
    void getSolution(StepCollection&);

    const StrategyCollection& getStrategies() const { return m_strategies; }
    StrategyCollection& getStrategies() { return m_strategies; }

    const StrategyCollection& getNewStrategies() const {
        return m_newStrategies;
    }
    StrategyCollection& getNewStrategies() { return m_newStrategies; }

    unsigned getHiddenSubsetSizeLimit() const {
        return m_hiddenSubsetSizeLimit;
    }

    unsigned getHiddenSubsetSize() const {
        return getState().hiddenSubsetSize;
    }
    void setHiddenSubsetSize(unsigned s);

    unsigned getMaxHiddenSubsetSize() const { return m_maxHiddenSubsetSize; }
    void setMaxHiddenSubsetSize(unsigned s);

    unsigned getLargestHiddenSubsetSize() const {
        return getState().largestHiddenSubsetSize;
    }

    unsigned getNakedSubsetSizeLimit() const { return m_nakedSubsetSizeLimit; }

    unsigned getNakedSubsetSize() const {
        return getState().nakedSubsetSize;
    }
    void setNakedSubsetSize(unsigned s);

    unsigned getMaxNakedSubsetSize() const { return m_maxNakedSubsetSize; }
    void setMaxNakedSubsetSize(unsigned s);

    unsigned getLargestNakedSubsetSize() const {
        return getState().largestNakedSubsetSize;
    }

    unsigned getPatternSizeLimit() const { return m_patternSizeLimit; }

    unsigned getPatternSize() const {
        return getState().patternSize;
    }
    void setPatternSize(unsigned s);

    unsigned getMaxPatternSize() const { return m_maxPatternSize; }
    void setMaxPatternSize(unsigned s);

    unsigned getLargestPatternSize() const {
        return getState().largestPatternSize;
    }

    void setColouring(ColouringOption c) { m_colouring = c; }
    ColouringOption getColouring() const { return m_colouring; }

    bool __attribute__((pure)) useColouring() const {
        return (getColouring() != NO_COLOURING);
    }
    bool __attribute__((pure)) useSimpleColouring() const {
        return useColouring();
    }
    bool __attribute__((pure)) useMultiColouring() const {
        return (getColouring() >= USE_MULTICOLOURING);
    }

    void setWings(unsigned w) { m_wings = w; }
    unsigned __attribute__((pure)) getWings() const { return m_wings; }

    bool __attribute__((pure)) useWings(unsigned w) const { return ((getWings() & w) != 0); }
    bool __attribute__((pure)) useXYWings() const { return useWings(USE_XY_WINGS); } 
    bool __attribute__((pure)) useXYZWings() const { return useWings(USE_XYZ_WINGS); } 

    bool solve();
    bool solveByDeduction();
    bool solveByBruteForce(const Selector&);
    bool __attribute__((pure)) completed() const { return getStack().empty(); }

    BoardCollection::size_type findAllSolutions(BoardCollection&,
                                                BoardCollection::size_type = 0);

    SequenceCollection& populate(SequenceCollection&,
                                 Geometry::Unit,
                                 bitmask_t);

    void addForcedSquare(const Square*, unsigned);
    void addCompletedSequence(const Square*, const Sequence*, unsigned);
    void addCrossSequenceConstraint(const Sequence*,
                                    const Sequence*,
                                    unsigned,
                                    const SquareCollection&);
    void addHiddenSubset(const Sequence*, const Subset&);
    void addNakedSubset(const Sequence*, const Subset&);
    void addMultiSequencePattern(const Pattern&, unsigned);
    void addXYWing(const Square*,
                   const Square*,
                   const Square*,
                   unsigned,
                   const SquareCollection&);
    void addXYZWing(const Square*,
                    const Square*,
                    const Square*,
                    unsigned,
                    const SquareCollection&);
    void addColouring(const SquareCollection&, Colouring::Type, unsigned);
    void addGuesswork(const Square*, unsigned);

    int __attribute__((pure)) getForcedSquareScore() const { return m_forcedSquareScore; }
    int __attribute__((pure)) getCompletedSequenceScore() const { return m_completedSequenceScore; }
    int __attribute__((pure)) getCrossSequenceConstraintScore() const {
          return m_crossSequenceConstraintScore;
    }
    int __attribute__((pure)) getHiddenSubsetScore() const { return m_hiddenSubsetScore; }
    int __attribute__((pure)) getNakedSubsetScore() const { return m_nakedSubsetScore; }
    int __attribute__((pure)) getMultiSequencePatternScore() const { return m_patternScore; }
    int __attribute__((pure)) getXYWingScore() const { return m_xywingScore; }
    int __attribute__((pure)) getXYZWingScore() const { return m_xyzwingScore; }
    int __attribute__((pure)) getColouringScore() const { return m_colouringScore; }
    int __attribute__((pure)) getGuessworkScore() const { return m_guessworkScore; }

    void setForcedSquareScore(int s) { m_forcedSquareScore = s; }
    void setCompletedSequenceScore(int s) { m_completedSequenceScore = s; }
    void setCrossSequenceConstraintScore(int s) {
           m_crossSequenceConstraintScore = s; 
    }
    void setHiddenSubsetScore(int s) { m_hiddenSubsetScore = s; }
    void setMultiSequencePatternScore(int s) { m_patternScore = s; }
    void setXYWingScore(int s) { m_xywingScore = s; }
    void setXYZWingScore(int s) { m_xyzwingScore = s; }
    void setColouringScore(int s) { m_colouringScore = s; }
    void setGuessworkScore(int s) { m_guessworkScore = s; }

    std::ostream& showSolution(std::ostream &ostr) const;

    int __attribute__((pure)) getScore() const { return m_rating.getScore(); }
    int getRating() const __attribute__((pure));

    void dumpBoard(std::ostream&, const std::string&) const;

    friend class SingleSequenceStrategy;
    friend class FindHiddenSubsets;
    friend class FindNakedSubsets;
    friend class FindMultiSequencePatterns;
  };
}

namespace std
{
  inline ostream&
  operator<<(ostream &ostr, const sudoku::Puzzle &puzzle) {
      return puzzle.showSolution(ostr);
  }
}

#endif

