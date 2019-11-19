#ifndef STRATEGY_DIALOG_H
#define STRATEGY_DIALOG_H

#include "component.h"
#include "puzzle.h"

namespace sudoku
{

  extern "C"
  {
    typedef void (*toggled_action)(GtkToggleButton*, gpointer);
    typedef void (*combo_action)(GtkComboBox*, gpointer);

    void on_numbercontrol_toggled(GtkToggleButton*, gpointer);
    void on_numbercontrol_defaulted(GtkToggleButton*, gpointer);
    void on_numbercontrol_value_changed(GtkSpinButton*, gpointer);
    void on_colouring_changed(GtkComboBox*, gpointer);
    void on_xywings_toggled(GtkToggleButton*, gpointer);
    void on_xyzwings_toggled(GtkToggleButton*, gpointer);

    void on_strategy_reveal(GtkWidget*, gpointer);
    void on_strategy_ok_clicked(GtkButton*, gpointer);
    void on_strategy_apply_clicked(GtkButton*, gpointer);
    void on_strategy_cancel_clicked(GtkButton*, gpointer);
    void on_strategy_delete(GtkWidget*, GdkEvent*, gpointer);
  }

  class Window;
  class Geometry;

  class StrategyDialog : public Component
  {
  private:
    typedef Puzzle::ColouringOption  ColouringOption;

    struct State
    {
      ColouringOption colouring;
      unsigned wings;

      State() : colouring(Puzzle::NO_COLOURING) , wings(0) {}
    };

    class NumberControl
    {
    private:
      static const unsigned MIN_LIMIT = 2;

      GtkWidget      *m_checkButton;
      GtkWidget      *m_controlBox;
      GtkWidget      *m_defaultButton;
      GtkWidget      *m_spinButton;
      gulong          m_spinHandler;
      StrategyDialog *m_dialog;

    public:
      NumberControl(StrategyDialog*, GtkWidget*, const gchar*);

      StrategyDialog* getStrategyDialog() const { return m_dialog; }

      unsigned getRawValue() const {
          return gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(m_spinButton));
      }

      void updateMaxLimit(unsigned);
      void set(unsigned, unsigned);
      unsigned getValue() const;
      void setSensitive(bool);
      void setSpinSensitive(bool);
    };

    GtkWidget *m_strategyBox;
    GtkWidget *m_applyButton;

    NumberControl m_hiddenSubsets;
    NumberControl m_nakedSubsets;
    NumberControl m_patterns;
    GtkWidget    *m_colouring;
    GtkWidget    *m_xywings;
    GtkWidget    *m_xyzwings;

    State         m_state;

    GtkWidget* addStrategyButton(GtkWidget*, const gchar*, toggled_action);
    GtkWidget* addStrategyCombo(GtkWidget*, combo_action);
    void addDialogButtons(GtkWidget*);

    void disableApplyButton() { disableWidget(m_applyButton); }
    void enableApplyButton() { enableWidget(m_applyButton); }

  public:
    StrategyDialog(Window*);
    ~StrategyDialog();

    void set(const Geometry&);

    ColouringOption getColouring() const { return m_state.colouring; }
    bool isColouring() const {
        return (getColouring() != Puzzle::NO_COLOURING);
    }
    unsigned getWings() const { return m_state.wings; }

    void setWings(bool, unsigned);

    void setColouring(ColouringOption);
    void setXYWings(bool w) { setWings(w, Puzzle::USE_XY_WINGS); }
    void setXYZWings(bool w) { setWings(w, Puzzle::USE_XYZ_WINGS); }

    void loadState();
    void applyState();
    bool areModified() const { return isWidgetEnabled(m_applyButton); }

    static void toggleButton(GtkWidget *w, bool state) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w), state);
    }

    friend void on_numbercontrol_toggled(GtkToggleButton*, gpointer);
    friend void on_numbercontrol_defaulted(GtkToggleButton*, gpointer);
    friend void on_numbercontrol_value_changed(GtkSpinButton*, gpointer);
    friend void on_colouring_changed(GtkComboBox*, gpointer);
    friend void on_strategy_apply_clicked(GtkButton*, gpointer);
    friend void on_strategy_reveal(GtkWidget*, gpointer);
  };

}

#endif

