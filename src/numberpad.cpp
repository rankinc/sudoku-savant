#include "numberpad.h"
#include "window.h"
#include "geometry.h"

extern "C"
{
#  include "support.h"
}

namespace sudoku
{

void
on_key_toggled(GtkToggleButton *button, gpointer user_data)
{
  if ( gtk_toggle_button_get_active(button) )
  {
    NumberPad::Key *key = static_cast<NumberPad::Key*>(user_data);
    key->getPad()->setSelection( key->getData() );
  }
}


/*********************************************************************
 */
NumberPad::Key::Key(NumberPad *pad,
                    GSList *group,
                    unsigned data,
                    const gchar *label)
  : m_key( gtk_radio_button_new_with_mnemonic(group, label) ) 
  , m_handler(0)
  , m_data(data)
  , m_pad(pad)
{
  gtk_widget_set_size_request(m_key, SIZE, SIZE);
  gtk_toggle_button_set_mode(GTK_TOGGLE_BUTTON(m_key), FALSE);
  gtk_widget_show(m_key);
}


/*********************************************************************
 */
void
NumberPad::Key::setSignal()
{
  m_handler = g_signal_connect(static_cast<gpointer>(m_key),
                               "toggled",
                               G_CALLBACK(on_key_toggled),
                               this);
}


/*********************************************************************
 */
NumberPad::~NumberPad() {}

NumberPad::NumberPad(Window *w, const Geometry &geometry)
  : Component( w, NULL )
  , m_selection(1)
{
  Geometry::size_type width = geometry.getBoxWidth();
  Geometry::size_type height = geometry.getBoxHeight();

  if (width > height)
  {
    const Geometry::size_type tmp = width;
    width = height;
    height = tmp;
  }

  setWidget( gtk_table_new(height, width, TRUE) );

  GtkWidget *pad = lookup_widget(w->getGtkWidget(), "pad_align");
  gtk_container_add(GTK_CONTAINER(pad), getGtkWidget());

  GSList *group = NULL;

  for (Geometry::size_type i = 0; i < height; ++i)
  {
    for (Geometry::size_type j = 0; j < width; ++j)
    {
      const Geometry::size_type data = ((i * width) + j + 1);

      gchar *label = g_strdup_printf("%u", data);
      m_keys.push_back( Key(this, group, data, label) );
      g_free(label);

      Key &key = m_keys.back();
      key.setSignal();

      GtkWidget *gkey = key.getKey();
      group = gtk_radio_button_get_group( GTK_RADIO_BUTTON(gkey) );
      gtk_table_attach(GTK_TABLE(getGtkWidget()),
                       gkey,
                       j, j+1, i, i+1,
                       static_cast<GtkAttachOptions>(0),
                       static_cast<GtkAttachOptions>(0),
                       0,
                       0);
    } /* for */
  } /* for */

  disable();
  show();
}

}

