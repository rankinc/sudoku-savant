#include "toolbar.h"
#include "canvas.h"
#include "window.h"

extern "C"
{
#  include "support.h"
}

namespace sudoku
{

/*********************************************************************
 */
void
on_colour_toggled(GtkToggleToolButton *button, gpointer user_data)
{
  if ( gtk_toggle_tool_button_get_active(button) )
  {
    ColourPad::Key *key = static_cast<ColourPad::Key*>(user_data);
    key->getPad()->setSelection( key->getData() );
  }
}


void
on_colour_realize(GtkWidget*, gpointer user_data)
{
  ColourPad::Key *key = static_cast<ColourPad::Key*>(user_data);

  const int colour = key->getData();
  if ( (colour == -1) ||
       gdk_colormap_alloc_color(gdk_colormap_get_system(),
                                &ColourPad::COLOUR[colour],
                                FALSE,
                                TRUE) )
  {
    key->loadIcon();
  }
}


/*********************************************************************
 */
void
on_highlight_toggled(GtkToggleToolButton *button, gpointer user_data)
{
  HighlightButton *highlight = static_cast<HighlightButton*>(user_data);

  unsigned selected;
  unsigned target;
  DRAWSTATE state;

  const gchar *format;
  if ( gtk_toggle_tool_button_get_active(button) ) {
    format = "<b>%u</b>";

    target = selected = highlight->getNumberPadSelection();
    state = HIGHLIGHT;
  } else {
    format = "";

    target = highlight->getSelection();
    selected = 0;
    state = NORMAL;
  }

  gchar *label = g_strdup_printf(format, selected);
  gtk_label_set_markup( highlight->m_label, label );
  g_free(label);

  highlight->setSelection( selected );
  highlight->showSelection( target, state );
}


void
on_highlight_realize(GtkWidget*, gpointer user_data)
{
  if ( gdk_colormap_alloc_color(gdk_colormap_get_system(),
                                &HighlightButton::COLOUR,
                                FALSE,
                                TRUE) )
  {
    HighlightButton *highlight = static_cast<HighlightButton*>(user_data);
    highlight->loadIcon();
  }
}


/*********************************************************************
 */
Button::~Button() {}


Button::Button(GtkToolItem *toolItem, guint width)
  : m_key(toolItem) 
  , m_icon(NULL)
  , m_toggleHandler(0)
  , m_realizeHandler(0)
{
  gtk_widget_set_size_request(GTK_WIDGET(m_key), width, BUTTON_SIZE);
  gtk_widget_show( GTK_WIDGET(m_key) );
}


/*********************************************************************
 *  */
void
Button::setSignals(on_toggled_handler toggled, on_realize_handler realize)
{
  m_toggleHandler = g_signal_connect(static_cast<gpointer>(m_key),
                                     "toggled",
                                     G_CALLBACK(toggled),
                                     this);
  m_realizeHandler = g_signal_connect(static_cast<gpointer>(m_key),
                                     "realize",
                                     G_CALLBACK(realize),
                                     this);
}


/*********************************************************************
 */
const guchar ColourPad::Key::colourXbm[(ICON_SIZE * ICON_SIZE) / CHAR_BIT] = {
       0xFF, 0xFF,
       0x01, 0x80,
       0x01, 0x80,
       0x01, 0x80,
       0x01, 0x80,
       0x01, 0x80,
       0x01, 0x80,
       0x01, 0x80,
       0x01, 0x80,
       0x01, 0x80,
       0x01, 0x80,
       0x01, 0x80,
       0x01, 0x80,
       0x01, 0x80,
       0x01, 0x80,
       0xFF, 0xFF
};


const guchar ColourPad::Key::numberXbm[(ICON_SIZE * ICON_SIZE) / CHAR_BIT] = {
       0xFF, 0xFF,
       0x01, 0x80,
       0x01, 0x80,
       0x01, 0x83,
       0xC1, 0x83,
       0x01, 0x83,
       0x01, 0x83,
       0x01, 0x83,
       0x01, 0x83,
       0x01, 0x83,
       0x01, 0x83,
       0xC1, 0x8F,
       0x01, 0x80,
       0x01, 0x80,
       0x01, 0x80,
       0xFF, 0xFF
};


#define _RGB_(r, g, b)  (r)*256, (g)*256, (b)*256

GdkColor ColourPad::COLOUR[MAX_COLOURS] = {
                      { 0, _RGB_(255, 255, 255) },  // White
                      { 0, _RGB_(142, 255, 135) },  // Dark green
                      { 0, _RGB_(210, 255, 108) },  // Light green
                      { 0, _RGB_(252, 252,   7) },  // Yellow
                      { 0, _RGB_(249, 227, 106) },  // Orange
                      { 0, _RGB_(246, 188, 238) },  // Pink
                      { 0, _RGB_(255, 181, 151) },  // Red
                      { 0, _RGB_(198, 218, 255) },  // Lilac
                      { 0, _RGB_(170, 255, 247) }   // Blue
};

GdkColor HighlightButton::COLOUR = { 0, _RGB_(180,  41,  59) };


/*********************************************************************
 */
ColourPad::Key::Key(ColourPad *pad,
                    GSList *group,
                    int data)
  : Button( gtk_radio_tool_button_new(group), BUTTON_SIZE )
  , m_data(data)
  , m_pad(pad)
{}


/*********************************************************************
 */
void
ColourPad::Key::loadIcon()
{
  const guchar *xbmData = colourXbm;
  const GdkColor *colour;

  if (m_data == -1)
  {
    xbmData = numberXbm;
    colour = ColourPad::getColour(0);
  }
  else
  {
    colour = ColourPad::getColour(m_data);
  }

  /*
   * We must have allocated the colours before creating the pixmaps.
   */
  GdkPixmap *pixmap = gdk_pixmap_create_from_data(
                          GTK_WIDGET(m_key)->window,
                          reinterpret_cast<const gchar*>(xbmData),
                          ICON_SIZE, ICON_SIZE,
                          -1,
                          &GTK_WIDGET(m_key)->style->black,
                          colour);

  m_icon = gtk_image_new_from_pixmap(pixmap, NULL);
  gtk_widget_show(m_icon);

  gtk_tool_button_set_icon_widget( GTK_TOOL_BUTTON(m_key), m_icon );

  if (pixmap != NULL)
    g_object_unref(pixmap);
}


/*********************************************************************
 */
GSList*
ColourPad::createKey(GtkToolbar *toolBar, int keyValue, GSList *group)
{
  m_keys.push_back( Key(this, group, keyValue) );

  Key &key = m_keys.back();
  key.setSignals(on_colour_toggled, on_colour_realize);

  GtkToolItem *gkey = key.getKey();
  group = gtk_radio_tool_button_get_group( GTK_RADIO_TOOL_BUTTON(gkey) );
  gtk_toolbar_insert(toolBar, gkey, -1);

  return group;
}


/*********************************************************************
 */
ColourPad::~ColourPad()
{
  /*
   * I don't need to deallocate the colours after
   * all - the colour array within the colour map
   * is NULL.
   *
   * deallocateColours();
   */
}


ColourPad::ColourPad(GtkToolbar *toolBar)
  : m_selection(-1)
{
  GSList *group = NULL;

  for (unsigned colourVal= 0; colourVal < ARRAY_SIZE(COLOUR); ++colourVal)
  {
    group = createKey(toolBar, colourVal, group);
  } /* for */

  ToolBar::addSeparator(toolBar);

  group = createKey(toolBar, -1, group);
}


/*********************************************************************
 */
void
ColourPad::reset()
{
  gtk_toggle_tool_button_set_active(
                        GTK_TOGGLE_TOOL_BUTTON(m_keys.back().getKey()), TRUE);
}


/*********************************************************************
 *
void
ColourPad::deallocateColours()
{
  gdk_colormap_free_colors( gdk_colormap_get_system(),
                            COLOUR,
                            ARRAY_SIZE(COLOUR) );
}
 */

/*********************************************************************
 */
const guchar HighlightButton::iconXbm[(ICON_SIZE * ICON_SIZE) / CHAR_BIT] = {
       0x00, 0x00,
       0x00, 0x00,
       0x80, 0x01,
       0x80, 0x01,
       0xB0, 0x0D,
       0x70, 0x0E,
       0x60, 0x06,
       0x9C, 0x39,
       0x9C, 0x39,
       0x60, 0x06,
       0x70, 0x0E,
       0xB0, 0x0D,
       0x80, 0x01,
       0x80, 0x01,
       0x00, 0x00,
       0x00, 0x00
};

HighlightButton::~HighlightButton() {}


HighlightButton::HighlightButton(GtkToolbar *toolBar, Window *window)
  : Button( gtk_toggle_tool_button_new(), 2 * BUTTON_SIZE )
  , m_window(window)
  , m_selection(0)
{
  GtkWidget *label = gtk_label_new(NULL);
  gtk_tool_button_set_label_widget(GTK_TOOL_BUTTON(m_key), label);
  gtk_widget_show(label);

  m_label = GTK_LABEL(label);

  gtk_tool_item_set_is_important(m_key, TRUE);

  setSignals(on_highlight_toggled, on_highlight_realize);
  gtk_toolbar_insert(toolBar, m_key, -1);
}


void
HighlightButton::loadIcon()
{
  /*
   * We must have allocated the colours before creating the pixmaps.
   */
  GdkPixmap *pixmap = gdk_pixmap_create_from_data(
                          GTK_WIDGET(m_key)->window,
                          reinterpret_cast<const gchar*>(iconXbm),
                          ICON_SIZE, ICON_SIZE,
                          -1,
                          &GTK_WIDGET(m_key)->style->white,
                          &HighlightButton::COLOUR);

  m_icon = gtk_image_new_from_pixmap(pixmap, NULL);
  gtk_widget_show(m_icon);

  gtk_tool_button_set_icon_widget( GTK_TOOL_BUTTON(m_key), m_icon );
}


void
HighlightButton::reset()
{
  gtk_toggle_tool_button_set_active(GTK_TOGGLE_TOOL_BUTTON(m_key), FALSE);
}


unsigned
HighlightButton::getNumberPadSelection() const
{
    return m_window->getNumberPadSelection();
}


void
HighlightButton::showSelection(unsigned n, DRAWSTATE state)
{
    m_window->getCanvas()->showHighlightNumber( n, state );
}


/*********************************************************************
 */
ToolBar::~ToolBar()
{
  delete m_colourPad;
  delete m_highlightButton;
}


ToolBar::ToolBar(Window *w)
  : Component( w, gtk_toolbar_new() )
{
  GtkWidget *bar = lookup_widget(w->getGtkWidget(), "toolbar_align");
  gtk_container_add(GTK_CONTAINER(bar), getGtkWidget());

  GtkToolbar *toolBar = GTK_TOOLBAR(getGtkWidget());
  gtk_toolbar_set_style(toolBar, GTK_TOOLBAR_BOTH_HORIZ);

  /* add colour pad */
  m_colourPad = new ColourPad(toolBar);

  addSeparator(toolBar);

  /* add highlight button */
  m_highlightButton = new HighlightButton(toolBar, w);

  disable();
  show();
}


void
ToolBar::addSeparator(GtkToolbar *toolBar)
{
    GtkToolItem *separator = gtk_separator_tool_item_new();
    gtk_toolbar_insert(toolBar, separator, -1);
    gtk_widget_show( GTK_WIDGET(separator) );
}


/*********************************************************************
 */
void
ToolBar::reset()
{
  m_colourPad->reset();
  m_highlightButton->reset();
}


void
ToolBar::disable()
{
  reset();
  disableWidget(getGtkWidget());
}


void
ToolBar::enable()
{ 
  reset();
  enableWidget(getGtkWidget());
}

}

