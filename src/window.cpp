#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>
using namespace std;

#include "window.h"
#include "document.h"
#include "numberpad.h"
#include "toolbar.h"
#include "solution.h"

extern "C"
{
  #include "interface.h"
  #include "support.h"
}

namespace sudoku
{

const char copyright_notice[] = "(C) 2005-2014 Chris Rankin";

const char translator_credit[] = "Translators:\n"
  "  German (de) - Mario Blättermann <mario.blaettermann@t-online.de>\n"
  "  French (fr) - Christophe Lincoln <pankso@slitaz.org>\n"
  "  Czech  (cs) - Pavel Fric <pavelfric@seznam.cz>";


void
on_open1_activate(GtkMenuItem*, gpointer user_data)
{
  Window *window = static_cast<Window*>(user_data);
  window->showOpenDialog();
}


void
on_save_play1_activate(GtkMenuItem*, gpointer user_data)
{
  Window *window = static_cast<Window*>(user_data);
  window->save();
}


void
on_save_play_as1_activate(GtkMenuItem*, gpointer user_data)
{
  Window *window = static_cast<Window*>(user_data);
  window->showSaveDialog(SAVE_PLAY);
}


void
on_save_puzzle1_activate(GtkMenuItem*, gpointer user_data)
{
  Window *window = static_cast<Window*>(user_data);
  window->showSaveDialog(SAVE_PUZZLE);
}


void
on_quit1_activate(GtkMenuItem*, gpointer)
{
  gtk_main_quit();
}


void
on_about1_activate(GtkMenuItem*, gpointer user_data)
{
  Window *window = static_cast<Window*>(user_data);
  window->showAboutBox();
}


void
on_strategies_activate(GtkMenuItem*, gpointer user_data)
{
  Window *window = static_cast<Window*>(user_data);
  window->showStrategyDialog();
}


void
on_solutionlog_activate(GtkMenuItem *item, gpointer user_data)
{
  Window *window = static_cast<Window*>(user_data);

  if ( reinterpret_cast<GtkCheckMenuItem*>(item)->active )
    window->showLogBook();
  else
    window->hideLogBook();
}


void
on_highlight_activate(GtkMenuItem *item, gpointer user_data)
{
  Window *window = static_cast<Window*>(user_data);
  window->setHighlightingErrors(
                         reinterpret_cast<GtkCheckMenuItem*>(item)->active );
}


void
on_geometry_activate(GtkMenuItem*, gpointer user_data)
{
  GeoDialog *geo = static_cast<GeoDialog*>(user_data);
  geo->run();
}


void
on_min_rating_activate(GtkMenuItem *item, gpointer user_data)
{
  const Window::RatingChoice *choice
                        = static_cast<Window::RatingChoice*>(user_data);
  if ( reinterpret_cast<GtkCheckMenuItem*>(item)->active )
    choice->mainWindow->setMinimumRating( choice->ratingIndex );
}


void
on_reset_activate(GtkMenuItem*, gpointer user_data)
{
  Window *window = static_cast<Window*>(user_data);
  window->reset();
}


void
on_autogenerate_clicked(GtkButton*, gpointer user_data)
{
  Window *mainWindow = static_cast<Window*>(user_data);
  mainWindow->generate();
}


void
on_manualgenerate_clicked(GtkButton*, gpointer user_data)
{
  Window *mainWindow = static_cast<Window*>(user_data);
  mainWindow->toggleManualGenerate();
}


void
on_manualcheck_clicked(GtkButton*, gpointer user_data)
{
  Window *mainWindow = static_cast<Window*>(user_data);
  mainWindow->validateManualGenerate();
}


void
on_solve_clicked(GtkButton*, gpointer user_data)
{
  Window *mainWindow = static_cast<Window*>(user_data);
  mainWindow->solve();
}


void
on_backbutton_clicked(GtkButton*, gpointer user_data)
{
  Window *mainWindow = static_cast<Window*>(user_data);
  mainWindow->onBackArrow();
}


void
on_forwardbutton_clicked(GtkButton*, gpointer user_data)
{
  Window *mainWindow = static_cast<Window*>(user_data);
  mainWindow->onForwardArrow();
}


/*********************************************************************
 */
void
FileLoader::operator()(Document *d) const
{
  d->load(*m_istr);
}


/*********************************************************************
 */
void
CodeLoader::operator()(Document *d) const
{
  load(*(d->getBoard()), m_code);
  d->finalise();
}


/*********************************************************************
 */
void
Window::RatingChoice::configure(guint maxNum)
{
  const bool enabled = isEnabled();

  if ( (maxNum >= 9) ^ enabled )
  {
    if (enabled)
    {
      disable();
    }
    else
    {
      enable();
    }
  }
}


/*********************************************************************
 */
void
Window::createOptionsMenu()
{
  GtkWidget *optionsMenu = lookup_widget( getGtkWidget(), "menuitem2_menu" );

  createRatingMenu(optionsMenu);

  addSeparator(optionsMenu);

  gtk_container_add( GTK_CONTAINER(optionsMenu), m_resetMenu);
  gtk_widget_show(m_resetMenu);
  gtk_widget_set_sensitive(m_resetMenu, FALSE);
  g_signal_connect( static_cast<gpointer>(m_resetMenu),
                    "activate",
                    G_CALLBACK(on_reset_activate),
                    this );

  addSeparator(optionsMenu);

  GtkWidget *strategies = gtk_menu_item_new_with_mnemonic(
                                                     _("Select _Strategies") );
  gtk_container_add( GTK_CONTAINER(optionsMenu), strategies );
  gtk_widget_show(strategies);
  g_signal_connect( static_cast<gpointer>(strategies),
                    "activate",
                    G_CALLBACK(on_strategies_activate),
                    this );
}


/*********************************************************************
 */
void
Window::createRatingMenu(GtkWidget *optionsMenu)
{
  GtkWidget *rating = gtk_menu_item_new_with_mnemonic( _("Minimum _Rating") );
  gtk_container_add( GTK_CONTAINER(optionsMenu), rating );

  GtkWidget *menu = gtk_menu_new();
  gtk_menu_item_set_submenu( GTK_MENU_ITEM(rating), menu );
  gtk_widget_show(rating);

  /*
   * We don't want the final rating of "obscene" to appear
   * in this list, because it's unattainable with a 3x3 box.
   */
  const gchar *label = Board::getRatingByIndex(0);
  GSList *group = NULL;
  guint i = 0;

  while (i < ARRAY_SIZE(m_ratings))
  {
    const gchar *nextLabel = Board::getRatingByIndex(i + 1);
    if ( nextLabel == NULL )
      break;

    GtkWidget *item = gtk_radio_menu_item_new_with_mnemonic( group, label );
    group = gtk_radio_menu_item_get_group( GTK_RADIO_MENU_ITEM(item) );

    gtk_container_add( GTK_CONTAINER(menu), item );
    gtk_widget_show(item);

    m_ratings[i] = new RatingChoice(this, item, i);

    g_signal_connect( static_cast<gpointer>(item),
                      "activate",
                      G_CALLBACK(on_min_rating_activate),
                      m_ratings[i] );

    label = nextLabel;
    ++i;
  } /* for */
}


/*********************************************************************
 */
void
Window::addSeparator(GtkWidget *menu)
{
  GtkWidget *separator = gtk_separator_menu_item_new();
  gtk_widget_show(separator);
  gtk_container_add(GTK_CONTAINER(menu), separator);
  gtk_widget_set_sensitive(separator, FALSE);
}


/*********************************************************************
 */
Window::Window()
  : Widget( create_sudoku_window() )
  , m_document( new Document() )
  , m_toolBar( new ToolBar( this ) )
  , m_numberPad( new NumberPad( this, m_document->getGeometry() ) )
  , m_canvas( this )
  , m_label( this )
  , m_statusBar( this )
  , m_fileDialog( this )
  , m_geoDialog( this )
  , m_aboutBox( this )
  , m_strategyDialog( this )
  , m_solutionLog( this )
  , m_hintLog( this, lookup_widget( getGtkWidget(), "hint_view" ) )
  , m_openMenu( lookup_widget( getGtkWidget(), "open1" ) )
  , m_savePlayMenu( lookup_widget( getGtkWidget(), "save_play1" ) )
  , m_savePlayAsMenu( lookup_widget( getGtkWidget(), "save_play_as1" ) )
  , m_savePuzzleMenu( lookup_widget( getGtkWidget(), "save_puzzle1" ) )
  , m_resetMenu( gtk_menu_item_new_with_mnemonic( _("_Reset Puzzle") ) )
  , m_logMenu( lookup_widget( getGtkWidget(), "show_logs1" ) )
  , m_highlightMenu( lookup_widget( getGtkWidget(), "hilight_wrong_moves1" ) )
  , m_solveButton( lookup_widget( getGtkWidget(), "solve_button" ) )
  , m_forwardButton( lookup_widget( getGtkWidget(), "forward_button" ) )
  , m_backButton( lookup_widget( getGtkWidget(), "back_button" ) )
  , m_autoButton( lookup_widget( getGtkWidget(), "auto_button" ) )
  , m_manualButton( lookup_widget( getGtkWidget(), "manual_button" ) )
  , m_checkButton( lookup_widget( getGtkWidget(), "check_button" ) )
  , m_logBook( lookup_widget( getGtkWidget(), "log_book" ) )
  , m_toolTips( gtk_tooltips_new() )
  , m_minimumRating(0)
  , m_mode(CONTROL)
  , m_solutionIndex(0)
{
  g_signal_connect(static_cast<gpointer>(m_openMenu),
                   "activate",
                   G_CALLBACK(on_open1_activate),
                   this);

  g_signal_connect(static_cast<gpointer>(m_savePlayMenu),
                   "activate",
                   G_CALLBACK(on_save_play1_activate),
                   this);

  g_signal_connect(static_cast<gpointer>(m_savePlayAsMenu),
                   "activate",
                   G_CALLBACK(on_save_play_as1_activate),
                   this);

  g_signal_connect(static_cast<gpointer>(m_savePuzzleMenu),
                   "activate",
                   G_CALLBACK(on_save_puzzle1_activate),
                   this);

  g_signal_connect(static_cast<gpointer>(m_solveButton),
                   "clicked",
                   G_CALLBACK(on_solve_clicked),
                   this);

  g_signal_connect(static_cast<gpointer>(m_forwardButton),
                   "clicked",
                   G_CALLBACK(on_forwardbutton_clicked),
                   this);

  g_signal_connect(static_cast<gpointer>(m_backButton),
                   "clicked",
                   G_CALLBACK(on_backbutton_clicked),
                   this);

  g_signal_connect(static_cast<gpointer>(m_autoButton),
                   "clicked",
                   G_CALLBACK(on_autogenerate_clicked),
                   this);

  g_signal_connect(static_cast<gpointer>(m_manualButton),
                   "clicked",
                   G_CALLBACK(on_manualgenerate_clicked),
                   this);

  g_signal_connect(static_cast<gpointer>(m_checkButton),
                   "clicked",
                   G_CALLBACK(on_manualcheck_clicked),
                   this);

  g_signal_connect(static_cast<gpointer>(m_logMenu),
                   "activate",
                   G_CALLBACK(on_solutionlog_activate),
                   this);

  g_signal_connect(static_cast<gpointer>(m_highlightMenu),
                   "activate",
                   G_CALLBACK(on_highlight_activate),
                   this);

  GtkWidget *quit = lookup_widget( getGtkWidget(), "quit1" );
  g_signal_connect(static_cast<gpointer>(quit),
                   "activate",
                   G_CALLBACK(on_quit1_activate),
                   NULL);

  GtkWidget *about1 = lookup_widget( getGtkWidget(), "about1" );
  g_signal_connect(static_cast<gpointer>(about1),
                   "activate",
                   G_CALLBACK(on_about1_activate),
                   this);

  GtkWidget *geo = lookup_widget( getGtkWidget(), "geometry1" );
  g_signal_connect(static_cast<gpointer>(geo),
                   "activate",
                   G_CALLBACK(on_geometry_activate),
                   &m_geoDialog);

  gtk_tooltips_set_tip(m_toolTips, m_backButton, _("Undo"), NULL);
  gtk_tooltips_set_tip(m_toolTips, m_forwardButton, _("Hint"), NULL);
  gtk_tooltips_set_tip(
      m_toolTips,
      GTK_WIDGET(m_toolBar->getHighlightButton()->getKey()),
      _("Highlight cells marked with the chosen number"),
      NULL
  );

  createOptionsMenu();
  show();
}


/********************************************************************* 
 */
Window::~Window()
{
  for (unsigned i = 0; i < ARRAY_SIZE(m_ratings); ++i)
    delete m_ratings[i];
  delete m_numberPad;
  delete m_toolBar;
  delete m_document;
}


/*********************************************************************
 */
void
Window::setMode(Mode m)
{
  if ( (m_mode = m) == PLAYING )
  {
    enableHighlightMenu();
    enableToolTips();
  }
  else
  {
    disableHighlightMenu();
    disableToolTips();
  }
}


/*********************************************************************
 */
void
Window::clear()
{
  m_canvas.clear();
  m_label.clear();
  m_statusBar.clear();
  m_solutionLog.clear();
  m_hintLog.clear();
  m_geoDialog.clear();
  m_fileName.clear();
  m_solutionIndex = 0;

  disableSavePlayMenu();
  disableSavePlayAsMenu();
  disableSavePuzzleMenu();
  disableForwardArrow();
  disableBackArrow();
}


/*********************************************************************
 */
void
Window::restoreDefaults()
{
  set( Geometry() );

  m_label.restoreDefaults();
  m_statusBar.restoreDefaults();
  m_solutionLog.restoreDefaults();
  m_hintLog.restoreDefaults();
  enableOpenMenu();
}


/*********************************************************************
 */
void
Window::setDisplayText()
{
  string str;
  m_statusBar.setStatus(m_document->getStatus(str));
  m_label.setText( m_document->getRating() );
  m_solutionLog.load( m_document->getSolutionLog() );
}


/*********************************************************************
 * Clears out the board, and resets all the display widgets.
 * We DON'T want to recreate the number pad here.
 */
void
Window::initialise()
{
  setDisplayText();
  m_canvas.resize();

  enableSavePlayAsMenu();
  enableSavePuzzleMenu();
  enableResetMenu();

  if ( m_document->isComplete() )
  {
    complete();
  }
  else
  {
    setMode(PLAYING);

    enableForwardArrow();
    enableSolveButton();
    enableToolBar();
    enableNumberPad();

    if ( m_document->countMoves() != 0 )
      enableBackArrow();
  }
}


/*********************************************************************
 */
void
Window::recreateNumberPad(const Geometry &geometry)
{
  m_numberPad->destroy();
  delete m_numberPad;
  m_numberPad = new NumberPad(this, geometry);

  if (getMode() == MANUAL_GENERATE)
    enableNumberPad();
}


/*********************************************************************
 */
const Geometry&
Window::recreateBoard(const Geometry &geometry)
{
  m_document->recreate(geometry);
  m_canvas.resize();
  clear();
  return geometry;
}


/*********************************************************************
 * The board has the correct geometry already, but the rest of the
 * widgets need to be adjusted to match.
 */
void
Window::adjustFor(const Geometry &geometry)
{
  recreateNumberPad(geometry);
  configureRatingMenu(geometry);
  m_geoDialog.set(geometry);
  m_strategyDialog.set(geometry);
}


/*********************************************************************
 * Recreates entire display against the new geometry, which we expect
 * is different to our previous geometry.
 */
void
Window::set(const Geometry &geometry)
{
  adjustFor( recreateBoard(geometry) );
  disableSolveButton();
}


/*********************************************************************
 */
void
Window::configureRatingMenu(const Geometry &geometry)
{
  for (unsigned i = 0; i < ARRAY_SIZE(m_ratings); ++i)
    m_ratings[i]->configure( geometry.getMaxNumber() );

  /*
   * We don't want to get stuck trying to generate e.g. "Difficult"
   * puzzles if our geometry only allows us to generate "Easy" ones.
   */ 
  if ((m_minimumRating != 0) && m_ratings[m_minimumRating]->isActive())
    m_ratings[0]->activate();
}


/*********************************************************************
 */
void
Window::load(const string &fileName)
{
  ifstream ifile(fileName.c_str());

  if ( ifile.fail() )
  {
    m_statusBar.pushError( _("Could not read file") );
  }
  else if ( load(FileLoader(ifile)) )
  {
    setFileName(fileName);
  }
}


/*********************************************************************
 */
bool
Window::load(const DataLoader &upload)
{
  bool ok;

  try
  {
    clear();
    upload(m_document);
    adjustFor( m_document->getGeometry() );
    initialise();
    ok = true;
  }
  catch(const violation &e)
  {
    set( Geometry( m_document->getGeometry() ) );
    m_statusBar.pushError( e.what() );
    m_statusBar.pushError( _("Illegal puzzle") );
    ok = false;
  }
  catch(const exception &e)
  {
    restoreDefaults();
    m_statusBar.pushError( e.what() );
    m_statusBar.pushError( _("Invalid file format") );
    ok = false;
  }

  showCanvas();

  return ok;
}


/*********************************************************************
 */
void
Window::save(SAVING saveMode, const string &fileName)
{
  ofstream ofile(fileName.c_str());

  if ( ofile.fail() )
  {
    m_statusBar.pushError( _("Could not write to file") );
  }
  else
  {
    setFileName(fileName);

    switch (saveMode)
    {
    case SAVE_PLAY:
      m_document->savePlay(ofile);
      disableSavePlayMenu();
      break;

    case SAVE_PUZZLE:
      m_document->savePuzzle(ofile);
      break;

    default:
      break;
    } /* switch */
  }
}


/*********************************************************************
 */
void
Window::save()
{
  save(SAVE_PLAY, getFileName());
}


/*********************************************************************
 */
void
Window::generate()
{
  clear();
  m_document->generate( getMinimumRating() );
  m_canvas.show( m_document->getBoard() );

  initialise();
  m_document->generateFileName(m_fileName);
}


/*********************************************************************
 */
void
Window::show(const Board *board, const GdkColor *fg)
{
  m_canvas.clear();
  m_canvas.draw( board, fg );
  m_canvas.drawOver( m_document->getOriginalBoard() );
  m_canvas.refresh();
}


/********************************************************************
 */
inline const GdkColor*
Window::getHighlightErrorColour() 
{
  return ToolBar::getColour(6);
}


/*********************************************************************
 */
unsigned
Window::getHighlightNumber() const
{
  return m_toolBar->getHighlightButton()->getSelection();
}

unsigned
Window::getNumberPadSelection() const
{
  return m_numberPad->getSelection();
}


/*********************************************************************
 */
void
Window::showCanvas()
{
  m_canvas.draw( *m_document );

  if ( showHighlightedErrors() && m_document->hasUniqueSolution() )
  {
    m_canvas.showErrors( m_document->getBoard(),
                         m_document->getUniqueSolution(),
                         &Canvas::BLUE,
                         getHighlightErrorColour() );
  }

  m_canvas.refresh();
}


/*********************************************************************
 */
void
Window::showLogPage(gint n)
{
  setLogPage(n);
  showLogBook();
  gtk_check_menu_item_set_active( GTK_CHECK_MENU_ITEM(m_logMenu), true );
}


/*********************************************************************
 */
void
Window::setHighlightingErrors(bool active)
{
  if (m_document->hasUniqueSolution())
  {
    m_canvas.showErrors( m_document->getBoard(),
                         m_document->getUniqueSolution(),
                         &Canvas::BLUE,
                         (active ? getHighlightErrorColour() : NULL) );
  }
}


/*********************************************************************
 */
void
Window::solve()
{
  disableSolveButton();
  disableBackArrow();
  disableForwardArrow();
  disableToolBar();
  disableNumberPad();
  setMode(CONTROL);

  const BoardCollection& solutions = m_document->getSolutions();
  Document::size_type count = solutions.size();
  if ( count != 0 )
  {
    m_solutionIndex = 1;
    m_solution = solutions.begin();
    show( m_solution.operator->(), &Canvas::GREEN );

    if (count > 1)
      enableForwardArrow();
  }
}


/*********************************************************************
 */
void
Window::reset()
{
  if (m_document->getOriginalBoard() == NULL)
    return;

  const string filename = m_fileName;
  clear();
  m_fileName = filename;

  m_document->reset();
  initialise();

  showCanvas();
}


/*********************************************************************
 */
void
Window::resetSolution()
{
  if ( m_document->resetSolution() )
  {
    m_solutionLog.clear();
    setDisplayText();
  }
}


/*********************************************************************
 */
void
Window::enableSavePlayMenu()
{
  /*
   * We can't enable the "Save Play" menu option
   * unless we know that we already have a file
   * name to save under.
   */
  if ( !getFileName().empty() )
  {
    enableWidget(m_savePlayMenu);
  }
}


/*********************************************************************
 */
void
Window::enableHighlightMenu()
{
  /*
   * High-lighting only makes sense if there is a unique
   * solution to compare the user's moves against ...
   */
  gtk_check_menu_item_set_active(
      GTK_CHECK_MENU_ITEM(m_highlightMenu),
      getHighlightingErrors() && m_document->hasUniqueSolution() );

  gtk_widget_set_sensitive(m_highlightMenu, m_document->hasUniqueSolution());
}


/*********************************************************************
 */
void
Window::lastSolution()
{
  if (m_solutionIndex > 1)
  {
    if (--m_solutionIndex == 1)
      disableBackArrow();

    if (m_solutionIndex < m_document->getSolutions().size())
      enableForwardArrow();

    show( (--m_solution).operator->(), &Canvas::GREEN );
  }
}


/*********************************************************************
 */
void
Window::nextSolution()
{
  const Document::size_type count = m_document->getSolutions().size();
  if (m_solutionIndex < count)
  {
    if (++m_solutionIndex == count)
      disableForwardArrow();

    if (m_solutionIndex > 1)
      enableBackArrow();

    show( (++m_solution).operator->(), &Canvas::GREEN );
  }
}


/*********************************************************************
 */
void
Window::enableManualGenerate()
{
  disableOpenMenu();
  disableSavePlayMenu();
  disableSavePlayAsMenu();
  disableResetMenu();
  disableSolveButton();
  disableAutoButton();
  disableToolBar();
  enableCheckButton();
  enableNumberPad();
  gtk_button_set_label(GTK_BUTTON(m_manualButton), _("_Done"));
}


/*********************************************************************
 */
void
Window::disableManualGenerate()
{
  enableOpenMenu();
  enableSavePuzzleMenu();
  enableSavePlayAsMenu();
  enableResetMenu();
  enableAutoButton();
  disableCheckButton();
  gtk_button_set_label(GTK_BUTTON(m_manualButton), _("_Manual"));
}


/*********************************************************************
 */
void
Window::endManualGeneration()
{
  m_document->finalise();
  initialise();
}


/*********************************************************************
 */
void
Window::beginManualGeneration()
{
  recreateBoard( Geometry(m_document->getGeometry()) );
  m_canvas.show( m_document->getBoard() );
  setMode(MANUAL_GENERATE);
}


/*********************************************************************
 */
void
Window::toggleManualGenerate()
{
  try
  {
    if ( getMode() == MANUAL_GENERATE )
    {
      endManualGeneration();
      disableManualGenerate();
    }
    else
    {
      beginManualGeneration();
      enableManualGenerate();
    }
  }
  catch(const exception &e)
  {
    m_statusBar.setStatus( e.what() );
  }
}


/*********************************************************************
 */
void
Window::validateManualGenerate()
{
  try
  {
    BoardCollection solutions;
    bool logical = m_document->validate(m_document->getBoard(), solutions);

    ostringstream ostr;

    string str;
    ostr << Document::getStatus(str, logical, solutions.size())
         << _(",")
         << Document::getRating(solutions);

    m_statusBar.setStatus( ostr.str() );
  }
  catch(const exception &e)
  {
    m_statusBar.setStatus( e.what() );
  }
}


/*********************************************************************
 */
void
Window::enableToolBar()
{
  m_toolBar->enable();
}


/*********************************************************************
 */
void
Window::disableToolBar()
{
  m_toolBar->disable();
}


/*********************************************************************
 */
void
Window::enableNumberPad()
{
  m_numberPad->enable();
}


/*********************************************************************
 */
void
Window::disableNumberPad()
{
  m_numberPad->disable();
}


/*********************************************************************
 */
void
Window::setColouring(unsigned c)
{
  getDocument()->setColouring(static_cast<Puzzle::ColouringOption>(c));
}


/*********************************************************************
 */
void
Window::setWings(unsigned wings)
{
  getDocument()->setWings(wings);
}


/*********************************************************************
 */
void
Window::onLeftClick(Square &square)
{
  if ( m_numberPad->isEnabled() )
  {
    try
    {
      const unsigned number = square.getNumber();

      if ( getMode() == MANUAL_GENERATE )
      {
        if (number != 0)
        {
          /*
           * There is already a number in this square
           * so remove it and reverse all the flags ...
           */
          square.revertNumber();
        }
        else
        {
          /*
           * Mark this as a constant square; one that is
           * forbidden to become anything else ...
           */
          try
          {
            square = Square::FIXED( m_numberPad->getSelection() );
          }
          catch(...)
          {
            square.revertNumber();
            throw;
          }
        }

        m_canvas.show(square);
      }
      else if (number == 0)
      {
        const int colour = m_toolBar->getColourPad()->getSelection();

        if (colour != -1)
        {
          Note& note = m_document->getNoteFor(square);
          if ( note.getColour() != colour )
          {
            note.setColour(colour);

            /*
             * We'll only repaint the square if it's not highlighted,
             * because we wouldn't notice the colour-change beneath
             * the highlight colour anyway.
             */
            unsigned highlight = getHighlightNumber();
            if ( (highlight == 0 ) || !note.hasMark(highlight) )
              m_canvas.paintSquare(square, note, NORMAL);

            enableSavePlayMenu();
          }
        }
        else
        {
          m_document->addMove(square, m_numberPad->getSelection());

          const GdkColor *bg = NULL;
          if (getHighlightingErrors() && m_document->isErrorMove(square))
            bg = getHighlightErrorColour();
          m_canvas.show(square, &Canvas::BLUE, bg);

          setMode(PLAYING);
          enableSavePlayMenu();
          enableBackArrow();

          if (m_document->isComplete())
          {
            complete();
          }
        }
      }
    }
    catch (const exception &e)
    {
      m_statusBar.pushError( e.what() );
    }
  }
}


/*********************************************************************
 */
bool
Window::checkMark(const Sequence *seq, unsigned number)
{
  const bool missing = seq->getMissing().isSet(number);

  if ( !missing )
  {
    ostringstream ostr;
    ostr << _("Number ")
         << number
         << _(" already present in ")
         << seq->getName();
    m_statusBar.pushError( ostr.str() ); 
  }

  return missing;
}


/*********************************************************************
 *
bool
Window::addMark(const Square &s, unsigned nidx, unsigned n)
{
  return m_document->addMark(s, nidx, n);
}
 */


/*********************************************************************
 */
void
Window::onRightClick(Square &s, unsigned nidx)
{
  if ( (getMode() == PLAYING) &&
       m_numberPad->isEnabled() &&
       (s.getNumber() == 0) )
  {
    Note& note = m_document->getNoteFor(s);

    unsigned mark = note.getMarkAt(nidx);
    if ( mark != 0 )
    {
      note.clearMarkAt(nidx);
      if ( mark == getHighlightNumber() )
      {
        /*
         * Removing the "highlight me" mark from the square...
         */
        m_canvas.paintSquare(s, note, NORMAL);
      }
      else
      {
        /*
         * Removing an ordinary mark...
         */
        m_canvas.showMark(s, note, nidx);
      }
      enableSavePlayMenu();
    }
    else
    {
      const unsigned number = m_numberPad->getSelection();

      if ( checkMark(s.getRow(), number) &&
           checkMark(s.getColumn(), number) &&
           checkMark(s.getBox(), number) &&
           note.setMark(nidx, number) )
      {
        if ( number == getHighlightNumber() )
        {
          /*
           * Add the "highlight me" mark to the square...
           */
          m_canvas.paintSquare(s, note, HIGHLIGHT);
        }
        else
        {
          /*
           * Add an ordinary mark to the square...
           */
          m_canvas.showMark(s, note, nidx);
        }
        enableSavePlayMenu();
      }
    }
  }
}


/*********************************************************************
 */
void
Window::undoMove()
{
  Square *square = m_document->removeLatestMove();
  if (square != NULL)
  {
    m_canvas.show(*square);

    if ( m_document->countMoves() == 0 )
      disableBackArrow();

    enableForwardArrow();
    enableSavePlayMenu();
  }
}


/*********************************************************************
 */
void
Window::hintMove()
{
  const Hint *hint = m_document->getHint();
  if (hint != NULL)
  {
    /*
     * Every hint has a comment ...
     */
    m_hintLog.show( hint->message );
    showLogPage(HINT_PAGE);

    /*
     * But not every hint is accompanied by a move ...
     */
    if (hint->number != 0)
    {
      Square& square = m_document->getBoard()->find(hint->squareIndex);
      m_document->addHint(square, hint->number);

      m_canvas.show(square, &Canvas::RED);
      enableSavePlayMenu();

      if (m_document->isComplete())
      {
        complete();
      }
    }

    if ( !m_document->hasHints() )
      disableForwardArrow();
  }
}


/*********************************************************************
 */
void
Window::onBackArrow()
{
  switch( getMode() )
  {
  case PLAYING:
    undoMove();
    break;

  case CONTROL:
    lastSolution();
    break;

  default:
    break;
  } /* switch */
}


/*********************************************************************
 */
void
Window::onForwardArrow()
{
  switch( getMode() )
  {
  case PLAYING:
    hintMove();
    break;

  case CONTROL:
    nextSolution();
    break;

  default:
    break;
  } /* switch */
}


/*********************************************************************
 */
void
Window::complete()
{
  disableSolveButton();
  disableBackArrow();
  disableForwardArrow();
  disableToolBar();
  disableNumberPad();
  setMode(CONTROL);
  m_statusBar.setStatus(_("Completed"));
}

}

