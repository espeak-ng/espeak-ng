#!/bin/sh

mkdir -p m4
touch AUTHORS
touch NEWS

aclocal -I m4 || exit 1

libtoolize || exit 1
autoheader || exit 1
automake --add-missing || exit 1
autoconf || exit 1
