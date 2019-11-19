#ifndef STATUSBAR_H
#define STATUSBAR_H

#include <string>
#include "component.h"

namespace sudoku
{
  extern "C"
  {
    gboolean on_statusbar_button_press_event(GtkWidget*,
                                             GdkEventButton*,
                                             gpointer);
  }

  class StatusBar : public Component
  {
  private:
    guint m_statusContext;

  public:
    explicit StatusBar(Window*);

    const GtkStatusbar* getGtkStatusBar() const {
        return GTK_STATUSBAR( getGtkWidget() );
    }

    GtkStatusbar* getGtkStatusBar() {
        return GTK_STATUSBAR( getGtkWidget() );
    }

    guint getStatusContext() const { return m_statusContext; }

    guint push(guint contextId, const std::string &msg);
    void pop(guint contextId);
    void remove(guint contextId, guint message);

    guint pushError(const std::string &msg) {
        return push(0, msg);
    }
    void popError() { return pop(0); }

    guint pushStatus(const std::string &msg) {
        return push(getStatusContext(), msg);
    }
    void popStatus() { pop(getStatusContext()); }

    void setStatus(const std::string &msg);
    void clear();
  };
}

#endif

