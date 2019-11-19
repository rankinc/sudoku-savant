#include "solutionlog.h"
#include "window.h"
#include "solution.h"

extern "C"
{
  #include "support.h"
}

namespace sudoku
{

void
on_reveal_solution_clicked(GtkButton*, gpointer user_data)
{
  SolutionLog *log = static_cast<SolutionLog*>(user_data);
  log->revealLine();
}

void
on_hide_solution_clicked(GtkButton*, gpointer user_data)
{
  SolutionLog *log = static_cast<SolutionLog*>(user_data);
  log->hideLine();
}

void
on_show_all_clicked(GtkButton*, gpointer user_data)
{
  SolutionLog *log = static_cast<SolutionLog*>(user_data);
  log->showAll();
}

void
on_hide_all_clicked(GtkButton*, gpointer user_data)
{
  SolutionLog *log = static_cast<SolutionLog*>(user_data);
  log->hideAll();
}


/*********************************************************************
 */
SolutionLog::SolutionLog(Window *w)
  : TextLog(w, lookup_widget(w->getGtkWidget(), "solution_view"))
  , m_buttonBox( lookup_widget(w->getGtkWidget(), "solution_buttonbox") )
  , m_revealButton( lookup_widget(w->getGtkWidget(), "reveal_button") )
  , m_hideButton( lookup_widget(w->getGtkWidget(), "hide_button") )
  , m_showAllButton( lookup_widget(w->getGtkWidget(), "showall_button") )
  , m_hideAllButton( lookup_widget(w->getGtkWidget(), "hideall_button") )
{
  g_signal_connect(static_cast<gpointer>(m_revealButton),
                   "clicked",
                   G_CALLBACK(on_reveal_solution_clicked),
                   this);

  g_signal_connect(static_cast<gpointer>(m_hideButton),
                   "clicked",
                   G_CALLBACK(on_hide_solution_clicked),
                   this);

  g_signal_connect(static_cast<gpointer>(m_showAllButton),
                   "clicked",
                   G_CALLBACK(on_show_all_clicked),
                   this);

  g_signal_connect(static_cast<gpointer>(m_hideAllButton),
                   "clicked",
                   G_CALLBACK(on_hide_all_clicked),
                   this);
}


/*********************************************************************
 */
SolutionLog::~SolutionLog() {}


/*********************************************************************
 */
void
SolutionLog::clear()
{
  TextLog::clear();
  disableButtons();
}


/*********************************************************************
 */
void
SolutionLog::load(const StepCollection &log)
{
  m_line = m_firstLine = log.begin();
  m_endLine = log.end();

  enableButtons();
  disableReveal();
  disableHide();

  if (m_line != m_endLine)
  {
    enableReveal();
  }
}


/*********************************************************************
 */
void
SolutionLog::revealLine()
{
  if (m_line != m_endLine)
  {
    show(m_line->message);

    if (m_line == m_firstLine)
      enableHide();

    if (++m_line == m_endLine)
      disableReveal();
  }
}


/*********************************************************************
 */
void
SolutionLog::hideLine()
{
  const gint lineCount = getLineCount();

  if (lineCount > 1)
  {
    eraseLine(lineCount - 2);

    if (lineCount == 2)
      disableHide();

    if (--m_line != m_endLine)
      enableReveal();
  }
}


/*********************************************************************
 */
void
SolutionLog::showAll()
{
  while (m_line != m_endLine)
  {
    writeLine(m_line->message);
    ++m_line;
  } /* while */

  disableReveal();
  enableHide();
}


/*********************************************************************
 */
void
SolutionLog::hideAll()
{
  gint lineCount = getLineCount();

  while (lineCount > 1)
  {
    eraseLine(lineCount - 2);
    --m_line;
    --lineCount;
  } /* while */

  disableHide();
  enableReveal();
}


/*********************************************************************
 */
void
SolutionLog::enableHide()
{
  enableHideButton();
  enableHideAllButton();
}

void
SolutionLog::disableHide()
{
  disableHideButton();
  disableHideAllButton();
}

void
SolutionLog::enableReveal()
{
  enableRevealButton();
  enableShowAllButton();
}

void
SolutionLog::disableReveal()
{
  disableRevealButton();
  disableShowAllButton();
}

}

