#include "statusbar.h"
#include "window.h"
using namespace std;

extern "C"
{
  #include "support.h"
}

namespace sudoku
{

gboolean
on_statusbar_button_press_event(GtkWidget*,
                                GdkEventButton *event,
                                gpointer        user_data)
{
  StatusBar *statusBar = static_cast<StatusBar*>(user_data);

  if (event->type == GDK_BUTTON_PRESS)
    statusBar->popError();

  return FALSE;
}


/*********************************************************************
 */
StatusBar::StatusBar(Window *w)
  : Component(w, lookup_widget(w->getGtkWidget(), "statusbar1"))
  , m_statusContext(gtk_statusbar_get_context_id(getGtkStatusBar(), "Status"))
{
  g_signal_connect(static_cast<gpointer>( lookup_widget(w->getGtkWidget(),
                                          "eventbox1") ),
                   "button_press_event",
                   G_CALLBACK(on_statusbar_button_press_event),
                   this);
}


/*********************************************************************
 */
guint
StatusBar::push(guint contextId, const string &msg)
{
  gchar *message = convert(msg);
  guint ret = gtk_statusbar_push( getGtkStatusBar(), contextId, message );
  g_free(message);
  return ret;
}


/*********************************************************************
 */
void
StatusBar::pop(guint contextId)
{
  gtk_statusbar_pop( getGtkStatusBar(), contextId );
}


/*********************************************************************
 */
void
StatusBar::remove(guint contextId, guint messageId)
{
  gtk_statusbar_remove( getGtkStatusBar(), contextId, messageId );
}


/*********************************************************************
 */
void
StatusBar::clear()
{
  popStatus();
}


/*********************************************************************
 */
void
StatusBar::setStatus(const string &msg)
{
  popStatus();
  pushStatus(msg);
}

}

