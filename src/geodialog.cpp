#include "geodialog.h"
#include "window.h"
#include "document.h"
#include "geometry.h"

extern "C"
{
  #include "interface.h"
  #include "support.h"
}

namespace sudoku
{

void
on_okbutton1_clicked(GtkButton*, gpointer data)
{
  GeoDialog *geo = static_cast<GeoDialog*>(data);
  gtk_dialog_response( geo->getGtkDialog(), GTK_RESPONSE_OK );
  geo->hide();
}


void
on_cancelbutton1_clicked(GtkButton*, gpointer data)
{
  GeoDialog *geo = static_cast<GeoDialog*>(data);
  geo->hide();
}


/*********************************************************************
 */
GeoDialog::GeoDialog(Window *w)
  : Component( w, create_geometry_dialog() )
  , m_widthButton( GTK_SPIN_BUTTON( lookup_widget( getGtkWidget(),
                                                   "width_button") ) )
  , m_heightButton( GTK_SPIN_BUTTON( lookup_widget( getGtkWidget(),
                                                    "height_button" ) ) )
{
  GtkWidget *ok = lookup_widget( getGtkWidget(), "okbutton1" );
  g_signal_connect( static_cast<gpointer>(ok),
                    "clicked",
                    G_CALLBACK(on_okbutton1_clicked),
                    this );

  GtkWidget *cancel = lookup_widget( getGtkWidget(), "cancelbutton1" );
  g_signal_connect( static_cast<gpointer>(cancel),
                    "clicked",
                    G_CALLBACK(on_cancelbutton1_clicked),
                    this );
}


/*********************************************************************
 */
GeoDialog::~GeoDialog()
{
  gtk_widget_destroy( getGtkWidget() );
}


/*********************************************************************
 */
void
GeoDialog::clear()
{
  set( getMainWindow()->getDocument()->getGeometry() );
}


/*********************************************************************
 */
void
GeoDialog::set(const Geometry &geometry)
{
  setWidthValue( geometry.getBoxWidth() );
  setHeightValue( geometry.getBoxHeight() );
}


/*********************************************************************
 */
void
GeoDialog::run()
{
  for (;;)
  {
    gint result = gtk_dialog_run( getGtkDialog() );

    if (result != GTK_RESPONSE_OK)
      break;

    const Geometry::size_type width = getWidthValue();
    const Geometry::size_type height = getHeightValue();

    if ((width * height) <= Geometry::MAX_BOX_SIZE )
    {
      getMainWindow()->set( Geometry(width, height) );
      break;
    }
  } /* for */

  hide();
}

}

