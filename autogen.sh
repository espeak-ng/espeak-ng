#!/bin/sh

LIBTOOLIZE=`which libtoolize`
if ! test -f "$LIBTOOLIZE" ; then
	# Mac OSX support ...
	LIBTOOLIZE=`which glibtoolize`
fi

mkdir -p m4
touch AUTHORS
touch NEWS
ln -svf README.md README

aclocal -I m4 || exit 1

${LIBTOOLIZE} || exit 1
autoheader || exit 1
automake --add-missing || exit 1
autoconf || exit 1
