#ifndef NUMBERPAD_H
#define NUMBERPAD_H

#include <gtk/gtk.h>
#include <list>

#include "component.h"

namespace sudoku
{
  extern "C"
  {
    void on_key_toggled(GtkToggleButton*, gpointer user_data);
  }

  class Window;
  class Geometry;

  class NumberPad : public Component
  {
  private:
    class Key
    {
    private:
      GtkWidget* m_key;
      gulong     m_handler;

      unsigned   m_data;
      NumberPad* m_pad;

      void setSignal();

      static const guint SIZE = 30;

    public:
      Key(NumberPad*, GSList*, unsigned, const gchar*);

      gulong getHandler() const { return m_handler; }
      GtkWidget* getKey() const { return m_key; }

      gboolean isActive() const {
          return gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_key));
      }

      unsigned getData() const { return m_data; }
      NumberPad* getPad() const { return m_pad; }

      friend class NumberPad;
    };

    typedef std::list<Key>  KeyCollection;

    KeyCollection m_keys;
    unsigned      m_selection;

  public:
    NumberPad(Window*, const Geometry&);
    ~NumberPad();

    void disable() { disableWidget(getGtkWidget()); }
    void enable() { enableWidget(getGtkWidget()); }

    void setSelection(unsigned n) { m_selection = n; }
    unsigned getSelection() const { return m_selection; }

    friend void on_key_toggled(GtkToggleButton*, gpointer user_data);
  };

}

#endif

