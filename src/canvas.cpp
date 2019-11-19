#include "canvas.h"
#include "board.h"
#include "square.h"
#include "toolbar.h"
#include "window.h"
#include "document.h"

#define _RGB_(r, g, b)  (r)*256, (g)*256, (b)*256

extern "C"
{
  #include "interface.h"
  #include "support.h"
}

namespace sudoku
{

const GdkColor Canvas::RED       = { 0, _RGB_(255,   0,   0) };
const GdkColor Canvas::BLUE      = { 0, _RGB_(  0,   0, 255) };
const GdkColor Canvas::GREEN     = { 0, _RGB_( 56, 134,  11) };
const GdkColor Canvas::DARKRED   = { 0, _RGB_(180,  41,  59) };

/*********************************************************************
 */
class DrawRef : public Document::SquareRefAction
{
private:
  Canvas *const m_canvas;
  const GdkColor *m_fg;

public:
  DrawRef(Canvas *cv, const GdkColor *fg)
    : m_canvas(cv)
    , m_fg(fg)
  {}

  void operator()(Square *s) const { m_canvas->draw(*s, m_fg); }
};


/*********************************************************************
 */
class ShowError : public Board::SquareAction
{
private:
  Canvas* const m_canvas;
  const Board &m_solution;
  const GdkColor *const m_fg;
  const GdkColor *const m_bg;

public:
  ShowError(Canvas *c, const Board &b, const GdkColor *fg, const GdkColor *bg)
    : m_canvas(c)
    , m_solution(b)
    , m_fg(fg)
    , m_bg(bg)
  {}

  void operator()(Square&) const;
};


void
ShowError::operator()(Square &s) const
{
  if ( Document::isErrorMove(m_solution, s) )
  {
    m_canvas->show(s, m_fg, m_bg);
  }
}


/*********************************************************************
 */
class DrawNote : public Document::NoteAction
{
protected:
  Canvas *const m_canvas;
  const DRAWSTATE m_state;

public:
  explicit DrawNote(Canvas *cv, DRAWSTATE state=DRAW)
    : m_canvas(cv)
    , m_state(state)
  {}
  void operator()(Note&, Note::size_type idx) const;
  void operator()(const Note&, const Square&, GdkRectangle&) const;
};


void
DrawNote::operator()(Note &n, Note::size_type idx) const
{
  if ( n.hasMarks() || n.hasColour() )
  {
    GdkRectangle rect;
    (*this)(n, m_canvas->getBoard()->find(idx), rect);
  }
}


void
DrawNote::operator()(const Note &n,
                     const Square &sq,
                     GdkRectangle &rect) const
{
  m_canvas->getCellRectangle(sq, rect);

  const GdkColor *foregroundColour = &Canvas::DARKRED;
  const GdkColor *backgroundColour = ToolBar::getColour(n.getColour()); 

  if ( m_state == HIGHLIGHT ) {
    foregroundColour = ToolBar::getColour(0);
    backgroundColour = ToolBar::getHighlightColour();
  }

  if ( (m_state != DRAW) || n.hasColour() ) {
    /*
     * Realizing the colour-pad's button widgets should have
     * already allocated the pixel elements in its private
     * colour array, and this is the pixel element that the
     * "set forground" function needs.
     */
    m_canvas->drawRectangle(rect, backgroundColour);
  }

  for (Note::size_type i = 0; i < Note::MAX_MARKS; ++i)
  {
    if ( n.hasMarkAt(i) )
    {
      GdkRectangle r(rect);
      m_canvas->drawMark( Canvas::adjustForMark(r, i), n[i],
                            foregroundColour, backgroundColour );
    }
  } /* for */
}


/*********************************************************************
 */
class ShowHighlight : public DrawNote
{
private:
  const unsigned m_highlight;

public:
  explicit ShowHighlight(Canvas *cv, unsigned h, DRAWSTATE state)
    : DrawNote(cv, state)
    , m_highlight(h)
  {}
  void operator()(Note&, Note::size_type idx) const;
};


void
ShowHighlight::operator()(Note &n, Note::size_type idx) const
{
  if ( n.hasMark( m_highlight ) )
  {
    GdkRectangle rect;
    DrawNote::operator()(n, m_canvas->getBoard()->find(idx), rect);
    m_canvas->refresh(rect);
  }
}


/*********************************************************************
 */
gboolean
on_canvas_configure(GtkWidget*, GdkEventConfigure *ev, gpointer user_data)
{
  sudoku::Canvas *canvas = static_cast<sudoku::Canvas*>(user_data);
  if (canvas->isDirty() || ev->send_event)
    canvas->repaint();
  return TRUE;
}


gboolean
on_canvas_expose_event(GtkWidget      *widget,
                       GdkEventExpose *event,
                       gpointer        user_data)
{
  sudoku::Canvas *canvas = static_cast<sudoku::Canvas*>(user_data);

  if (canvas->m_backingStore != NULL)
  {
    gdk_draw_drawable(widget->window,
                      widget->style->fg_gc[GTK_WIDGET_STATE(widget)],
                      canvas->m_backingStore,
                      event->area.x, event->area.y,
                      event->area.x, event->area.y,
                      event->area.width,
                      event->area.height);
  }

  return FALSE;
}


gboolean
on_canvas_button_press_event(GtkWidget*,
                             GdkEventButton *event,
                             gpointer        user_data)
{
  sudoku::Canvas *canvas = static_cast<sudoku::Canvas*>(user_data);

  gint xx = sudoku::Canvas::getClickableDistance(event->x);
  gint yy = sudoku::Canvas::getClickableDistance(event->y);

  if ( (xx >= 0) &&
       (xx < canvas->getClickableWidth()) &&
       (yy >= 0) &&
       (yy < canvas->getClickableHeight()) )
  {
    Board* board = canvas->getBoard();
    const Geometry& geometry = board->getGeometry();

    gint row = sudoku::Canvas::getCellRowIndex(yy, geometry);
    gint col = sudoku::Canvas::getCellColumnIndex(xx, geometry);

    Square &square = (*board)[row][col];

    switch (event->button)
    {
    case 1:
      canvas->getMainWindow()->onLeftClick(square);
      break;

    case 3:
      canvas->getMainWindow()->onRightClick(square,
                                 Canvas::getMarkIndex(square,
                                                 static_cast<gint>(event->x),
                                                 static_cast<gint>(event->y)));
      break;

    default:
      break;
    } /* switch */
  }

  return FALSE;
}

/*********************************************************************
 */
Canvas::Canvas(Window *w)
  : Component( w, gtk_drawing_area_new() )
  , m_backingStore(NULL)
  , m_isDirty(false)
{
  GtkWidget *alignment;

  alignment = lookup_widget( GTK_WIDGET(w->getGtkWidget()),
                             "canvas_alignment" );
  if ( alignment != NULL )
  {
    gtk_container_add(GTK_CONTAINER(alignment), getGtkWidget());
    gtk_widget_set_events(getGtkWidget(),
                          GDK_EXPOSURE_MASK | GDK_BUTTON_PRESS_MASK);

    g_signal_connect(static_cast<gpointer>(getGtkWidget()),
                     "configure_event",
                     G_CALLBACK(on_canvas_configure),
                     this);
    g_signal_connect(static_cast<gpointer>(getGtkWidget()),
                     "expose_event",
                     G_CALLBACK(on_canvas_expose_event),
                     this);
    g_signal_connect(static_cast<gpointer>(getGtkWidget()),
                     "button_press_event",
                     G_CALLBACK(on_canvas_button_press_event),
                     this);

    resize();
    show();
  }
}


/*********************************************************************
 */
Canvas::~Canvas()
{
  if (m_backingStore != NULL)
    g_object_unref(m_backingStore);
}


/*******************************************************************
 */
Document*
Canvas::getDocument() const
{
  return getMainWindow()->getDocument();
}


/*******************************************************************
 */
Board*
Canvas::getBoard() const
{
  return getDocument()->getBoard();
}


/*********************************************************************
 */
gint
Canvas::getBoardWidth() const
{
  const Geometry &geometry = getBoard()->getGeometry();
  return (geometry.getMaxNumber() * (CELL_SIZE + CELL_LINE_WIDTH))
         + geometry.getBoxesPerRow() * (BOX_LINE_WIDTH - CELL_LINE_WIDTH)
         + BOX_LINE_WIDTH;
}


/*********************************************************************
 */
gint
Canvas::getBoardHeight() const
{
  const Geometry &geometry = getBoard()->getGeometry();
  return (geometry.getMaxNumber() * (CELL_SIZE + CELL_LINE_WIDTH))
         + geometry.getBoxesPerColumn() * (BOX_LINE_WIDTH - CELL_LINE_WIDTH)
         + BOX_LINE_WIDTH;
}


/*********************************************************************
 */
void
Canvas::drawBoxLines(GdkGC *gc, const Geometry &geometry)
{
  const Geometry::size_type segsize = geometry.getBoxesPerRow()
                        + geometry.getBoxesPerColumn()
                        + 2;
  auto_array_ptr<GdkSegment> segs( new GdkSegment[ segsize ] );
  GdkSegment *segment = segs.get();

  const gint boardWidth = getBoardWidth();
  gint height = TOP_BORDER_HEIGHT + (BOX_LINE_WIDTH / 2);
  Geometry::size_type i = 0;

  while (i <= geometry.getBoxesPerColumn())
  {
    segment[i].x1 = LEFT_BORDER_WIDTH;
    segment[i].x2 = LEFT_BORDER_WIDTH + boardWidth;
    segment[i].y1 = segment[i].y2 = height;

    height += (geometry.getBoxHeight() * (CELL_SIZE + CELL_LINE_WIDTH))
              + (BOX_LINE_WIDTH - CELL_LINE_WIDTH);
    ++i;
  } /* while */

  const gint boardHeight = getBoardHeight();
  gint width = LEFT_BORDER_WIDTH + (BOX_LINE_WIDTH / 2);

  while (i < segsize)
  {
    segment[i].y1 = TOP_BORDER_HEIGHT;
    segment[i].y2 = TOP_BORDER_HEIGHT + boardHeight;
    segment[i].x1 = segment[i].x2 = width;

    width += (geometry.getBoxWidth() * (CELL_SIZE + CELL_LINE_WIDTH))
             + (BOX_LINE_WIDTH - CELL_LINE_WIDTH);
    ++i;
  } /* while */

  gdk_gc_set_line_attributes(gc, BOX_LINE_WIDTH,
                                 GDK_LINE_SOLID,
                                 GDK_CAP_BUTT,
                                 GDK_JOIN_MITER);
  gdk_draw_segments(m_backingStore, gc, segment, segsize);
}


/*********************************************************************
 */
void
Canvas::drawCellLines(GdkGC *gc, const Geometry &geometry)
{
  const Geometry::size_type horizontals
             = (geometry.getBoxHeight() - 1) * geometry.getBoxesPerColumn();
  const Geometry::size_type verticals
             = (geometry.getBoxWidth() - 1) * geometry.getBoxesPerRow();
  const Geometry::size_type segsize = horizontals + verticals;
  auto_array_ptr<GdkSegment> segs( new GdkSegment[ segsize ] );
  GdkSegment *segment = segs.get();

  const gint boardWidth = getBoardWidth();
  gint height = TOP_BORDER_HEIGHT + (CELL_LINE_WIDTH / 2);
  Geometry::size_type i = 0;
  unsigned n = 0;

  while (i < horizontals )
  {
    drawLabel((LEFT_BORDER_WIDTH - LABEL_SIZE)/2, height + 4, n++);

    height += (CELL_SIZE + BOX_LINE_WIDTH);

    for (Geometry::size_type j = 1; j < geometry.getBoxHeight(); ++j)
    {
      segment[i].x1 = LEFT_BORDER_WIDTH;
      segment[i].x2 = LEFT_BORDER_WIDTH + boardWidth;
      segment[i].y1 = segment[i].y2 = height;

      drawLabel((LEFT_BORDER_WIDTH - LABEL_SIZE)/2, height + 4, n++);
      height += (CELL_SIZE + CELL_LINE_WIDTH);
      ++i;
    } /* for */
  } /* while */

  const gint boardHeight = getBoardHeight();
  gint width = LEFT_BORDER_WIDTH + (CELL_LINE_WIDTH / 2);
  n = 0;

  while (i < segsize)
  {
    drawLabel(width + (CELL_SIZE - LABEL_SIZE)/2, 2, n++);

    width += (CELL_SIZE + BOX_LINE_WIDTH);

    for (Geometry::size_type j = 1; j < geometry.getBoxWidth(); ++j)
    {
      segment[i].y1 = TOP_BORDER_HEIGHT;
      segment[i].y2 = TOP_BORDER_HEIGHT + boardHeight;
      segment[i].x1 = segment[i].x2 = width;

      drawLabel(width + (CELL_SIZE - LABEL_SIZE)/2, 2, n++);
      width += (CELL_SIZE + CELL_LINE_WIDTH);
      ++i;
    } /* for */
  } /* while */

  gdk_gc_set_line_attributes(gc, CELL_LINE_WIDTH,
                                 GDK_LINE_SOLID,
                                 GDK_CAP_BUTT,
                                 GDK_JOIN_MITER);
  gdk_draw_segments(m_backingStore, gc, segment, segsize);
}


/*********************************************************************
 */
void
Canvas::drawGrid(GdkGC *gcGrid, GdkGC *gcCells)
{
  const Geometry &geometry = getBoard()->getGeometry();
  drawCellLines(gcCells, geometry);
  drawBoxLines(gcGrid, geometry);
}


/*********************************************************************
 */
void
Canvas::drawLabel(int x, int y, unsigned n)
{
  PangoLayout *layout = getLayoutFor(n);
  if (layout != NULL)
  {
    pango_layout_set_alignment(layout, PANGO_ALIGN_CENTER);
    pango_layout_set_width(layout, LABEL_SIZE * PANGO_SCALE);

    PangoContext *context = pango_layout_get_context(layout);
    PangoFontDescription *fontDesc;

    fontDesc = pango_context_get_font_description(context);
    pango_font_description_set_weight(fontDesc, PANGO_WEIGHT_BOLD);
    pango_font_description_set_size(fontDesc, LABEL_FONT_SIZE * PANGO_SCALE);

    gdk_draw_layout( m_backingStore,
                     getGtkWidget()->style->fg_gc[GTK_STATE_NORMAL],
                     x,
                     y,
                     layout );

    g_object_unref(layout);
  }
}


/*********************************************************************
 */
void
Canvas::drawRectangle(const GdkRectangle &rect, const GdkColor *colour)
{
  GdkGC *gc;

  if (colour == NULL)
  {
    gc = getGtkWidget()->style->white_gc;
  }
  else
  {
    gc = gdk_gc_new( GDK_DRAWABLE(getGtkWidget()->window) );
    gdk_gc_set_foreground(gc, colour);
  }

  gdk_draw_rectangle(m_backingStore,
                     gc,
                     TRUE,
                     rect.x,
                     rect.y,
                     rect.width,
                     rect.height);

  if (colour != NULL)
    g_object_unref(gc);
}


/*********************************************************************
 */
void
Canvas::drawMark(GdkRectangle &rect, unsigned n,
                 const GdkColor *foreColour, const GdkColor *backColour)
{
  PangoLayout *layout = getLayoutFor(n);
  if (layout != NULL)
  {
    pango_layout_set_alignment(layout, PANGO_ALIGN_CENTER);
    pango_layout_set_width(layout, MARK_SIZE * PANGO_SCALE);

    PangoContext *context = pango_layout_get_context(layout);
    PangoFontDescription *fontDesc;

    fontDesc = pango_context_get_font_description(context);
    pango_font_description_set_size(fontDesc, MARK_FONT_SIZE * PANGO_SCALE);

    gdk_draw_layout_with_colors(
                            m_backingStore,
                            getGtkWidget()->style->fg_gc[GTK_STATE_NORMAL],
                            rect.x,
                            rect.y,
                            layout,
                            foreColour,
                            backColour);

    g_object_unref(layout);
  }
}


/*********************************************************************
 */
void
Canvas::showErrors(const Board *board,
                   const Board &solution,
                   const GdkColor *fg,
                   const GdkColor *bg)
{
  if (board != NULL)
    const_cast<Board*>(board)->doAllSquares(
                                        ShowError(this, solution, fg, bg) );
}


/*********************************************************************
 */
void
Canvas::clear()
{
  if (m_backingStore != NULL)
    g_object_unref(m_backingStore);

  GtkWidget* widget = getGtkWidget();

  m_backingStore = gdk_pixmap_new(widget->window,
                                  widget->allocation.width,
                                  widget->allocation.height,
                                  -1);
  gdk_draw_rectangle(m_backingStore,
                     widget->style->white_gc,
                     TRUE,
                     0,
                     0,
                     widget->allocation.width,
                     widget->allocation.height);
  gdk_draw_rectangle(m_backingStore,
                     widget->style->black_gc,
                     FALSE,
                     0,
                     0,
                     widget->allocation.width - 1,
                     widget->allocation.height - 1);
  drawGrid( widget->style->fg_gc[GTK_STATE_NORMAL],
            widget->style->fg_gc[GTK_STATE_INSENSITIVE] );
}


/*********************************************************************
 */
void
Canvas::repaint()
{
  clear();
  draw( *getDocument() );
  refresh();
  clean();
}


/*********************************************************************
 */
void
Canvas::resize()
{
  GtkWidget *w = getGtkWidget();
  const gint width = getCanvasWidth();
  const gint height = getCanvasHeight();

  if ((w->allocation.width != width) || (w->allocation.height != height))
  {
    gtk_widget_set_size_request(w, width, height);
    dirty();
  }
}


/*********************************************************************
 */
void
Canvas::refresh() const
{
  const GtkWidget *w = getGtkWidget();

  /*
   * We only need to refresh the active part of the canvas,
   * not the unchanging bit around the edge.
   */
  gtk_widget_queue_draw_area(
      const_cast<GtkWidget*>(w),
      LEFT_BORDER_WIDTH + BOX_LINE_WIDTH,
      TOP_BORDER_HEIGHT + BOX_LINE_WIDTH,
      w->allocation.width
              - (LEFT_BORDER_WIDTH + RIGHT_BORDER_WIDTH + 2*BOX_LINE_WIDTH),
      w->allocation.height
              - (TOP_BORDER_HEIGHT + BOTTOM_BORDER_HEIGHT + 2*BOX_LINE_WIDTH)
  );
}


/*********************************************************************
 */
PangoLayout*
Canvas::getLayoutFor(unsigned n) const
{
  gchar *number = g_strdup_printf("%u", n);

  PangoLayout *layout = gtk_widget_create_pango_layout(getGtkWidget(), number);
  g_free(number);

  return layout;
}


/*********************************************************************
 */
void
Canvas::draw(const GdkRectangle &rect,
             unsigned n,
             const GdkColor *fg,
             const GdkColor *bg)
{
  /*
   * Clear away any marks that we may have written ...
   */
  drawRectangle(rect, bg);

  /*
   * Draw a number here, assuming we have one.
   */
  if (n != 0)
  {
    PangoLayout *layout = getLayoutFor(n);
    if (layout != NULL)
    {
      pango_layout_set_alignment(layout, PANGO_ALIGN_CENTER);
      pango_layout_set_width(layout, CELL_SIZE * PANGO_SCALE);

      PangoContext *context = pango_layout_get_context(layout);
      PangoFontDescription *fontDesc;

      fontDesc = pango_context_get_font_description(context);
      pango_font_description_set_weight(fontDesc, PANGO_WEIGHT_BOLD);
      pango_font_description_set_size(fontDesc, CELL_FONT_SIZE * PANGO_SCALE);

      gdk_draw_layout_with_colors(
                              m_backingStore,
                              getGtkWidget()->style->fg_gc[GTK_STATE_NORMAL],
                              rect.x,
                              rect.y,
                              layout,
                              fg,
                              bg);

      g_object_unref(layout);
    }
  }
}


/*********************************************************************
 */
void
Canvas::draw(const Square &square, const GdkColor *fg, const GdkColor *bg)
{
  GdkRectangle rect;
  draw(getCellRectangle(square, rect), square.getNumber(), fg, bg);
}


/*********************************************************************
 */
void
Canvas::draw(const Board *board, const GdkColor *fg)
{
  if (board != NULL)
    const_cast<Board*>(board)->doAllSquares( Draw<Square>(this, fg) );
}


/*********************************************************************
 */
void
Canvas::drawOver(const Square &square, const GdkColor *fg, const GdkColor *bg)
{
  if (square.getNumber() != 0)
  {
    draw(square, fg, bg);
  }
}


/*********************************************************************
 */
void
Canvas::drawOver(const Board *board, const GdkColor *fg)
{
  if (board != NULL)
    const_cast<Board*>(board)->doAllSquares( DrawOver<Square>(this, fg) );
}


/*********************************************************************
 * Write our changes to the backing store onto the window.
 */
void
Canvas::show(const Square &square, const GdkColor *fg, const GdkColor *bg)
{
  GdkRectangle rect;

  draw(getCellRectangle(square, rect), square.getNumber(), fg, bg);
  refresh(rect);
}


/*********************************************************************
 */
void
Canvas::refresh(GdkRectangle &rect)
{
  gtk_widget_queue_draw_area(getGtkWidget(),
                             rect.x,
                             rect.y,
                             rect.width,
                             rect.height);
}


/*********************************************************************
 */
void
Canvas::show(const Board *board, const GdkColor *fg)
{
  if (board != NULL)
  {
    const_cast<Board*>(board)->doAllSquares( Draw<Square>(this, fg) );
    refresh();
  }
}


/*********************************************************************
 */
void
Canvas::showMark(const Square &square,
                 const Note &note,
                 unsigned nidx)
{
  const GdkColor *foreground = &DARKRED;
  const GdkColor *background = ToolBar::getColour(note.getColour());

  if ( note.hasMark(getMainWindow()->getHighlightNumber()) )
  {
    foreground = ToolBar::getColour(0);
    background = ToolBar::getHighlightColour();
  }

  GdkRectangle rect;

  drawRectangle( adjustForMark(getCellRectangle(square, rect), nidx),
                 background );
  if ( note.hasMarkAt(nidx) )
    drawMark( rect, note.getMarkAt(nidx), foreground, background );

  refresh(rect);
}


/*********************************************************************
 */
void
Canvas::showHighlightNumber(unsigned highlight, DRAWSTATE state)
{
  // Only those squares containing the highlight number are affected
  Document::NoteBook *notebook = getDocument()->getNoteBook();
  if ( notebook != NULL )
    notebook->do_for_each( ShowHighlight(this, highlight, state) );
}


/*********************************************************************
 */
void
Canvas::paintSquare(const Square &s, const Note &note, DRAWSTATE state)
{
  GdkRectangle rect;
  DrawNote(this, state)(note, s, rect);
  refresh(rect);
}


/*********************************************************************
 */
void
Canvas::draw(const Document &document)
{
  draw( document.getOriginalBoard() );
  document.do_for_each_hint( DrawRef(this, &RED) );
  document.do_for_each_move( DrawRef(this, &BLUE) );

  Document::NoteBook *notebook = document.getNoteBook();
  if ( notebook != NULL )
    notebook->do_for_each( DrawNote(this) );
}


/*********************************************************************
 */
gint
Canvas::getCellDistance(gint index, gint gridSize)
{
  return LEFT_BORDER_WIDTH +
         BOX_LINE_WIDTH +
         (index * (CELL_SIZE + CELL_LINE_WIDTH)) +
         ((index / gridSize) * (BOX_LINE_WIDTH - CELL_LINE_WIDTH));
}


/*********************************************************************
 */
gint
Canvas::getCellYDistance(const Square &square)
{
  return getCellDistance(square.getRowIndex(),
                         square.getGeometry().getBoxHeight());
}


/*********************************************************************
 */
gint
Canvas::getCellXDistance(const Square &square)
{
  return getCellDistance(square.getColumnIndex(),
                         square.getGeometry().getBoxWidth());
}


/*********************************************************************
 */
GdkRectangle&
Canvas::getCellRectangle(const Square &square, GdkRectangle &rect)
{
  rect.x = getCellXDistance(square);
  rect.y = getCellYDistance(square);

  rect.width = rect.height = CELL_SIZE;
  
  return rect;
}


/*********************************************************************
 */
gint
Canvas::getCellRowIndex(gint y, const Geometry &geometry)
{
  const gint boxHeight
               = (CELL_SIZE + CELL_LINE_WIDTH) * geometry.getBoxHeight()
                 + (BOX_LINE_WIDTH - CELL_LINE_WIDTH);
  return (y - (y / boxHeight) * (BOX_LINE_WIDTH - CELL_LINE_WIDTH))
            / (CELL_SIZE + CELL_LINE_WIDTH);
}


/*********************************************************************
 */
gint
Canvas::getCellColumnIndex(gint x, const Geometry &geometry)
{
  const gint boxWidth
               = (CELL_SIZE + CELL_LINE_WIDTH) * geometry.getBoxWidth()
                 + (BOX_LINE_WIDTH - CELL_LINE_WIDTH);
  return (x - (x / boxWidth) * (BOX_LINE_WIDTH - CELL_LINE_WIDTH))
            / (CELL_SIZE + CELL_LINE_WIDTH);
}


/*********************************************************************
 */
GdkRectangle&
Canvas::adjustForMark(GdkRectangle &rect, unsigned nidx)
{
  if (nidx > 1)
    rect.y += MARK_SIZE;

  if ((nidx & 1) != 0)
    rect.x += MARK_SIZE;

  rect.width = rect.height = MARK_SIZE;

  return rect;
}


/*********************************************************************
 */
unsigned
Canvas::getMarkIndex(const Square &s, gint x, gint y)
{
  unsigned index = 0;

  if (x >= getCellXDistance(s) + MARK_SIZE)
    ++index;

  if (y >= getCellYDistance(s) + MARK_SIZE)
    index += 2;
  
  return index;
}


/*********************************************************************
 */
void
Canvas::restoreDefaults()
{
  resize();
}

}

