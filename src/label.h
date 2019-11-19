#ifndef LABEL_H
#define LABEL_H

#include "component.h"

namespace sudoku
{
  class Window;

  class Label : public Component
  {
  public:
    Label(Window*);

    const GtkLabel* getGtkLabel() const { return GTK_LABEL( getGtkWidget() ); }
    GtkLabel* getGtkLabel() { return GTK_LABEL( getGtkWidget() ); }

    void setText(const gchar*);
    void clear();
  };
}

#endif
