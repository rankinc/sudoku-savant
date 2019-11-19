#ifndef WIDGET_H
#define WIDGET_H

#include <gtk/gtk.h>
#include <string>

namespace sudoku
{
  class Widget
  {
  private:
    GtkWidget* m_widget;

  protected:
    void setWidget(GtkWidget *w) { m_widget = w; }

  public:
    explicit Widget(GtkWidget*);
    virtual ~Widget();

    GtkWidget* getGtkWidget() const { return m_widget; }

    void show() const;
    void hide() const;
    void destroy();

    bool isEnabled() const { return isWidgetEnabled(m_widget); }
    void enable() { enableWidget(m_widget); }
    void disable() { disableWidget(m_widget); }

    virtual void clear();
    void restoreDefaults();

    static void enableWidget(GtkWidget*);
    static void disableWidget(GtkWidget*);
    static bool isWidgetEnabled(GtkWidget *w) {
        return (w->state != GTK_STATE_INSENSITIVE);
    }

    static gchar* convert(const std::string&);
  };
}

#endif
