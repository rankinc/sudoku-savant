#ifndef WINDOW_H
#define WINDOW_H

#include <string>
#include <istream>
#include <gtk/gtk.h>

#include "widget.h"
#include "canvas.h"
#include "statusbar.h"
#include "geodialog.h"
#include "filedialog.h"
#include "label.h"
#include "aboutbox.h"
#include "strategydialog.h"
#include "solutionlog.h"

namespace sudoku
{
  extern "C"
  {
    typedef void (*menu_activator)(GtkMenuItem*, gpointer);

    void on_quit1_activate(GtkMenuItem*, gpointer);
    void on_open1_activate(GtkMenuItem*, gpointer);
    void on_save_play1_activate(GtkMenuItem*, gpointer);
    void on_save_play_as1_activate(GtkMenuItem*, gpointer);
    void on_save_puzzle1_activate(GtkMenuItem*, gpointer);
    void on_about1_activate(GtkMenuItem*, gpointer);
    void on_strategies_activate(GtkMenuItem*, gpointer);
    void on_geometry_activate(GtkMenuItem*, gpointer);
    void on_min_rating_activate(GtkMenuItem*, gpointer);
    void on_solutionlog_activate(GtkMenuItem*, gpointer);
    void on_highlight_activate(GtkMenuItem*, gpointer);
    void on_reset_activate(GtkMenuItem*, gpointer);
    void on_autogenerate_clicked(GtkButton*, gpointer);
    void on_manualgenerate_clicked(GtkButton*, gpointer);
    void on_manualcheck_clicked(GtkButton*, gpointer);
    void on_solve_clicked(GtkButton*, gpointer);
    void on_backbutton_clicked(GtkButton*, gpointer);
    void on_forwardbutton_clicked(GtkButton*, gpointer);

    extern const char copyright_notice[];
    extern const char translator_credit[];
  }

  class Document;
  class Geometry;
  class NumberPad;
  class ToolBar;

  class Window : public Widget
  {
  public:
    enum Mode
    {
      CONTROL,
      MANUAL_GENERATE,
      PLAYING
    };

    class DataLoader
    {
    public:
      virtual void operator()(Document*) const = 0;
      virtual ~DataLoader() {}
    };

  private:
    static const unsigned RATINGS_SIZE = 5;

    static const gint HINT_PAGE = 0;
    static const gint SOLUTION_PAGE = 1;

    struct RatingChoice
    {
      Window*    mainWindow;
      GtkWidget* menuItem;
      guint      ratingIndex;

      RatingChoice(Window *w, GtkWidget *m, guint r)
        : mainWindow(w), menuItem(m), ratingIndex(r) {}

      void enable() const { enableWidget(menuItem); }
      void disable() const { disableWidget(menuItem); }
      bool isEnabled() const { return isWidgetEnabled(menuItem); }

      bool isActive() const {
        return gtk_check_menu_item_get_active( GTK_CHECK_MENU_ITEM(menuItem) );
      }
      void activate() {
        gtk_check_menu_item_set_active( GTK_CHECK_MENU_ITEM(menuItem), true );
      }

      void configure(guint);
    };

    Document*      m_document;
    ToolBar*       m_toolBar;
    NumberPad*     m_numberPad;

    Canvas         m_canvas;
    Label          m_label;
    StatusBar      m_statusBar;
    FileDialog     m_fileDialog;
    GeoDialog      m_geoDialog;
    AboutBox       m_aboutBox;
    StrategyDialog m_strategyDialog;
    SolutionLog    m_solutionLog;
    TextLog        m_hintLog;

    GtkWidget*     m_openMenu;
    GtkWidget*     m_savePlayMenu;
    GtkWidget*     m_savePlayAsMenu;
    GtkWidget*     m_savePuzzleMenu;
    GtkWidget*     m_resetMenu;
    GtkWidget*     m_logMenu;
    GtkWidget*     m_highlightMenu;
    GtkWidget*     m_solveButton;
    GtkWidget*     m_forwardButton;
    GtkWidget*     m_backButton;
    GtkWidget*     m_autoButton;
    GtkWidget*     m_manualButton;
    GtkWidget*     m_checkButton;
    GtkWidget*     m_logBook;
    GtkTooltips*   m_toolTips;

    RatingChoice  *m_ratings[RATINGS_SIZE];
    guint          m_minimumRating;

    std::string    m_fileName;

    Mode           m_mode;

    BoardCollection::const_iterator m_solution;
    unsigned                        m_solutionIndex;

    void initialise();

    void enableOpenMenu() { enableWidget(m_openMenu); }
    void disableOpenMenu() { disableWidget(m_openMenu); }

    void enableSavePlayMenu();
    void disableSavePlayMenu() { disableWidget(m_savePlayMenu); }

    void enableSavePlayAsMenu() { enableWidget(m_savePlayAsMenu); }
    void disableSavePlayAsMenu() { disableWidget(m_savePlayAsMenu); }

    void enableSavePuzzleMenu() { enableWidget(m_savePuzzleMenu); }
    void disableSavePuzzleMenu() { disableWidget(m_savePuzzleMenu); }

    void enableResetMenu() { enableWidget(m_resetMenu); }
    void disableResetMenu() { disableWidget(m_resetMenu); }

    void enableHighlightMenu();
    void disableHighlightMenu() { disableWidget(m_highlightMenu); }

    void enableSolveButton() { enableWidget(m_solveButton); }
    void disableSolveButton() { disableWidget(m_solveButton); }

    void enableToolBar();
    void disableToolBar();

    void enableNumberPad();
    void disableNumberPad();

    void enableBackArrow() { enableWidget(m_backButton); }
    void disableBackArrow() { disableWidget(m_backButton); }

    void enableForwardArrow() { enableWidget(m_forwardButton); }
    void disableForwardArrow() { disableWidget(m_forwardButton); }

    void enableAutoButton() { enableWidget(m_autoButton); }
    void disableAutoButton() { disableWidget(m_autoButton); }

    void enableManualButton() { enableWidget(m_manualButton); }
    void disableManualButton() { disableWidget(m_manualButton); }

    void enableCheckButton() { enableWidget(m_checkButton); }
    void disableCheckButton() { disableWidget(m_checkButton); }

    void enableManualGenerate();
    void disableManualGenerate();

    void enableToolTips() { gtk_tooltips_enable(m_toolTips); }
    void disableToolTips() { gtk_tooltips_disable(m_toolTips); }

    void showLogPage(gint n);
    void setLogPage(gint n) {
        gtk_notebook_set_current_page(GTK_NOTEBOOK(m_logBook), n);
    }
    void showLogBook() { gtk_widget_show(m_logBook); }
    void hideLogBook() { gtk_widget_hide(m_logBook); }

    void showAboutBox() { m_aboutBox.show(); }
    void hideAboutBox() { m_aboutBox.hide(); }

    void showStrategyDialog() { m_strategyDialog.show(); }
    void hideStrategyDialog() { m_strategyDialog.hide(); }

    void nextSolution();
    void lastSolution();

    void undoMove();
    void hintMove();

    void beginManualGeneration();
    void endManualGeneration();

    void setDisplayText();

    void adjustFor(const Geometry&);
    void recreateNumberPad(const Geometry&);
    void configureRatingMenu(const Geometry&);

    void createOptionsMenu();
    void createRatingMenu(GtkWidget*);
    static void addSeparator(GtkWidget*);

    bool checkMark(const Sequence*, unsigned);

    void setHighlightingErrors(bool active);

    void showCanvas();
 
    void setMode(Mode);
    void setColouring(unsigned);
    void setWings(unsigned);

  public:
    Window();
    ~Window();

    const Canvas* getCanvas() const { return &m_canvas; }
    Canvas* getCanvas() { return &m_canvas; }

    const StatusBar* getStatusBar() const { return &m_statusBar; }
    StatusBar* getStatusBar() { return &m_statusBar; }

    const Label* getLabel() const { return &m_label; }
    Label* getLabel() { return &m_label; }

    const SolutionLog* getSolutionLog() const { return &m_solutionLog; }
    SolutionLog* getSolutionLog() { return &m_solutionLog; }

    const Document* getDocument() const { return m_document; }
    Document* getDocument() { return m_document; }

    const std::string& getFileName() const { return m_fileName; }
    void setFileName(const std::string &f) { m_fileName = f; }

    void showOpenDialog() { m_fileDialog.showOpen(); }
    void showSaveDialog(SAVING s) { 
        m_fileDialog.setMode(s);
        m_fileDialog.showSave();
    }

    void show(const Board*, const GdkColor* =NULL);
    void show() { return Widget::show(); }

    bool load(const DataLoader&);
    void load(const std::string&);
    void save(SAVING, const std::string&);
    void save();

    const Geometry& recreateBoard(const Geometry&);
    void clear();
    void generate();
    void solve();
    void reset();
    void resetSolution();

    void set(const Geometry&);

    void restoreDefaults();
    bool getHighlightingErrors() const {
        return gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(m_highlightMenu));
    }
    bool showHighlightedErrors() const {
        return getHighlightingErrors();
    }

    guint getMinimumRating() const { return m_minimumRating; }
    void setMinimumRating(guint i) { m_minimumRating = i; }

    unsigned getNumberPadSelection() const;
    unsigned getHighlightNumber() const;

    Mode getMode() const { return m_mode; }
    void toggleManualGenerate();
    void validateManualGenerate();
    void complete();

    void onLeftClick(Square&);
    void onRightClick(Square&, unsigned);
    void onForwardArrow();
    void onBackArrow();

    static const GdkColor *getHighlightErrorColour();

    friend void on_about1_activate(GtkMenuItem*, gpointer);
    friend void on_strategies_activate(GtkMenuItem*, gpointer);
    friend void on_min_rating_activate(GtkMenuItem*, gpointer);
    friend void on_solutionlog_activate(GtkMenuItem*, gpointer);
    friend void on_highlight_activate(GtkMenuItem*, gpointer);
    friend gboolean on_forwardarrow_pressed(GtkWidget*, GdkEventButton*, gpointer);
  };

  class FileLoader : public Window::DataLoader
  {
  private:
    std::istream* const m_istr;

  public:
    FileLoader(std::istream &istr) : m_istr(&istr) {}
    void operator()(Document*) const;
  };

  class CodeLoader : public Window::DataLoader
  {
  private:
    const std::string& m_code;

  public:
    CodeLoader(const std::string &c) : m_code(c) {}
    void operator()(Document*) const;
  };
}

namespace std
{
  inline istream&
  operator>>(istream &istr, sudoku::Window &w)
  {
    w.load(sudoku::FileLoader(istr));
    return istr;
  }
}

#endif
