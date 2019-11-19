#include "component.h"

namespace sudoku
{

/*********************************************************************
 */
Component::Component(Window *w, GtkWidget *g)
  : Widget(g)
  , m_mainWindow(w)
{}

}

