#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <gtk/gtk.h>
#include <list>

#include "constants.h"
#include "component.h"
#include "enums.h"

namespace sudoku
{
  extern "C"
  {
    typedef void(*on_toggled_handler)(GtkToggleToolButton*, gpointer);
    typedef void(*on_realize_handler)(GtkWidget*, gpointer);

    void on_colour_toggled(GtkToggleToolButton*, gpointer user_data);
    void on_colour_realize(GtkWidget*, gpointer user_data);

    void on_highligh_toggled(GtkToggleToolButton*, gpointer user_data);
    void on_highlight_realize(GtkWidget*, gpointer user_data);
  }

  class Window;

  class Button
  {
  protected:
    GtkToolItem* m_key;
    GtkWidget*   m_icon;
    gulong       m_toggleHandler;
    gulong       m_realizeHandler;

    Button(GtkToolItem*, guint);
    virtual ~Button();

    void setSignals(on_toggled_handler, on_realize_handler);

    static const guint BUTTON_SIZE = 30;
    static const guint ICON_SIZE = 16;

  public:
    GtkToolItem* getKey() const { return m_key; }
    virtual void loadIcon() = 0;
  };

  class ColourPad
  {
  private:
    class Key : public Button
    {
    private:
      int        m_data;
      ColourPad* m_pad;

      //static void deallocateColours();

      static const guchar colourXbm[];
      static const guchar numberXbm[];

    public:
      Key(ColourPad*, GSList*, int);

      void loadIcon();

      int getData() const { return m_data; }
      ColourPad* getPad() const { return m_pad; }

      friend class ColourPad;
    };

    typedef std::list<Key>  KeyCollection;

    KeyCollection m_keys;
    int           m_selection;

    GSList* createKey(GtkToolbar*, int keyValue, GSList*);

    static GdkColor COLOUR[MAX_COLOURS];

  public:
    ColourPad(GtkToolbar*);
    ~ColourPad();

    void reset();

    void setSelection(int n) { m_selection = n; }
    int getSelection() const { return m_selection; }

    static const GdkColor* getColour(unsigned i) { return &ColourPad::COLOUR[i]; }

    friend void on_colour_toggled(GtkToggleToolButton*, gpointer user_data);
    friend void on_colour_realize(GtkWidget*, gpointer user_data);
  };


  class HighlightButton : public Button
  {
  private:
    Window *m_window;
    GtkLabel *m_label;
    unsigned m_selection;

    unsigned getNumberPadSelection() const;

    static const guchar iconXbm[];
    static GdkColor COLOUR;

  public:
    HighlightButton(GtkToolbar*, Window*);
    ~HighlightButton();

    void reset();

    void setSelection(unsigned n) { m_selection = n; }
    unsigned getSelection() const {  return m_selection; }
    void showSelection(unsigned, DRAWSTATE);

    void loadIcon();

    static const GdkColor* getColour() { return &HighlightButton::COLOUR; }

    friend void on_highlight_toggled(GtkToggleToolButton*, gpointer user_data);
    friend void on_highlight_realize(GtkWidget*, gpointer user_data);
  };


  class ToolBar : public Component
  {
  private:
    ColourPad       *m_colourPad;
    HighlightButton *m_highlightButton;

    void reset();

  public:
    ToolBar(Window*);
    ~ToolBar();

    const ColourPad* getColourPad() const { return m_colourPad; }
    ColourPad* getColourPad() { return m_colourPad; }

    const HighlightButton* getHighlightButton() const { return m_highlightButton; }
    HighlightButton* getHighlightButton() { return m_highlightButton; }

    void enable();
    void disable();

    static void addSeparator(GtkToolbar*);

    static const GdkColor* getColour(unsigned i) { return ColourPad::getColour(i); }
    static const GdkColor* getHighlightColour() { return HighlightButton::getColour(); }
  };

}

#endif

