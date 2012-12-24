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

UCD_VERSION=6.2.0
UCD_ROOTDIR=data/ucd

data/language-subtag-registry:
	mkdir -pv data
	wget -O $@ http://www.iana.org/assignments/language-subtag-registry

data/ucd/PropList.txt:
	mkdir -pv data/ucd
	wget -O $@ http://www.unicode.org/Public/${UCD_VERSION}/ucd/PropList.txt

data/ucd/Scripts.txt:
	mkdir -pv data/ucd
	wget -O $@ http://www.unicode.org/Public/${UCD_VERSION}/ucd/Scripts.txt

data/ucd/UnicodeData.txt:
	mkdir -pv data/ucd
	wget -O $@ http://www.unicode.org/Public/${UCD_VERSION}/ucd/UnicodeData.txt

############################# libucd ##########################################

src/case.cpp: tools/case.py tools/ucd.py \
	data/ucd/UnicodeData.txt
	tools/case.py ${UCD_ROOTDIR} ${UCD_VERSION} > $@

src/categories.cpp: tools/categories.py tools/ucd.py \
	data/ucd/UnicodeData.txt
	tools/categories.py ${UCD_ROOTDIR} ${UCD_VERSION} > $@

src/scripts.cpp: tools/scripts.py tools/ucd.py \
	data/language-subtag-registry \
	data/ucd/Scripts.txt
	tools/scripts.py ${UCD_ROOTDIR} ${UCD_VERSION} > $@

libucd_includedir = $(includedir)/ucd
libucd_include_HEADERS = \
	src/include/ucd/ucd.h

lib_LTLIBRARIES       += src/libucd.la
src_libucd_la_LDFLAGS  = -version-info $(LIBUCD_VERSION)
src_libucd_la_CXXFLAGS = ${AM_CXXFLAGS}
src_libucd_la_SOURCES  = \
	src/case.cpp \
	src/categories.cpp \
	src/ctype.cpp \
	src/scripts.cpp

############################# tests ###########################################

noinst_bin_PROGRAMS       += tests/printucddata
tests_printucddata_SOURCES = tests/printucddata.cpp
tests_printucddata_LDADD   = src/libucd.la

tests/unicode-data.expected: tools/printdata.py tools/ucd.py \
	data/ucd/UnicodeData.txt \
	data/ucd/PropList.txt
	tools/printdata.py ${UCD_ROOTDIR} > $@

tests/unicode-data.actual: tests/printucddata
	tests/printucddata > $@

tests/unicode-data.diff: tests/unicode-data.expected tests/unicode-data.actual
	diff -U0 tests/unicode-data.expected tests/unicode-data.actual > tests/unicode-data.diff

check: tests/unicode-data.diff
