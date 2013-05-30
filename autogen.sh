#!/bin/sh

mkdir -p m4
touch AUTHORS
touch NEWS
ln -sv README.md README

if [ -d .git ] ; then
	git submodule update --init --recursive || exit 1
fi

aclocal -I m4 || exit 1

libtoolize || exit 1
autoheader || exit 1
automake --add-missing || exit 1
autoconf || exit 1
