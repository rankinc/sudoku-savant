#ifndef CANVAS_H
#define CANVAS_H

#include <iostream>
#include <gtk/gtk.h>

#include "component.h"
#include "actions.h"
#include "constants.h"
#include "enums.h"

namespace sudoku
{
  extern "C"
  {
    gboolean on_canvas_configure(GtkWidget*,
                                 GdkEventConfigure*,
                                 gpointer);
    gboolean on_canvas_expose_event(GtkWidget*,
                                    GdkEventExpose*,
                                    gpointer);
    gboolean on_canvas_button_press_event(GtkWidget*,
                                          GdkEventButton*,
                                          gpointer);
  }

  class Board;
  class Geometry;
  class Square;
  class Window;
  class Document;
  class Note;

  class Canvas : public Component
  {
  private:
    static const gint LABEL_SIZE = 12;
    static const gint CELL_SIZE = 28;
    static const gint CELL_LINE_WIDTH = 1;
    static const gint BOX_LINE_WIDTH = 2;
    static const gint LEFT_BORDER_WIDTH = 20;
    static const gint RIGHT_BORDER_WIDTH = 12;
    static const gint TOP_BORDER_HEIGHT = 20;
    static const gint BOTTOM_BORDER_HEIGHT = 12;

    static const gint MARK_SIZE = (CELL_SIZE / 2);

    static const gint CELL_FONT_SIZE = 14;
    static const gint MARK_FONT_SIZE = 7;
    static const gint LABEL_FONT_SIZE = 10;

    template<class T>
    class Draw : public Action<T>
    {
    private:
      Canvas* const m_canvas;
      const GdkColor *const m_fg;

    public:
      Draw(Canvas *c, const GdkColor *fg)
        : m_canvas(c)
        , m_fg(fg)
      {}
      void operator()(T &x) const { m_canvas->draw(x, m_fg); }
    };

    template<class T>
    class DrawOver : public Action<T>
    {
    private:
      Canvas* const m_canvas;
      const GdkColor *const m_fg;

    public:
      DrawOver(Canvas*c, const GdkColor *fg)
        : m_canvas(c)
        , m_fg(fg)
      {}
      void operator()(T &x) const { m_canvas->drawOver(x, m_fg); }
    };

    GdkGC     *m_context;

    GdkPixmap *m_backingStore;
    bool       m_isDirty;

    gint getCanvasWidth() const {
        return getBoardWidth() + LEFT_BORDER_WIDTH + RIGHT_BORDER_WIDTH;
    }

    gint getCanvasHeight() const {
        return getBoardHeight() + TOP_BORDER_HEIGHT + BOTTOM_BORDER_HEIGHT;
    }

    gint getClickableWidth() const {
        return getBoardWidth() - 2*BOX_LINE_WIDTH;
    }

    gint getClickableHeight() const {
        return getBoardHeight() - 2*BOX_LINE_WIDTH;
    }

    gint getBoardWidth() const;
    gint getBoardHeight() const;

    PangoLayout* getLayoutFor(unsigned) const;

    void drawBoxLines(GdkGC*, const Geometry&);
    void drawCellLines(GdkGC*, const Geometry&);

    void drawGrid(GdkGC*, GdkGC*);

    void draw(const GdkRectangle&,
              unsigned,
              const GdkColor*,
              const GdkColor*);
    void drawRectangle(const GdkRectangle&, const GdkColor*);
    void drawLabel(int x, int y, unsigned);
    void drawMark(GdkRectangle&, unsigned, const GdkColor*, const GdkColor*);
    void refresh(GdkRectangle&);

  public:
    explicit Canvas(Window*);
    ~Canvas();

    Board* getBoard() const;
    Document* getDocument() const;

    void clear();
    void repaint();
    void resize();

    void showErrors(const Board*,
                    const Board&,
                    const GdkColor*,
                    const GdkColor*);

    void draw(const Square&, const GdkColor* =NULL, const GdkColor* =NULL);
    void draw(const Board*, const GdkColor* =NULL);

    void drawOver(const Square&, const GdkColor* =NULL, const GdkColor* =NULL);
    void drawOver(const Board*, const GdkColor* =NULL);

    void draw(const Document&);

    void show(const Square&, const GdkColor* =NULL, const GdkColor* =NULL);
    void showMark(const Square&, unsigned, unsigned, const GdkColor*, const GdkColor*);
    void showMark(const Square&, const Note&, unsigned);
    void show(const Board*, const GdkColor* =NULL);
    void showHighlightNumber(unsigned, DRAWSTATE);

    void refresh() const;

    void show() const { Component::show(); }

    void paintSquare(const Square&, const Note&, DRAWSTATE);

    void restoreDefaults();

    bool isDirty() const { return m_isDirty; }
    void dirty() { m_isDirty = true; }
    void clean() { m_isDirty = false; }

    static GdkRectangle& getCellRectangle(const Square&, GdkRectangle&);
    static gint getCellDistance(gint index, gint boxSize);
    static gint getCellXDistance(const Square&);
    static gint getCellYDistance(const Square&);

    static unsigned getMarkIndex(const Square&, gint, gint);
    static GdkRectangle& adjustForMark(GdkRectangle&, unsigned mark);

    static gint getClickableDistance(gdouble d) {
        return (static_cast<gint>(d) - LEFT_BORDER_WIDTH - BOX_LINE_WIDTH);
    }

    static gint getCellRowIndex(gint d, const Geometry&);
    static gint getCellColumnIndex(gint d, const Geometry&);

    static const GdkColor RED;
    static const GdkColor BLUE;
    static const GdkColor GREEN;
    static const GdkColor DARKRED;

    friend gboolean on_canvas_expose_event(GtkWidget*,
                                           GdkEventExpose*,
                                           gpointer);
    friend gboolean on_canvas_button_press_event(GtkWidget*,
                                                 GdkEventButton*,
                                                 gpointer);
    friend class ShowHighlight;
    friend class DrawNote;
  };
}

#endif
