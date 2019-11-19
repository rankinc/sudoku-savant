#include <string>
using namespace std;

#include "filedialog.h"
#include "window.h"
#include "document.h"

extern "C"
{
  #include "interface.h"
  #include "support.h"
}

namespace sudoku
{

void
on_file_open_clicked(GtkButton*, gpointer user_data)
{
  FileDialog *dialog = static_cast<FileDialog*>(user_data);
  string fileName;

  dialog->getMainWindow()->load( dialog->getFileName(fileName) );
  dialog->hide();
}


void
on_file_save_clicked(GtkButton*, gpointer user_data)
{
  const FileDialog *dialog = static_cast<FileDialog*>(user_data);
  string fileName;

  dialog->getMainWindow()->save( dialog->getMode(),
                                 dialog->getFileName(fileName) );
  dialog->hide();
}


void
on_file_cancel_clicked(GtkButton*, gpointer user_data)
{
  const FileDialog *dialog = static_cast<FileDialog*>(user_data);
  dialog->hide();
}


void
on_filedialog_delete_event(GtkWidget*, GdkEvent*, gpointer user_data)
{
  const FileDialog *dialog = static_cast<FileDialog*>(user_data);
  dialog->hide();
}


/*********************************************************************
 */
FileDialog::FileDialog(Window *w)
  : Component( w, create_filechooserdialog1() )
  , m_saveButton( lookup_widget( getGtkWidget(), "file_save" ) )
  , m_openButton( lookup_widget( getGtkWidget(), "file_open" ) ) 
  , m_mode(SAVE_PUZZLE)
{
  GtkWidget *cancel = lookup_widget( getGtkWidget(), "file_cancel" );

  g_signal_connect(static_cast<gpointer>(cancel),
                   "clicked",
                   G_CALLBACK(on_file_cancel_clicked),
                   this);

  g_signal_connect(static_cast<gpointer>(m_openButton),
                   "clicked",
                   G_CALLBACK(on_file_open_clicked),
                   this);

  g_signal_connect(static_cast<gpointer>(m_saveButton),
                   "clicked",
                   G_CALLBACK(on_file_save_clicked),
                   this);

  g_signal_connect(getGtkWidget(),
                   "delete_event",
                   G_CALLBACK(on_filedialog_delete_event),
                   this);
}


/*********************************************************************
 */
FileDialog::~FileDialog()
{
  gtk_widget_destroy( getGtkWidget() );
}


/*********************************************************************
 */
string&
FileDialog::getFileName(string &str) const
{
  gchar *s = gtk_file_chooser_get_filename(
                       const_cast<GtkFileChooser*>(getGtkFileChooser()) );
  str = (s != NULL ? s : "");
  g_free(s);

  return str;
}


/*********************************************************************
 */
void
FileDialog::showOpen()
{
  gtk_file_chooser_set_action( getGtkFileChooser(),
                               GTK_FILE_CHOOSER_ACTION_OPEN );
  gtk_widget_grab_default(m_openButton);
  gtk_widget_hide(m_saveButton);
  gtk_widget_show(m_openButton);
  show();
}


/*********************************************************************
 */
void
FileDialog::showSave()
{
  gtk_file_chooser_set_action( getGtkFileChooser(),
                               GTK_FILE_CHOOSER_ACTION_SAVE );

  gchar *fileName = convert( getMainWindow()->getFileName() );
  gtk_file_chooser_set_current_name( getGtkFileChooser(), fileName );
  g_free(fileName);

  gtk_widget_hide(m_openButton);
  gtk_widget_show(m_saveButton);
  show();
}

}

