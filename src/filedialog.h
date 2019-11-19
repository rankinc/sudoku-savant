#ifndef FILE_DIALOG_H
#define FILE_DIALOG_H

#include <string>

#include "component.h"
#include "enums.h"

namespace sudoku
{
  extern "C"
  {
    void on_file_open_clicked(GtkButton*, gpointer);
    void on_file_save_clicked(GtkButton*, gpointer);
    void on_file_cancel_clicked(GtkButton*, gpointer);
    void on_filedialog_delete_event(GtkWidget*, GdkEvent*, gpointer);
  }

  class Window;

  class FileDialog : public Component
  {
  private:
    GtkWidget* m_saveButton;
    GtkWidget* m_openButton;
    SAVING     m_mode;

  public:
    explicit FileDialog(Window*);
    ~FileDialog();

    GtkFileChooser* getGtkFileChooser() {
      return GTK_FILE_CHOOSER(getGtkWidget());
    }

    const GtkFileChooser* getGtkFileChooser() const {
      return GTK_FILE_CHOOSER(getGtkWidget());
    }

    void showOpen();
    void showSave();

    SAVING getMode() const { return m_mode; }
    void setMode(SAVING s) { m_mode = s; }

    std::string& getFileName(std::string &) const;
  };
}

#endif

