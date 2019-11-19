#include "strategydialog.h"
#include "window.h"
#include "document.h"
#include "generator.h"

extern "C"
{
 #include "interface.h"
}


namespace sudoku
{

void
on_numbercontrol_toggled(GtkToggleButton *button, gpointer user_data)
{
  StrategyDialog::NumberControl *control
    = static_cast<StrategyDialog::NumberControl*>(user_data);
  control->setSensitive( button->active );
}


void
on_numbercontrol_defaulted(GtkToggleButton *button, gpointer user_data)
{
  StrategyDialog::NumberControl *control
    = static_cast<StrategyDialog::NumberControl*>(user_data);
  control->setSpinSensitive( !button->active );
}


void
on_numbercontrol_value_changed(GtkSpinButton*, gpointer user_data)
{
  StrategyDialog::NumberControl *control
    = static_cast<StrategyDialog::NumberControl*>(user_data);
  control->getStrategyDialog()->enableApplyButton();
}


void
on_colouring_changed(GtkComboBox *combo, gpointer user_data)
{
  StrategyDialog *strategies = static_cast<StrategyDialog*>(user_data);
  strategies->setColouring(
    static_cast<StrategyDialog::ColouringOption>(gtk_combo_box_get_active(combo)) );
}


void
on_xywings_toggled(GtkToggleButton *button, gpointer user_data)
{
  StrategyDialog *strategies = static_cast<StrategyDialog*>(user_data);
  strategies->setXYWings( button->active );
}


void
on_xyzwings_toggled(GtkToggleButton *button, gpointer user_data)
{
  StrategyDialog *strategies = static_cast<StrategyDialog*>(user_data);
  strategies->setXYZWings( button->active );
}


void
on_strategy_reveal(GtkWidget*, gpointer user_data)
{
  StrategyDialog *strategies = static_cast<StrategyDialog*>(user_data);
  strategies->disableApplyButton();
  strategies->loadState();
}


void
on_strategy_delete(GtkWidget*, GdkEvent*, gpointer user_data)
{
  const StrategyDialog *strategies = static_cast<StrategyDialog*>(user_data);
  strategies->hide();
}


void
on_strategy_ok_clicked(GtkButton*, gpointer user_data)
{
  StrategyDialog *strategies = static_cast<StrategyDialog*>(user_data);
  if (strategies->areModified())
    strategies->applyState();
  strategies->hide();
}


void
on_strategy_apply_clicked(GtkButton*, gpointer user_data)
{
  StrategyDialog *strategies = static_cast<StrategyDialog*>(user_data);
  strategies->disableApplyButton();
  strategies->applyState();
}


void
on_strategy_cancel_clicked(GtkButton*, gpointer user_data)
{
  const StrategyDialog *strategies = static_cast<StrategyDialog*>(user_data);
  strategies->hide();
}


/********************************************************************
 */
StrategyDialog::NumberControl::NumberControl(StrategyDialog *dialog,
                                             GtkWidget *box,
                                             const gchar *label)
  : m_dialog(dialog)
{
  GtkWidget *frame = gtk_frame_new(NULL);
  gtk_widget_show(frame);
  gtk_box_pack_start(GTK_BOX(box), frame, TRUE, TRUE, 0);

  GtkWidget *alignment = gtk_alignment_new(0.5, 0.5, 1, 1);
  gtk_widget_show(alignment);

  gtk_container_add(GTK_CONTAINER(frame), alignment);
  gtk_alignment_set_padding(GTK_ALIGNMENT(alignment), 0, 0, 4, 0);

  GtkWidget *masterBox = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(masterBox);
  gtk_container_add(GTK_CONTAINER(alignment), masterBox);

  m_checkButton = gtk_check_button_new_with_mnemonic(label);
  gtk_widget_show(m_checkButton);
  gtk_box_pack_start(GTK_BOX(masterBox), m_checkButton, TRUE, TRUE, 0);
  g_signal_connect(static_cast<gpointer>(m_checkButton),
                   "toggled",
                   G_CALLBACK(on_numbercontrol_toggled),
                   this);

  m_controlBox = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(m_controlBox);
  gtk_box_pack_start(GTK_BOX(masterBox), m_controlBox, FALSE, TRUE, 4);

  m_defaultButton = gtk_check_button_new_with_mnemonic(_("Default"));
  gtk_widget_show(m_defaultButton);
  gtk_box_pack_start(GTK_BOX(m_controlBox), m_defaultButton, FALSE, FALSE, 0);
  // gtk_button_set_relief(GTK_BUTTON(m_defaultButton), GTK_RELIEF_NONE);
  gtk_toggle_button_set_mode(GTK_TOGGLE_BUTTON(m_defaultButton), FALSE);
  g_signal_connect(static_cast<gpointer>(m_defaultButton),
                   "toggled",
                   G_CALLBACK(on_numbercontrol_defaulted),
                   this);

  GtkObject *spinAdjustment
               = gtk_adjustment_new(MIN_LIMIT, MIN_LIMIT, 10, 1, 1, 0);
  m_spinButton = gtk_spin_button_new(GTK_ADJUSTMENT(spinAdjustment), 1, 0);
  gtk_widget_show(m_spinButton);
  gtk_box_pack_start(GTK_BOX(m_controlBox), m_spinButton, FALSE, FALSE, 0);

  GtkTooltips *tooltips = gtk_tooltips_new();

  gtk_tooltips_set_tip(tooltips, m_spinButton, _("Up to this size"), NULL);
  gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(m_spinButton), TRUE);
  m_spinHandler = g_signal_connect(static_cast<gpointer>(m_spinButton),
                                   "value-changed",
                                   G_CALLBACK(on_numbercontrol_value_changed),
                                   this);
}


/********************************************************************
 */
void
StrategyDialog::NumberControl::set(unsigned value, unsigned limit)
{
  gtk_signal_handler_block(static_cast<gpointer>(m_spinButton),
                           m_spinHandler);

  gtk_spin_button_set_range(GTK_SPIN_BUTTON(m_spinButton), MIN_LIMIT, limit);

  const bool isActive = (value != 0);
  const bool isDefault = (value == Generator::DEFAULT_SIZE);

  gtk_widget_set_sensitive(m_controlBox, isActive);
  gtk_widget_set_sensitive(m_spinButton, !isDefault);
  toggleButton(m_defaultButton, isDefault);
  toggleButton(m_checkButton, isActive);

  const unsigned svalue = ((!isActive || isDefault) ? limit : value);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(m_spinButton), svalue);

  gtk_signal_handler_unblock(static_cast<gpointer>(m_spinButton),
                             m_spinHandler);
}


/********************************************************************
 */
unsigned
StrategyDialog::NumberControl::getValue() const
{
  unsigned value = 0;

  if (Widget::isWidgetEnabled(m_controlBox))
  {
    value = (Widget::isWidgetEnabled(m_spinButton) ?
                                getRawValue() : Generator::DEFAULT_SIZE);
  }

  return value;
}


/********************************************************************
 */
void
StrategyDialog::NumberControl::setSensitive(bool s)
{
  if (Widget::isWidgetEnabled(m_controlBox) ^ s)
  {
    gtk_widget_set_sensitive(m_controlBox, s);
    getStrategyDialog()->enableApplyButton();
  }
}


/********************************************************************
 */
void
StrategyDialog::NumberControl::setSpinSensitive(bool s)
{
  if (Widget::isWidgetEnabled(m_spinButton) ^ s)
  {
    gtk_widget_set_sensitive(m_spinButton, s);
    getStrategyDialog()->enableApplyButton();
  }
}


/********************************************************************
 */
void
StrategyDialog::NumberControl::updateMaxLimit(unsigned limit)
{
  set(getValue(), limit);
}


/********************************************************************
 */
StrategyDialog::StrategyDialog(Window *w)
  : Component(w, gtk_window_new(GTK_WINDOW_TOPLEVEL))
  , m_strategyBox( gtk_vbox_new(FALSE, 0)  )
  , m_hiddenSubsets( this, m_strategyBox, _("_Hidden Subsets") )
  , m_nakedSubsets( this, m_strategyBox, _("_Naked Subsets") )
  , m_patterns( this, m_strategyBox, _("NxN _Fish") )
{
  GtkWidget *window = getGtkWidget();

  gtk_window_set_title(GTK_WINDOW(window), _("Sudoku Strategies"));
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_resizable(GTK_WINDOW(window), FALSE);

  gtk_widget_show(m_strategyBox);
  gtk_container_add(GTK_CONTAINER(window), m_strategyBox);

  g_signal_connect(window,
                   "show",
                   G_CALLBACK(on_strategy_reveal),
                   this);

  g_signal_connect(window,
                   "delete_event",
                   G_CALLBACK(on_strategy_delete),
                   this);

  GtkWidget *frame = gtk_frame_new(NULL);
  gtk_widget_show(frame);
  gtk_box_pack_start(GTK_BOX(m_strategyBox), frame, TRUE, TRUE, 0);

  GtkWidget *alignment = gtk_alignment_new(0.5, 0.5, 1, 1);
  gtk_widget_show(alignment);
  gtk_container_add(GTK_CONTAINER(frame), alignment);
  gtk_alignment_set_padding(GTK_ALIGNMENT(alignment), 0, 0, 4, 0);

  GtkWidget *buttonBox = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(buttonBox);
  gtk_container_add(GTK_CONTAINER(alignment), buttonBox);

  m_colouring = addStrategyCombo(buttonBox, on_colouring_changed);
  gtk_combo_box_append_text(GTK_COMBO_BOX(m_colouring), _("No Colouring"));
  gtk_combo_box_append_text(GTK_COMBO_BOX(m_colouring), _("Simple Colouring"));
  gtk_combo_box_append_text(GTK_COMBO_BOX(m_colouring), _("Multi-Colouring"));

  m_xywings = addStrategyButton(buttonBox,
                                _("_XY-Wings"),
                                on_xywings_toggled);
  m_xyzwings = addStrategyButton(buttonBox,
                                 _("XY_Z-Wings"),
                                 on_xyzwings_toggled);

  addDialogButtons(m_strategyBox);
}


/********************************************************************
 */
StrategyDialog::~StrategyDialog()
{
  gtk_widget_destroy( getGtkWidget() );
}


/********************************************************************
 */
GtkWidget*
StrategyDialog::addStrategyButton(GtkWidget *box,
                                  const gchar *label,
                                  toggled_action action)
{
  GtkWidget *button = gtk_check_button_new_with_mnemonic(label);
  gtk_widget_show(button);
  gtk_box_pack_start(GTK_BOX(box), button, FALSE, FALSE, 0);
  g_signal_connect(static_cast<gpointer>(button),
                   "toggled",
                   G_CALLBACK(action),
                   this);
  return button;
}


/********************************************************************
 */
GtkWidget*
StrategyDialog::addStrategyCombo(GtkWidget *box, combo_action action)
{
  GtkWidget *combo = gtk_combo_box_new_text(); 
  gtk_widget_show(combo);
  gtk_box_pack_start(GTK_BOX(box), combo, FALSE, FALSE, 0);
  g_signal_connect(static_cast<gpointer>(combo),
                   "changed",
                   G_CALLBACK(action),
                   this);

  return combo;
}


/********************************************************************
 */
void
StrategyDialog::addDialogButtons(GtkWidget *box)
{
  GtkWidget *buttonBox = gtk_hbutton_box_new();
  gtk_widget_show(buttonBox);
  gtk_box_pack_start(GTK_BOX(box), buttonBox, TRUE, TRUE, 0);

  m_applyButton = gtk_button_new_from_stock("gtk-apply");
  gtk_widget_show(m_applyButton);
  gtk_container_add(GTK_CONTAINER(buttonBox), m_applyButton);
  GTK_WIDGET_SET_FLAGS(m_applyButton, GTK_CAN_DEFAULT);

  g_signal_connect(static_cast<gpointer>(m_applyButton),
                   "clicked",
                   G_CALLBACK(on_strategy_apply_clicked),
                   this);

  GtkWidget *cancelButton = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelButton);
  gtk_container_add(GTK_CONTAINER(buttonBox), cancelButton);
  GTK_WIDGET_SET_FLAGS(cancelButton, GTK_CAN_DEFAULT);

  g_signal_connect(static_cast<gpointer>(cancelButton),
                   "clicked",
                   G_CALLBACK(on_strategy_cancel_clicked),
                   this);

  GtkWidget *okButton = gtk_button_new_from_stock("gtk-ok");
  gtk_widget_show(okButton);
  gtk_container_add(GTK_CONTAINER(buttonBox), okButton);
  GTK_WIDGET_SET_FLAGS(okButton, GTK_CAN_DEFAULT);

  g_signal_connect(static_cast<gpointer>(okButton),
                   "clicked",
                   G_CALLBACK(on_strategy_ok_clicked),
                   this);
}


/********************************************************************
 */
void
StrategyDialog::loadState()
{
  const Document *document = getMainWindow()->getDocument();

  m_hiddenSubsets.set( document->getMaxHiddenSubsetSize(),
                       document->getSubsetSizeLimit() );
  m_nakedSubsets.set( document->getMaxNakedSubsetSize(),
                      document->getSubsetSizeLimit() );
  m_patterns.set( document->getMaxPatternSize(),
                  document->getPatternSizeLimit() );

  m_state.colouring = document->getColouring();
  gtk_combo_box_set_active(GTK_COMBO_BOX(m_colouring), m_state.colouring);

  m_state.wings = document->getWings();
  toggleButton(m_xywings, ((m_state.wings & Puzzle::USE_XY_WINGS) != 0));
  toggleButton(m_xyzwings, ((m_state.wings & Puzzle::USE_XYZ_WINGS) != 0));
}


/********************************************************************
 */
void
StrategyDialog::applyState()
{
  Window *window = getMainWindow();

  Document *document = window->getDocument();
  document->setMaxHiddenSubsetSize( m_hiddenSubsets.getValue() );
  document->setMaxNakedSubsetSize( m_nakedSubsets.getValue() );
  document->setMaxPatternSize( m_patterns.getValue() );
  document->setColouring( getColouring() );
  document->setWings( getWings() );

  window->resetSolution();
}


/********************************************************************
 */
void
StrategyDialog::setColouring(ColouringOption colouring)
{
  if (colouring != m_state.colouring)
  {
    m_state.colouring = colouring;
    enableApplyButton();
  }
}
 

/********************************************************************
 */
void
StrategyDialog::setWings(bool active, unsigned wing)
{
  if (active ^ ((m_state.wings & wing) != 0))
  {
    m_state.wings ^= wing;
    enableApplyButton();
  }
}


/********************************************************************
 */
void
StrategyDialog::set(const Geometry &geometry)
{
  m_hiddenSubsets.updateMaxLimit( geometry.getSubsetSizeLimit() );
  m_nakedSubsets.updateMaxLimit( geometry.getSubsetSizeLimit() );
  m_patterns.updateMaxLimit( geometry.getPatternSizeLimit() );
}

}

