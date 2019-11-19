#ifndef TEXT_LOG_H
#define TEXT_LOG_H

#include "component.h"

namespace sudoku
{
  class Window;

  class TextLog : public Component
  {
  private:
    GtkTextBuffer* m_buffer;
    GtkTextIter    m_end;

  protected:
    void writeLine(const std::string&);
    void eraseLine(gint lineIndex);

  public:
    explicit TextLog(Window*, GtkWidget*);
    ~TextLog();

    const GtkTextView* getGtkTextView() const {
        return GTK_TEXT_VIEW(getGtkWidget());
    }

    GtkTextView* getGtkTextView() {
        return GTK_TEXT_VIEW(getGtkWidget());
    }

    void show(const std::string&);
    void clear();

    gint getLineCount() const {
        return gtk_text_buffer_get_line_count(m_buffer);
    }
  };
}

#endif
