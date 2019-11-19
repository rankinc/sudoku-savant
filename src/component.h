#ifndef COMPONENT_H
#define COMPONENT_H

#include "widget.h"

namespace sudoku
{
  class Window;

  class Component : public Widget
  {
  private:
    Window* m_mainWindow;

  public:
    Component(Window*, GtkWidget*);

    Window* getMainWindow() const { return m_mainWindow; }
  };
}

#endif

