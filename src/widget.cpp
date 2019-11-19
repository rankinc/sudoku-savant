#include "widget.h"

namespace sudoku
{

/*********************************************************************
 */
Widget::Widget(GtkWidget *w)
  : m_widget(w)
{}

Widget::~Widget() {}


/*********************************************************************
 */
void
Widget::show() const
{
  gtk_widget_show(m_widget);
}


/*********************************************************************
 */
void
Widget::hide() const
{
  gtk_widget_hide(m_widget);
}


/*********************************************************************
 */
void
Widget::destroy()
{
  gtk_widget_hide(m_widget);
  gtk_widget_destroy(m_widget);
}


/*********************************************************************
 */
void
Widget::restoreDefaults()
{
  clear();
}


/*********************************************************************
 */
void
Widget::clear()
{}


/*********************************************************************
 */
void
Widget::enableWidget(GtkWidget *w)
{
  if ( !isWidgetEnabled(w) )
    gtk_widget_set_sensitive(w, true);
}


/*********************************************************************
 */
void
Widget::disableWidget(GtkWidget *w)
{
  if ( isWidgetEnabled(w) )
    gtk_widget_set_sensitive(w, false);
}


/*********************************************************************
 */
gchar*
Widget::convert(const std::string &str)
{
  return g_strdup(str.c_str());
}

}

