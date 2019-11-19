#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <string>
#include <istream>

#include "puzzle.h"
#include "board.h"
#include "note.h"
#include "actions.h"

namespace sudoku
{
  typedef Step  Hint;
  typedef StepCollection  HintCollection;
  typedef HintCollection::const_iterator  HintIterator;

  class Generator;

  class Document
  {
  public:
    typedef Board::SquareCollection  SquareCollection;
    typedef BoardCollection::size_type  size_type;
    typedef Puzzle::ColouringOption ColouringOption;

    typedef Puzzle::SquareCollection  RefCollection;
    typedef RefAction<Square>         SquareRefAction;

    class Moves
    {
    private:
      RefCollection  m_moves;

    public:
      void add(Square *s) { m_moves.push_back(s); }
      void removeLatest() { m_moves.pop_back(); }
      Square* getLatest() const {
          return (m_moves.empty() ? NULL : m_moves.back());
      }
      void clear() { m_moves.clear(); }
      size_type count() const { return m_moves.size(); }

      const RefCollection& getMoves() const { return m_moves; }
    };

    typedef std::vector<Note>  NoteCollection;

    class NoteAction
    {
    public:
      virtual ~NoteAction() {}
      virtual void operator()(Note&, Note::size_type) const = 0;
    };

    class NoteBook
    {
    public:
      typedef NoteCollection::size_type  size_type;

    private:
      const Geometry& m_geometry;
      NoteCollection m_notes;

    public:
      explicit NoteBook(const Geometry&);
      void clear();

      const Geometry& __attribute__((pure)) getGeometry() const { return m_geometry; }

      const Note& __attribute__((pure)) getNoteFor(const Square &s) const {
          return m_notes[s.getSquareIndex()];
      }
      Note& __attribute__((pure)) getNoteFor(const Square &s) {
          return m_notes[s.getSquareIndex()];
      }

      size_type __attribute__((pure)) size() const { return m_notes.size(); }

      const Note& __attribute__((pure)) operator[](int i) const { return m_notes[i]; }
      Note& __attribute__((pure)) operator[](int i) { return m_notes[i]; }

      void do_for_each(const NoteAction&);

      bool readMarks(std::istream&);
      bool readColour(std::istream&);
    };

  private:
    BoardCollection m_solutions;
    Board*          m_original;
    Board*          m_board;

    bool            m_hasLogical;
    StepCollection  m_solutionLog;

    Moves           m_moves;
    RefCollection   m_givenHints;

    HintCollection  m_allHints;
    HintIterator    m_hint;

    bool            m_hintsAreStale;

    NoteBook*       m_notebook;

    unsigned        m_maxHiddenSubsetSize;
    unsigned        m_maxNakedSubsetSize;
    unsigned        m_maxPatternSize;
    ColouringOption m_colouring;
    unsigned        m_wings;

    bool __attribute__((pure)) hintsAreStale() const { return m_hintsAreStale; }
    void staleHints() { m_hintsAreStale = true; }
    void freshHints() { m_hintsAreStale = false; }

    bool solve(Board&, BoardCollection&, StepCollection* =NULL);
    bool solve();

    void loadPlayState(std::istream &istr);
    void generateHints();

    static bool deleteMistakes(HintCollection&,
                               HintCollection::iterator,
                               HintCollection::iterator&);
    static void deleteMistakes(HintCollection&);

    static void throw_illegal_option(const char*, const Square&);

  public:
    static const size_type UPPER_SOLUTION_LIMIT = 5;

    Document();
    ~Document();

    Board* __attribute__((pure)) getBoard() const { return m_board; }
    Board* __attribute__((pure)) getOriginalBoard() const { return m_original; }
    const Geometry& __attribute__((pure)) getGeometry() const { return m_board->getGeometry(); }
    NoteBook* __attribute__((pure)) getNoteBook() const { return m_notebook; }

    const Note& __attribute__((pure)) getNoteFor(const Square &s) const {
        return m_notebook->getNoteFor(s);
    }
    Note& __attribute__((pure)) getNoteFor(const Square &s) {
        return m_notebook->getNoteFor(s);
    }

    const BoardCollection& __attribute__((pure)) getSolutions() const { return m_solutions; }
    size_type __attribute__((pure)) getSolutionCount() const { return m_solutions.size(); }
    bool __attribute__((pure)) hasLogicalSolution() const { return m_hasLogical; }

    bool __attribute__((pure)) hasUniqueSolution() const { return (m_solutions.size() == 1); }
    const Board& __attribute__((pure)) getUniqueSolution() const { return m_solutions.front(); }

    const StepCollection& __attribute__((pure)) getSolutionLog() const { return m_solutionLog; }

    const char* __attribute__((pure)) getRating() const { return getRating(m_solutions); }
    const std::string& __attribute__((pure)) getStatus(std::string &s) const {
        return getStatus(s, hasLogicalSolution(), getSolutionCount());
    }

    bool __attribute__((pure)) isComplete() const {
        return isComplete( getBoard() );
    }

    unsigned __attribute__((pure)) getSubsetSizeLimit() const {
        return getGeometry().getSubsetSizeLimit();
    }
    unsigned __attribute__((pure)) getMaxHiddenSubsetSize() const { return m_maxHiddenSubsetSize; }
    void setMaxHiddenSubsetSize(unsigned s) { m_maxHiddenSubsetSize = s; }

    unsigned __attribute__((pure)) getMaxNakedSubsetSize() const { return m_maxNakedSubsetSize; }
    void setMaxNakedSubsetSize(unsigned s) { m_maxNakedSubsetSize = s; }

    unsigned __attribute__((pure)) getPatternSizeLimit() const {
        return getGeometry().getPatternSizeLimit();
    }
    unsigned __attribute__((pure)) getMaxPatternSize() const { return m_maxPatternSize; }
    void setMaxPatternSize(unsigned s) { m_maxPatternSize = s; }

    ColouringOption __attribute__((pure)) getColouring() const { return m_colouring; }
    void setColouring(ColouringOption c) { m_colouring = c; }

    unsigned __attribute__((pure)) getWings() const { return m_wings; }
    void setWings(unsigned w) { m_wings = w; }

    Board& recreate(const Geometry&);

    void load(std::istream&);
    void savePlay(std::ostream&) const;
    void savePuzzle(std::ostream&) const;
    bool resetSolution();
    void reset();
    void finalise();
    void generate(int target=0);

    void addMove(Square&, unsigned);
    Square* removeLatestMove();
    size_type __attribute__((pure)) countMoves() const { return m_moves.count(); }

    void do_for_each_move(const SquareRefAction&) const;
    void do_for_each_hint(const SquareRefAction&) const;
    void do_for_each_note(const NoteAction &action) {
        return m_notebook->do_for_each(action);
    }

    const Hint *getHint();
    bool __attribute__((pure)) hasHints() const { return (m_hint != m_allHints.end()); }
    void addHint(Square&, unsigned);

    void clearMarks(const Square&);

    bool validate(const Board*, BoardCollection&);
    bool isErrorMove(const Square&) const __attribute__((pure));
    static bool isErrorMove(const Board&, const Square&) __attribute__((pure));

    std::string& generateFileName(std::string&) const;
    static const char* getRating(const BoardCollection&) __attribute__((pure));
    static void validate(const Board*);
    static const std::string& getStatus(std::string&, bool, size_type);
    static bool isComplete(const Board*) __attribute__((pure));
  };
}

#endif

