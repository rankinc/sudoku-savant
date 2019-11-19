#ifndef ABOUTBOX_H
#define ABOUTBOX_H

#include "component.h"

namespace sudoku
{
  extern "C"
  {
    void on_about_button_clicked(GtkButton*, gpointer);    
    void on_aboutbox_delete(GtkWidget*, GdkEvent*, gpointer);
  }

  class Window;

  class AboutBox : public Component
  {
  public:
    AboutBox(Window*);
    ~AboutBox();
  };
}

#endif

