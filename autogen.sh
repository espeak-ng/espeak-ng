#!/bin/sh

mkdir -p m4
ln -sf README.md README
ln -sf docs/ReleaseNotes.md NEWS

aclocal -I m4 || exit 1

libtoolize || exit 1
autoheader || exit 1
automake --add-missing || exit 1
autoconf || exit 1
