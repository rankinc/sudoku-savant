#include "label.h"
#include "window.h"

extern "C"
{
  #include "support.h"
}

namespace sudoku
{

/*********************************************************************
 */
Label::Label(Window *w)
  : Component(w, lookup_widget( w->getGtkWidget(), "label1" ))
{}


/*********************************************************************
 */
void
Label::setText(const gchar *text)
{
  gchar *gtext = g_strdup_printf("<b>%s</b>", gettext(text));
  gtk_label_set_markup_with_mnemonic( getGtkLabel(), gtext );
  g_free(gtext);
}


/*********************************************************************
 */
void
Label::clear()
{
  setText(_("Unknown"));
}

}

