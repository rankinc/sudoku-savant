#include "aboutbox.h"
#include "window.h"
#include "config.h"

extern "C"
{
 #include "interface.h"
}

namespace sudoku
{

void
on_about_button_clicked(GtkButton*, gpointer user_data)
{
  const AboutBox *aboutBox = static_cast<AboutBox*>(user_data);
  aboutBox->hide();
}


void
on_aboutbox_delete(GtkWidget*, GdkEvent*, gpointer user_data)
{
  const AboutBox *aboutBox = static_cast<AboutBox*>(user_data);
  aboutBox->hide();
}


/*********************************************************************
 */
AboutBox::AboutBox(Window *w)
  : Component(w, create_aboutbox())
{
  GtkLabel *title = GTK_LABEL( lookup_widget(getGtkWidget(), "title_label") );
  gtk_label_set_markup(title, "<b>Sudoku Savant " VERSION "</b>");

  GtkTextView *aboutView = GTK_TEXT_VIEW( lookup_widget( getGtkWidget(), "about_view" ) );
  gchar *gtext = g_strdup_printf("%s\n\n%s\n\n%s\n",
                                 copyright_notice,
_("A GUI version of a perfectly-good command line tool, written for the Linux Format magazine's \"Sudoku Bounty\" competition, and released under the GPL.\n"
"\n"
"With many thanks to the GTK and Glade developers, without whose efforts I would never have managed to get any kind of dratted GUI-thing working.\n"
"\n"
"Also, a big thank-you to the denizens of the \"Advanced Solving Techniques\" forum on www.sudoku.com, without whom I would never have understood half of the algorithms that I have implemented here."),
                                 translator_credit);
  gtk_text_buffer_set_text( gtk_text_view_get_buffer(aboutView), gtext, -1 );
  g_free(gtext);

  GtkWidget *aboutButton = lookup_widget( getGtkWidget(), "about_button" );
  g_signal_connect(static_cast<gpointer>(aboutButton),
                   "clicked",
                   G_CALLBACK(on_about_button_clicked),
                   this);

  g_signal_connect(getGtkWidget(),
                   "delete_event",
                   G_CALLBACK(on_aboutbox_delete),
                   this);
}


/*********************************************************************
 */
AboutBox::~AboutBox()
{
  gtk_widget_destroy( getGtkWidget() );
}

}

