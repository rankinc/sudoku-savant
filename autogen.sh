#! /bin/sh

aclocal \
&& autopoint \
&& autoheader \
&& automake --add-missing \
&& autoconf
