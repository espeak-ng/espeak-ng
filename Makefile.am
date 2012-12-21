AUTOMAKE_OPTIONS = subdir-objects

localedir  = $(datadir)/locale
xdgdatadir = @XDGDATADIR@
mimedir    = $(xdgdatadir)/mime
xmldir     = $(mimedir)/packages

AM_LDFLAGS  = ${LTLIBINTL}
AM_CXXFLAGS = -Isrc/include -Isrc

ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS =
lib_LTLIBRARIES =
man1_MANS =
pkgdata_DATA =

noinst_bin_PROGRAMS =
noinst_LIBRARIES =

noinst_bindir =

EXTRA_DIST = config.rpath ChangeLog
CLEANFILES =
SUBDIRS =

############################# libtool #########################################

EXTRA_DIST += config.guess config.sub ltmain.sh

# Increment if the interface has changed and is not backward compatible
CURRENT=0

# Increment  if source files have changed
# Reset to 0 if the interface has changed
REVISION=0

# Increment  if the interface is backward compatible (superset)
# Reset to 0 if the interface is not backward compatible
AGE=0

LIBUCD_VERSION=$(CURRENT):$(REVISION):$(AGE)

############################# ChangeLog #######################################

ChangeLog:
	git log > ChangeLog

dist-hook: ChangeLog

.PHONY: ChangeLog

EXTRA_DIST += ChangeLog

############################# Unicode Character Database ######################

src/categories.cpp: tools/categories.py tools/ucd.py
	tools/categories.py ${UCD_ROOTDIR} > $@

############################# libucd ##########################################

libucd_includedir = $(includedir)/ucd
libucd_include_HEADERS = \
	src/include/ucd/ucd.h

lib_LTLIBRARIES += src/libucd.la

src_libucd_la_LDFLAGS = -version-info $(LIBUCD_VERSION)
src_libucd_la_CXXFLAGS = ${AM_CXXFLAGS}
src_libucd_la_SOURCES = \
	src/categories.cpp
