#include "textlog.h"
#include "window.h"

extern "C"
{
  #include "support.h"
}

namespace sudoku
{

/*********************************************************************
 */
TextLog::TextLog(Window *w, GtkWidget *gw)
  : Component(w, gw)
  , m_buffer( gtk_text_buffer_new(NULL) )
{
  gtk_text_view_set_buffer( getGtkTextView(), m_buffer );
  gtk_text_buffer_get_end_iter( m_buffer, &m_end );
}


/*********************************************************************
 */
TextLog::~TextLog()
{
  g_object_unref(m_buffer);
}


/*********************************************************************
 */
void
TextLog::clear()
{
  GtkTextIter start;
  gtk_text_buffer_get_start_iter( m_buffer, &start );
  gtk_text_buffer_delete( m_buffer, &start, &m_end );
}


/*********************************************************************
 */
void
TextLog::writeLine(const std::string &message)
{
  gchar *text = convert( message );
  gtk_text_buffer_insert( m_buffer, &m_end, text, -1 );
  gtk_text_buffer_insert( m_buffer, &m_end, "\n", 1);
  g_free(text);
}


/*********************************************************************
 */
void
TextLog::eraseLine(gint lineIndex)
{
  GtkTextIter lineStart;
  gtk_text_buffer_get_iter_at_line(m_buffer, &lineStart, lineIndex);
  gtk_text_buffer_delete(m_buffer, &lineStart, &m_end);
}


/*********************************************************************
 */
void
TextLog::show(const std::string &message)
{
  writeLine(message);

  GtkTextMark *mark = gtk_text_buffer_create_mark(
                                      m_buffer, NULL, &m_end, FALSE);
  gtk_text_view_scroll_to_mark(getGtkTextView(), mark, 0.1, FALSE, 0.0, 0.0);
  gtk_text_buffer_delete_mark( m_buffer, mark );
}

}

