#ifndef SOLUTION_LOG_H
#define SOLUTION_LOG_H

#include <string>
#include "textlog.h"
#include "puzzle.h"

namespace sudoku
{
  extern "C"
  {
    void on_reveal_solution_clicked(GtkButton*, gpointer);
    void on_hide_solution_clicked(GtkButton*, gpointer);
    void on_show_all_clicked(GtkButton*, gpointer);
    void on_hide_all_clicked(GtkButton*, gpointer);
  }

  class Window;

  class SolutionLog : public TextLog
  {
  private:
    GtkWidget*     m_buttonBox;
    GtkWidget*     m_revealButton;
    GtkWidget*     m_hideButton;
    GtkWidget*     m_showAllButton;
    GtkWidget*     m_hideAllButton;

    StepCollection::const_iterator m_firstLine;
    StepCollection::const_iterator m_endLine;
    StepCollection::const_iterator m_line;

  public:
    explicit SolutionLog(Window*);
    ~SolutionLog();

    const GtkTextView* getGtkTextView() const {
        return GTK_TEXT_VIEW(getGtkWidget());
    }

    GtkTextView* getGtkTextView() {
        return GTK_TEXT_VIEW(getGtkWidget());
    }

    void enableHideButton() { enableWidget(m_hideButton); }
    void disableHideButton() { disableWidget(m_hideButton); }

    void enableRevealButton() { enableWidget(m_revealButton); }
    void disableRevealButton() { disableWidget(m_revealButton); }

    void enableShowAllButton() { enableWidget(m_showAllButton); }
    void disableShowAllButton() { disableWidget(m_showAllButton); }

    void enableHideAllButton() { enableWidget(m_hideAllButton); }
    void disableHideAllButton() { disableWidget(m_hideAllButton); }

    void enableButtons() { enableWidget(m_buttonBox); }
    void disableButtons() { disableWidget(m_buttonBox); }

    void enableReveal();
    void disableReveal();

    void enableHide();
    void disableHide();

    void revealLine();
    void hideLine();

    void showAll();
    void hideAll();

    void load(const StepCollection&);
    void clear();
  };
}

#endif
