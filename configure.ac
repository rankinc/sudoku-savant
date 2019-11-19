dnl Process this file with autoconf to produce a configure script.

AC_INIT(configure.ac)
AM_INIT_AUTOMAKE(sudoku-savant, 1.3.1)
AM_MAINTAINER_MODE
AM_CONFIG_HEADER(config.h)

AC_LANG_CPLUSPLUS
AC_ISC_POSIX
AC_PROG_CXX
AM_PROG_CC_STDC
AC_HEADER_STDC

AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION(0.15)
dnl AM_XGETTEXT_OPTION([--from-code=UTF-8])

pkg_modules="gtk+-2.0 >= 2.4.0"
PKG_CHECK_MODULES(PACKAGE, [$pkg_modules])
AC_SUBST(PACKAGE_CFLAGS)
AC_SUBST(PACKAGE_LIBS)

SUDOKU_CXXFLAGS="-fno-implement-inlines -std=c++11 -pedantic -pipe -Wall -Winline -Wshadow -Wsign-compare -Woverloaded-virtual"
AC_SUBST(SUDOKU_CXXFLAGS)

dnl The "-frepo" option needs separate compile and link steps,
dnl which the standard autoconf macros do not support. So it's
dnl time for some good ol' Cut-And-Paste.
dnl
dnl The "repository check" demo code was copied from RedHat Bugzilla.
m4_define([_AC_COMPILE_LINK_IFELSE],
[m4_ifvaln([$1], [AC_LANG_CONFTEST([$1])])dnl
rm -f conftest.$ac_objext conftest.$ac_exeext
AS_IF([_AC_EVAL_STDERR($ac_compile) &&
       _AC_EVAL_STDERR($CXX -o conftest$ac_exeext $CXXFLAGS $CPPFLAGS $LDFLAGS conftest.$ac_objext $LIBS >&AS_MESSAGE_LOG_FD) &&
       AC_TRY_COMMAND([test -z "$ac_[]_AC_LANG_ABBREV[]_werror_flag"
                      || test ! -s conftest.err]) &&
       AC_TRY_COMMAND([test -s conftest$ac_exeext])],
      [$2],
      [_AC_MSG_LOG_CONFTEST
m4_ifvaln([$3], [$3])dnl])
rm -f conftest.err conftest.$ac_objext conftest.rpo \
      conftest$ac_exeext m4_ifval([$1], [conftest.$ac_ext])[]dnl 
])# _AC_COMPILE_LINK_IFELSE

AC_DEFUN([AC_COMPILE_LINK_IFELSE],
[AC_LANG_COMPILER_REQUIRE()dnl
_AC_COMPILE_LINK_IFELSE($@)])

AU_DEFUN([AC_TRY_COMPILE_LINK],
[AC_COMPILE_LINK_IFELSE([AC_LANG_PROGRAM([[$1]], [[$2]])], [$3], [$4])])

SAVE_CXXFLAGS="$CXXFLAGS"
CXXFLAGS="-frepo $CXXFLAGS"
AC_MSG_CHECKING([for working g++ template repository])
AC_TRY_COMPILE_LINK([
template<typename T>
class R
{
public:
  R();
};

template<typename T>
R<T>::R() {}
],
[ R<int>  _x ], repo_ok=yes, repo_ok=no)
AC_MSG_RESULT($repo_ok)
if test x"$repo_ok" = xyes; then
    SUDOKU_CXXFLAGS="-frepo $SUDOKU_CXXFLAGS"
fi
CXXFLAGS="$SAVE_CXXFLAGS"

CXXFLAGS="$CXXFLAGS -Wextra"
AC_MSG_CHECKING([whether compiler understands -Wextra])
AC_TRY_COMPILE([inline int foo() { return 0; }],[], param_ok=yes, param_ok=no)
AC_MSG_RESULT($param_ok)
if test x"$param_ok" == xyes; then
    SUDOKU_CXXFLAGS="$SUDOKU_CXXFLAGS -Wextra"
else
    SUDOKU_CXXFLAGS="$SUDOKU_CXXFLAGS -W"
fi
CXXFLAGS="$SAVE_CXXFLAGS"

CXXFLAGS="$CXXFLAGS --param max-inline-insns-single=400"
AC_MSG_CHECKING([whether compiler understands --param max-inline-insns-single]) 
AC_TRY_COMPILE([inline int foo() { return 0; }],[], param_ok=yes, param_ok=no)
AC_MSG_RESULT($param_ok)
if test x"$param_ok" == xyes; then
    SUDOKU_CXXFLAGS="$SUDOKU_CXXFLAGS --param max-inline-insns-single=400"
fi
CXXFLAGS="$SAVE_CXXFLAGS"

GETTEXT_PACKAGE=sudoku-savant
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE", [Gettext package.])

AC_OUTPUT([
Makefile
src/Makefile
po/Makefile.in
sudoku-savant.desktop
])

