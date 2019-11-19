## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	@PACKAGE_CFLAGS@
AM_CXXFLAGS=@SUDOKU_CXXFLAGS@
AM_LDFLAGS=-Wl,-O -Wl,-zcombreloc -Wl,--sort-common -Wl,-znoexecstack -Wl,-zrelro

bin_PROGRAMS = sudoku-savant

sudoku_savant_SOURCES = \
  main.cpp \
  enums.h \
  constants.h \
  utils.cpp \
  loader.cpp \
  strip_blanks.h \
  strip_leading.cpp strip_trailing.cpp \
  date.cpp date.h \
  actions.h \
  document.cpp document.h \
  note.cpp note.h \
  widget.cpp widget.h \
  canvas.cpp canvas.h \
  window.cpp window.h \
  component.cpp component.h \
  numberpad.cpp numberpad.h \
  toolbar.cpp toolbar.h \
  filedialog.cpp filedialog.h \
  geodialog.cpp geodialog.h \
  strategydialog.cpp strategydialog.h \
  statusbar.cpp statusbar.h \
  label.cpp label.h \
  textlog.cpp textlog.h \
  solutionlog.cpp solutionlog.h \
  bitmask.cpp bitmask.h \
  subsets.h \
  pattern.h \
  validate.cpp \
  geometry.cpp geometry.h \
  square.cpp square.h \
  sequence.cpp sequence.h \
  solution.cpp solution.h \
  displaygrid.cpp displaygrid.h \
  board.cpp board.h \
  puzzle.cpp puzzle.h \
  strategy.cpp strategy.h \
  completesequences.cpp completesequences.h \
  crosssequences.cpp crosssequences.h \
  singleseqstrategy.cpp singleseqstrategy.h \
  subsetstrategy.cpp subsetstrategy.h \
  hiddensubsets.cpp hiddensubsets.h \
  nakedsubsets.cpp nakedsubsets.h \
  multiseqstrategy.cpp multiseqstrategy.h \
  multiseqpatterns.cpp multiseqpatterns.h \
  xywings.cpp xywings.h \
  xyzwings.cpp xyzwings.h \
  colouring.cpp colouring.h \
  freedom.cpp freedom.h \
  workset.cpp workset.h \
  selector.cpp selector.h \
  generator.cpp generator.h \
  aboutbox.cpp aboutbox.h \
  support.c support.h \
  callbacks.h \
  interface.c interface.h

sudoku_savant_LDADD = @PACKAGE_LIBS@ $(INTLLIBS)

