#!/bin/sh

LIBTOOLIZE=`which libtoolize`
if ! test -f "$LIBTOOLIZE" ; then
	# Mac OSX support ...
	LIBTOOLIZE=`which glibtoolize`
fi

mkdir -p m4
touch AUTHORS
ln -sf ChangeLog.md NEWS
ln -sf README.md README

aclocal -I m4 || exit 1

${LIBTOOLIZE} --copy || exit 1
autoheader || exit 1
automake --add-missing --copy || exit 1
autoconf || exit 1
