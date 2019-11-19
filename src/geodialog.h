#ifndef GEO_DIALOG_H
#define GEO_DIALOG_H

#include "component.h"

namespace sudoku
{
  extern "C"
  {
    void on_cancelbutton1_clicked(GtkButton*, gpointer);
    void on_okbutton1_clicked(GtkButton*, gpointer);
  }

  class Window;
  class Geometry;

  class GeoDialog : public Component
  {
  private:
    GtkSpinButton* m_widthButton;
    GtkSpinButton* m_heightButton;

  public:
    explicit GeoDialog(Window*);
    ~GeoDialog();

    const GtkDialog* getGtkDialog() const {
        return GTK_DIALOG(getGtkWidget());
    }

    GtkDialog* getGtkDialog() {
        return GTK_DIALOG(getGtkWidget());
    }

    int getWidthValue() const {
        return gtk_spin_button_get_value_as_int(m_widthButton);
    }

    void setWidthValue(gint height) {
        gtk_spin_button_set_value(m_widthButton, height);
    }

    int getHeightValue() const {
        return gtk_spin_button_get_value_as_int(m_heightButton);
    }

    void setHeightValue(gint height) {
        gtk_spin_button_set_value(m_heightButton, height);
    }

    void run();

    void clear();
    void set(const Geometry&);
  };
}

#endif
