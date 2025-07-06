AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = -I m4

PREFIX=@prefix@
BINDIR=@bindir@
INCDIR=@includedir@/espeak
LIBDIR=@libdir@
DATADIR=$(PREFIX)/share/espeak-ng-data
VIMDIR=$(PREFIX)/share/vim

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = espeak-ng.pc

SHARED_VERSION=2:51:1 # = *.so.1.1.51

MKDIR=mkdir -p

AM_CFLAGS = \
	-I$(srcdir)/src/include -Isrc/include/compat -I$(srcdir)/src/speechPlayer/include -I$(srcdir)/src/ucd-tools/src/include \
	-D_BSD_SOURCE -D_DEFAULT_SOURCE -D_POSIX_C_SOURCE=200112L
AM_CXXFLAGS =

if USE_COVERAGE
AM_CFLAGS += -fprofile-instr-generate -fcoverage-mapping
AM_CXXFLAGS += -fprofile-instr-generate -fcoverage-mapping
endif

EXTRA_DIST=
CLEANFILES = dictsource/ru_listx dictsource/cmn_listx dictsource/yue_listx

bin_PROGRAMS =
lib_LTLIBRARIES =
man1_MANS =

check_PROGRAMS =
check_LTLIBRARIES =

##### ChangeLog:

ChangeLog:
	git log > ChangeLog

dist-hook: ChangeLog

.PHONY: ChangeLog

EXTRA_DIST += ChangeLog

##### standard build actions:

all-local: \
	espeak-ng-data/phontab \
	dictionaries

uninstall-hook:
	rm -rf $(DESTDIR)$(DATADIR)

install-data-hook: uninstall-hook
	$(MKDIR) $(DESTDIR)$(DATADIR)
	cp -prf espeak-ng-data/* $(DESTDIR)$(DATADIR)

install-exec-hook:
	cd $(DESTDIR)$(BINDIR) && rm -f espeak && $(LN_S) espeak-ng espeak
	cd $(DESTDIR)$(BINDIR) && rm -f speak  && $(LN_S) speak-ng speak
	cd $(DESTDIR)$(LIBDIR) && rm -f libespeak.la && $(LN_S) libespeak-ng.la libespeak.la

clean-local:
	cd src && rm -f *.o *~ && cd ..
	cd src/libespeak-ng/ && rm -f *.o *.lo && cd ../..
	cd tests && rm -f *.o *.lo && cd ..
if HAVE_GRADLE
	cd android && $(GRADLE) clean
endif

distclean-local:
	rm -f espeak-ng-data/*_dict
	rm -f espeak-ng-data/intonations
	rm -f espeak-ng-data/phondata
	rm -f espeak-ng-data/phondata-manifest
	rm -f espeak-ng-data/phonindex
	rm -f espeak-ng-data/phontab

##### custom rules:

SUFFIXES=.html .md .ronn .check .test

.md.html: _layouts/webpage.html
	@echo "  MD        $@"
	@cat $< | sed -e 's/\.md)/.html)/g' -e 's/\.ronn/.html/g' | \
		$(KRAMDOWN) --template _layouts/webpage.html > $@

.ronn.html:
	$(RONN) --html $<

##### vim:

vim_registrydir = $(VIMDIR)/registry
vim_registry_DATA = $(srcdir)/vim/registry/*.yaml
EXTRA_DIST += $(vim_registry_DATA)

vim_addons_ftdetectdir = $(VIMDIR)/addons/ftdetect
vim_addons_ftdetect_DATA = $(srcdir)/vim/ftdetect/*.vim
EXTRA_DIST += $(vim_addons_ftdetect_DATA)

vim_addons_syntaxdir = $(VIMDIR)/addons/syntax
vim_addons_syntax_DATA = $(srcdir)/vim/syntax/*.vim
EXTRA_DIST += $(vim_addons_syntax_DATA)

##### documentation:

src/espeak-ng.1: src/espeak-ng.1.ronn
	$(RONN) --roff -o $(shell dirname $@) $<

src/speak-ng.1: src/speak-ng.1.ronn
	$(RONN) --roff -o $(shell dirname $@) $<

docs_MARKDOWN = $(wildcard docs/*.md) $(wildcard docs/*/*.md) $(wildcard docs/*/*/*.md)
docs_HTML = ${docs_MARKDOWN:.md=.html}

docs:	$(docs_HTML) \
	src/espeak-ng.1.html \
	src/speak-ng.1.html \
	ChangeLog.html \
	README.html

EXTRA_DIST += src/espeak-ng.1.ronn src/speak-ng.1.ronn $(docs_MARKDOWN)

##### build targets:

espeak_includedir = $(includedir)/espeak
espeak_include_HEADERS = \
	src/include/espeak/speak_lib.h

espeak_ng_includedir = $(includedir)/espeak-ng
espeak_ng_include_HEADERS = \
	src/include/espeak-ng/encoding.h \
	src/include/espeak-ng/espeak_ng.h \
	src/include/espeak-ng/speak_lib.h

lib_LTLIBRARIES += src/libespeak-ng.la

src_libespeak_ng_la_LDFLAGS = -version-info $(SHARED_VERSION) -lpthread -lm \
	${PCAUDIOLIB_LIBS}

src_libespeak_ng_la_CFLAGS = \
	-fPIC -fvisibility=hidden \
	-pedantic -fno-exceptions -DPATH_ESPEAK_DATA=\"$(DATADIR)\" -DLIBESPEAK_NG_EXPORT \
	${PCAUDIOLIB_CFLAGS} ${SONIC_CFLAGS} ${AM_CFLAGS}

src_libespeak_ng_la_SOURCES = \
	src/ucd-tools/src/case.c \
	src/ucd-tools/src/categories.c \
	src/ucd-tools/src/ctype.c \
	src/ucd-tools/src/proplist.c \
	src/ucd-tools/src/scripts.c \
	src/ucd-tools/src/tostring.c \
	src/libespeak-ng/common.c \
	src/libespeak-ng/compiledata.c \
	src/libespeak-ng/compiledict.c \
	src/libespeak-ng/dictionary.c \
	src/libespeak-ng/encoding.c \
	src/libespeak-ng/error.c \
	src/libespeak-ng/espeak_api.c \
	src/libespeak-ng/ieee80.c \
	src/libespeak-ng/intonation.c \
	src/libespeak-ng/langopts.c \
	src/libespeak-ng/mnemonics.c \
	src/libespeak-ng/numbers.c \
	src/libespeak-ng/readclause.c \
	src/libespeak-ng/phoneme.c \
	src/libespeak-ng/phonemelist.c \
	src/libespeak-ng/setlengths.c \
	src/libespeak-ng/soundicon.c \
	src/libespeak-ng/spect.c \
	src/libespeak-ng/speech.c \
	src/libespeak-ng/ssml.c \
	src/libespeak-ng/synthdata.c \
	src/libespeak-ng/synthesize.c \
	src/libespeak-ng/translate.c \
	src/libespeak-ng/translateword.c \
	src/libespeak-ng/tr_languages.c \
	src/libespeak-ng/voices.c \
	src/libespeak-ng/wavegen.c

noinst_HEADERS = \
	src/libespeak-ng/common.h \
	src/libespeak-ng/compiledict.h \
	src/libespeak-ng/dictionary.h \
	src/libespeak-ng/error.h \
	src/libespeak-ng/espeak_command.h \
	src/libespeak-ng/event.h \
	src/libespeak-ng/fifo.h \
	src/libespeak-ng/ieee80.h \
	src/libespeak-ng/intonation.h \
	src/libespeak-ng/klatt.h \
	src/libespeak-ng/langopts.h \
	src/libespeak-ng/mbrola.h \
	src/libespeak-ng/mbrowrap.h \
	src/libespeak-ng/mnemonics.h \
	src/libespeak-ng/numbers.h \
	src/libespeak-ng/phoneme.h \
	src/libespeak-ng/phonemelist.h \
	src/libespeak-ng/readclause.h \
	src/libespeak-ng/setlengths.h \
	src/libespeak-ng/sintab.h \
	src/libespeak-ng/soundicon.h \
	src/libespeak-ng/spect.h \
	src/libespeak-ng/speech.h \
	src/libespeak-ng/sPlayer.h \
	src/libespeak-ng/ssml.h \
	src/libespeak-ng/synthdata.h \
	src/libespeak-ng/synthesize.h \
	src/libespeak-ng/translate.h \
	src/libespeak-ng/translateword.h \
	src/libespeak-ng/voice.h \
	src/libespeak-ng/wavegen.h \
	src/speechPlayer/include/speechPlayer.h \
	src/speechPlayer/src/frame.h \
	src/speechPlayer/src/sample.h \
	src/speechPlayer/src/speechPlayer.h \
	src/speechPlayer/src/speechWaveGenerator.h \
	src/speechPlayer/src/utils.h \
	src/speechPlayer/src/waveGenerator.h \
	src/ucd-tools/src/include/ucd/ucd.h

if OPT_KLATT
AM_CFLAGS  += -DUSE_KLATT=1
src_libespeak_ng_la_SOURCES += src/libespeak-ng/klatt.c
endif

if OPT_SPEECHPLAYER
AM_CFLAGS  += -DUSE_SPEECHPLAYER=1
src_libespeak_ng_la_SOURCES += src/libespeak-ng/sPlayer.c
src_libespeak_ng_la_SOURCES += src/speechPlayer/src/frame.cpp
src_libespeak_ng_la_SOURCES += src/speechPlayer/src/speechPlayer.cpp
src_libespeak_ng_la_SOURCES += src/speechPlayer/src/speechWaveGenerator.cpp
src_speak_ng_SOURCES = src/speak-ng.cpp
else
src_speak_ng_SOURCES = src/speak-ng.c
endif

if OPT_MBROLA
AM_CFLAGS  += -DUSE_MBROLA=1
src_libespeak_ng_la_SOURCES += \
	src/libespeak-ng/mbrowrap.c \
	src/libespeak-ng/synth_mbrola.c \
	src/libespeak-ng/compilembrola.c
endif

if OPT_ASYNC
AM_CFLAGS  += -DUSE_ASYNC=1
src_libespeak_ng_la_SOURCES += \
	src/libespeak-ng/espeak_command.c \
	src/libespeak-ng/event.c \
	src/libespeak-ng/fifo.c
endif

bin_PROGRAMS += src/speak-ng

if HAVE_RONN
man1_MANS += src/speak-ng.1
endif

src_speak_ng_LDADD   = src/libespeak-ng.la
src_speak_ng_LDFLAGS = -static -lm ${PCAUDIOLIB_LIBS} ${LIBFUZZER_NO_MAIN}
src_speak_ng_CFLAGS  = -Isrc/libespeak-ng ${AM_CFLAGS}

bin_PROGRAMS += src/espeak-ng

if HAVE_RONN
man1_MANS += src/espeak-ng.1
endif

src_espeak_ng_LDADD   = src/libespeak-ng.la ${PCAUDIOLIB_LIBS} ${LIBFUZZER_NO_MAIN}
src_espeak_ng_SOURCES = src/espeak-ng.c
if HAVE_LIBFUZZER
nodist_EXTRA_src_espeak_ng_SOURCES = force-cxx-linking.cxx
endif

##### tests:

# Test version of libespeak-ng.so with access to the internal APIs, so they can
# be accessed in the test code. This version should not be installed, as the
# internal APIs are not guaranteed to be stable between releases.
check_LTLIBRARIES += src/libespeak-ng-test.la

src_libespeak_ng_test_la_LDFLAGS = $(src_libespeak_ng_la_LDFLAGS)
src_libespeak_ng_test_la_CFLAGS  = \
	-fPIC -pedantic -fno-exceptions -DPATH_ESPEAK_DATA=\"$(DATADIR)\" -DLIBESPEAK_NG_EXPORT \
	${PCAUDIOLIB_CFLAGS} ${AM_CFLAGS}
src_libespeak_ng_test_la_SOURCES = $(src_libespeak_ng_la_SOURCES)

check_PROGRAMS += tests/encoding.test

tests_encoding_test_LDADD   = src/libespeak-ng.la
tests_encoding_test_SOURCES = tests/encoding.c
tests/encoding.check: LOG_COMPILER=$(VALGRIND)

check_PROGRAMS += tests/readclause.test

tests_readclause_test_CFLAGS  = -Isrc/libespeak-ng ${AM_CFLAGS}
tests_readclause_test_LDADD   = src/libespeak-ng-test.la
tests_readclause_test_SOURCES = tests/readclause.c
tests/readclause.check: LOG_COMPILER=$(VALGRIND)

check_PROGRAMS += tests/api.test

tests_api_test_CFLAGS  = -Isrc/libespeak-ng ${AM_CFLAGS}
tests_api_test_LDADD   = src/libespeak-ng-test.la
tests_api_test_SOURCES = tests/api.c
tests/api.check: LOG_COMPILER=$(VALGRIND)

check_PROGRAMS += tests/ieee80.test

tests_ieee80_test_CFLAGS  = -Isrc/libespeak-ng ${AM_CFLAGS}
tests_ieee80_test_LDADD   = src/libespeak-ng-test.la
tests_ieee80_test_SOURCES = tests/ieee80.c
tests/ieee80.check: LOG_COMPILER=$(VALGRIND)

.test.check:
	@echo "  TEST      $<"
	@ESPEAK_DATA_PATH=$(CURDIR) $(LOG_COMPILER) $< && echo "  PASSED    $<"

TEST = $(filter-out tests/windows-data.test,$(wildcard tests/*.test)) \
       $(check_PROGRAMS)

check:	$(TEST:.test=.check)

EXTRA_DIST += $(TEST) tests/common src/windows/installer/Product.wxs

##### fuzzer:

if !HAVE_LIBFUZZER
# libfuzzrunner is a stub implementation of libFuzzer that calls the
# LLVMFuzzerTestOneInput with the files passed to the fuzzer program.
check_LTLIBRARIES += tests/libfuzzrunner.la
tests_libfuzzrunner_la_CFLAGS  = -Isrc/libespeak-ng ${AM_CFLAGS}
tests_libfuzzrunner_la_SOURCES = tests/fuzzrunner.c
endif



check_PROGRAMS += tests/ssml-fuzzer.test
tests_ssml_fuzzer_test_CFLAGS  = ${AM_CFLAGS}
tests_ssml_fuzzer_test_SOURCES = tests/ssml-fuzzer.c
tests_ssml_fuzzer_test_LDADD   = src/libespeak-ng.la

if OPT_SPEECHPLAYER
tests_api_test_SOURCES += tests/dummy.cpp
tests_encoding_test_SOURCES += tests/dummy.cpp
tests_readclause_test_SOURCES += tests/dummy.cpp
tests_ssml_fuzzer_test_SOURCES += tests/dummy.cpp
endif


if HAVE_LIBFUZZER
noinst_PROGRAMS =  tests/fuzzing/synth_fuzzer.test
tests_fuzzing_synth_fuzzer_test_SOURCES = tests/fuzzing/synth_fuzzer.c
tests_fuzzing_synth_fuzzer_test_LDADD =  src/libespeak-ng.la
tests_fuzzing_synth_fuzzer_test_CFLAGS=  ${AM_CFLAGS} -DPATH_ESPEAK_DATA=\"$(abs_top_srcdir)/espeak-ng-data\" -Isrc/libespeak-ng
tests_fuzzing_synth_fuzzer_test_LDFLAGS= ${AM_LDFLAGS} -static -lm -Wl,-z,relro ${PCAUDIOLIB_LIBS} ${LIBFUZZER}
tests_ssml_fuzzer_test_LDFLAGS = ${AM_LDFLAGS} ${LIBFUZZER}
else
tests_ssml_fuzzer_test_LDADD += tests/libfuzzrunner.la
endif

FUZZ_SSML = $(wildcard tests/ssml/*.ssml tests/ssml-fuzzer/*.ssml)

tests/ssml-fuzzer.check: tests/ssml-fuzzer.test
	@echo "  TEST      $<"
	@ESPEAK_DATA_PATH=$(CURDIR) $< $(FUZZ_SSML) && echo "  PASSED    $<"


EXTRA_DIST += $(FUZZ_SSML) $(wildcard tests/ssml/*.expected)

##### phoneme data:

espeak-ng-data/phondata: phsource/phonemes.stamp
espeak-ng-data/phondata-manifest: phsource/phonemes.stamp
espeak-ng-data/phonindex: phsource/phonemes.stamp
espeak-ng-data/phontab: phsource/phonemes.stamp
espeak-ng-data/intonations: phsource/phonemes.stamp

PHSOURCE = \
	$(wildcard phsource/*) \
	$(wildcard phsource/*/*) \
	$(wildcard phsource/*/*/*)

LANGDATA = \
	$(wildcard espeak-ng-data/lang/*) \
	$(wildcard espeak-ng-data/lang/*/*) \
	$(wildcard espeak-ng-data/voices/*/*)

EXTRA_DIST += $(filter-out phsource/phonemes.stamp,$(PHSOURCE)) \
	      $(filter-out espeak-ng-data/voices/!v/Mr serious,$(LANGDATA))

phsource/phonemes.stamp: \
	$(PHSOURCE) \
	src/espeak-ng
	ESPEAK_DATA_PATH=$(CURDIR) src/espeak-ng --compile-intonations && \
		ESPEAK_DATA_PATH=$(CURDIR) src/espeak-ng --compile-phonemes && \
		touch $@

##### android targets:

apk-release: espeakdata
	cd android && $(GRADLE) assembleRelease

apk-debug: espeakdata
	cd android && $(GRADLE) assembleDebug

apk-check: espeakdata
	cd android && $(GRADLE) connectedCheck

espeakdata: espeak-ng-data/phontab dictionaries

##### dictionaries:

dictsource/%_emoji:
	@echo "  EMOJI     $@"
	@if test x"${CLDR_PATH}" = x ; then \
		touch $@ ; \
	else \
		tools/emoji \
			dictsource/en_emoji \
			`echo $@ | sed -e 's,dictsource/,,g' -e 's,_emoji,,g'` \
			${CLDR_PATH} > $@ ; \
	fi

dictsource/%_extra:
	touch $@

# NOTE: % (pattern) rules are GNU make specific we can extract the 'stem' that
# matched the % in the target ($@) using $(*F) (which isn't GNU make specific,
# just this interpretation is.)
#
# The rm $@ stops espeak-ng trying to load the old dictionary when it is run
# (at the cost of a spurious error message.)
#
# NOTE: keep in sync with 'espeak-ng-data/yue_dict' more specific rule.
espeak-ng-data/%_dict: src/espeak-ng phsource/phonemes.stamp
	@echo "  DICT      $@"
	rm -f $@
	cd dictsource && ESPEAK_DATA_PATH=$(CURDIR) ../src/espeak-ng --compile="$(*F)"

dictionaries: \
	espeak-ng-data/af_dict \
	espeak-ng-data/am_dict \
	espeak-ng-data/an_dict \
	espeak-ng-data/ar_dict \
	espeak-ng-data/as_dict \
	espeak-ng-data/az_dict \
	espeak-ng-data/ba_dict \
	espeak-ng-data/be_dict \
	espeak-ng-data/bg_dict \
	espeak-ng-data/bn_dict \
	espeak-ng-data/bpy_dict \
	espeak-ng-data/bs_dict \
	espeak-ng-data/ca_dict \
	espeak-ng-data/chr_dict \
	espeak-ng-data/cmn_dict \
        espeak-ng-data/crh_dict \
	espeak-ng-data/cs_dict \
	espeak-ng-data/cv_dict \
	espeak-ng-data/cy_dict \
	espeak-ng-data/da_dict \
	espeak-ng-data/de_dict \
	espeak-ng-data/el_dict \
	espeak-ng-data/en_dict \
	espeak-ng-data/eo_dict \
	espeak-ng-data/es_dict \
	espeak-ng-data/et_dict \
	espeak-ng-data/eu_dict \
	espeak-ng-data/fa_dict \
	espeak-ng-data/fi_dict \
	espeak-ng-data/fo_dict \
	espeak-ng-data/fr_dict \
	espeak-ng-data/ga_dict \
	espeak-ng-data/gd_dict \
	espeak-ng-data/gn_dict \
	espeak-ng-data/grc_dict \
	espeak-ng-data/gu_dict \
	espeak-ng-data/hak_dict \
	espeak-ng-data/haw_dict \
	espeak-ng-data/he_dict \
	espeak-ng-data/hi_dict \
	espeak-ng-data/hr_dict \
	espeak-ng-data/ht_dict \
	espeak-ng-data/hu_dict \
	espeak-ng-data/hy_dict \
	espeak-ng-data/ia_dict \
	espeak-ng-data/id_dict \
	espeak-ng-data/io_dict \
	espeak-ng-data/is_dict \
	espeak-ng-data/it_dict \
	espeak-ng-data/ja_dict \
	espeak-ng-data/jbo_dict \
	espeak-ng-data/ka_dict \
	espeak-ng-data/kaa_dict \
	espeak-ng-data/kk_dict \
	espeak-ng-data/kl_dict \
	espeak-ng-data/kn_dict \
	espeak-ng-data/ko_dict \
	espeak-ng-data/kok_dict \
	espeak-ng-data/ku_dict \
	espeak-ng-data/ky_dict \
	espeak-ng-data/la_dict \
	espeak-ng-data/lb_dict \
	espeak-ng-data/lfn_dict \
	espeak-ng-data/lt_dict \
	espeak-ng-data/lv_dict \
	espeak-ng-data/mi_dict \
	espeak-ng-data/mk_dict \
	espeak-ng-data/ml_dict \
	espeak-ng-data/mn_dict \
	espeak-ng-data/mr_dict \
	espeak-ng-data/ms_dict \
	espeak-ng-data/mt_dict \
	espeak-ng-data/mto_dict \
	espeak-ng-data/my_dict \
	espeak-ng-data/nci_dict \
	espeak-ng-data/ne_dict \
	espeak-ng-data/nl_dict \
	espeak-ng-data/no_dict \
	espeak-ng-data/nog_dict \
	espeak-ng-data/om_dict \
	espeak-ng-data/or_dict \
	espeak-ng-data/pa_dict \
	espeak-ng-data/pap_dict \
	espeak-ng-data/piqd_dict \
	espeak-ng-data/pl_dict \
	espeak-ng-data/ps_dict \
	espeak-ng-data/pt_dict \
	espeak-ng-data/py_dict \
	espeak-ng-data/qdb_dict \
	espeak-ng-data/qu_dict \
	espeak-ng-data/quc_dict \
	espeak-ng-data/qya_dict \
	espeak-ng-data/ro_dict \
	espeak-ng-data/ru_dict \
        espeak-ng-data/rup_dict \
	espeak-ng-data/sd_dict \
	espeak-ng-data/shn_dict \
	espeak-ng-data/si_dict \
	espeak-ng-data/sjn_dict \
	espeak-ng-data/sk_dict \
	espeak-ng-data/sl_dict \
	espeak-ng-data/smj_dict \
	espeak-ng-data/sq_dict \
	espeak-ng-data/sr_dict \
	espeak-ng-data/sv_dict \
	espeak-ng-data/sw_dict \
	espeak-ng-data/ta_dict \
	espeak-ng-data/te_dict \
	espeak-ng-data/th_dict \
	espeak-ng-data/ti_dict \
	espeak-ng-data/tk_dict \
	espeak-ng-data/tn_dict \
	espeak-ng-data/tr_dict \
	espeak-ng-data/tt_dict \
	espeak-ng-data/uk_dict \
	espeak-ng-data/ug_dict \
	espeak-ng-data/ur_dict \
	espeak-ng-data/uz_dict \
	espeak-ng-data/vi_dict \
	espeak-ng-data/xex_dict \
	espeak-ng-data/yue_dict

DICTSOURCE = \
	$(wildcard dictsource/*_emoji) \
	$(wildcard dictsource/*_extra) \
	$(wildcard dictsource/*_list) \
	$(wildcard dictsource/*_rules) \
	$(wildcard dictsource/extra/*) \
	dictsource/ar_listx \
	dictsource/bg_listx \
	dictsource/he_listx \
	dictsource/ia_listx \
	dictsource/it_listx \
	dictsource/tk_listx \
	dictsource/tr_listx

EXTRA_DIST += $(DICTSOURCE)

af: espeak-ng-data/af_dict
espeak-ng-data/af_dict: dictsource/af_list dictsource/af_rules dictsource/af_extra dictsource/af_emoji

am: espeak-ng-data/am_dict
espeak-ng-data/am_dict: dictsource/am_list dictsource/am_rules dictsource/am_extra dictsource/am_emoji

an: espeak-ng-data/an_dict
espeak-ng-data/an_dict: dictsource/an_list dictsource/an_rules dictsource/an_extra

ar: espeak-ng-data/ar_dict
espeak-ng-data/ar_dict: dictsource/ar_list dictsource/ar_listx dictsource/ar_rules dictsource/ar_extra dictsource/ar_emoji

as: espeak-ng-data/as_dict
espeak-ng-data/as_dict: dictsource/as_list dictsource/as_rules dictsource/as_extra

az: espeak-ng-data/az_dict
espeak-ng-data/az_dict: dictsource/az_list dictsource/az_rules dictsource/az_extra dictsource/az_emoji

ba: espeak-ng-data/ba_dict
espeak-ng-data/ba_dict: dictsource/ba_list dictsource/ba_rules

be: espeak-ng-data/be_dict
espeak-ng-data/be_dict: dictsource/be_list dictsource/be_rules

bg: espeak-ng-data/bg_dict
espeak-ng-data/bg_dict: dictsource/bg_list dictsource/bg_listx dictsource/bg_rules dictsource/bg_extra dictsource/bg_emoji

bn: espeak-ng-data/bn_dict
espeak-ng-data/bn_dict: dictsource/bn_list dictsource/bn_rules dictsource/bn_extra dictsource/bn_emoji

bpy: espeak-ng-data/bpy_dict
espeak-ng-data/bpy_dict: dictsource/bpy_list dictsource/bpy_rules dictsource/bpy_extra

bs: espeak-ng-data/bs_dict
espeak-ng-data/bs_dict: dictsource/bs_list dictsource/bs_rules dictsource/bs_extra dictsource/bs_emoji

ca: espeak-ng-data/ca_dict
espeak-ng-data/ca_dict: dictsource/ca_list dictsource/ca_rules dictsource/ca_extra dictsource/ca_emoji

chr: espeak-ng-data/chr_dict
espeak-ng-data/chr_dict: dictsource/chr_list dictsource/chr_rules

crh: espeak-ng-data/crh_dict
espeak-ng-data/crh_dict: dictsource/crh_list dictsource/crh_rules dictsource/crh_extra dictsource/crh_emoji

cs: espeak-ng-data/cs_dict
espeak-ng-data/cs_dict: dictsource/cs_list dictsource/cs_rules dictsource/cs_extra dictsource/cs_emoji

cv: espeak-ng-data/cv_dict
espeak-ng-data/cv_dict: dictsource/cv_list dictsource/cv_rules dictsource/cv_extra

cy: espeak-ng-data/cy_dict
espeak-ng-data/cy_dict: dictsource/cy_list dictsource/cy_rules dictsource/cy_extra dictsource/cy_emoji

da: espeak-ng-data/da_dict
espeak-ng-data/da_dict: dictsource/da_list dictsource/da_rules dictsource/da_extra dictsource/da_emoji

de: espeak-ng-data/de_dict
espeak-ng-data/de_dict: dictsource/de_list dictsource/de_rules dictsource/de_extra dictsource/de_emoji

el: espeak-ng-data/el_dict
espeak-ng-data/el_dict: dictsource/el_list dictsource/el_rules dictsource/el_extra dictsource/el_emoji

en: espeak-ng-data/en_dict
espeak-ng-data/en_dict: dictsource/en_list dictsource/en_rules dictsource/en_extra dictsource/en_emoji

eo: espeak-ng-data/eo_dict
espeak-ng-data/eo_dict: dictsource/eo_list dictsource/eo_rules dictsource/eo_extra

es: espeak-ng-data/es_dict
espeak-ng-data/es_dict: dictsource/es_list dictsource/es_rules dictsource/es_extra dictsource/es_emoji

et: espeak-ng-data/et_dict
espeak-ng-data/et_dict: dictsource/et_list dictsource/et_rules dictsource/et_extra dictsource/et_emoji

eu: espeak-ng-data/eu_dict
espeak-ng-data/eu_dict: dictsource/eu_list dictsource/eu_rules dictsource/eu_extra dictsource/eu_emoji

fa: espeak-ng-data/fa_dict
espeak-ng-data/fa_dict: dictsource/fa_list dictsource/fa_rules dictsource/fa_extra dictsource/fa_emoji

fi: espeak-ng-data/fi_dict
espeak-ng-data/fi_dict: dictsource/fi_list dictsource/fi_rules dictsource/fi_extra dictsource/fi_emoji

fo: espeak-ng-data/fo_dict
espeak-ng-data/fo_dict: dictsource/fo_list dictsource/fo_rules dictsource/fo_extra dictsource/fo_emoji

fr: espeak-ng-data/fr_dict
espeak-ng-data/fr_dict: dictsource/fr_list dictsource/fr_rules dictsource/fr_extra dictsource/fr_emoji

ga: espeak-ng-data/ga_dict
espeak-ng-data/ga_dict: dictsource/ga_list dictsource/ga_rules dictsource/ga_extra dictsource/ga_emoji

gd: espeak-ng-data/gd_dict
espeak-ng-data/gd_dict: dictsource/gd_list dictsource/gd_rules dictsource/gd_extra dictsource/gd_emoji

gn: espeak-ng-data/gn_dict
espeak-ng-data/gn_dict: dictsource/gn_list dictsource/gn_rules dictsource/gn_extra

grc: espeak-ng-data/grc_dict
espeak-ng-data/grc_dict: dictsource/grc_list dictsource/grc_rules dictsource/grc_extra

gu: espeak-ng-data/gu_dict
espeak-ng-data/gu_dict: dictsource/gu_list dictsource/gu_rules dictsource/gu_extra dictsource/gu_emoji

hak: espeak-ng-data/hak_dict
espeak-ng-data/hak_dict: dictsource/hak_list dictsource/hak_rules dictsource/hak_extra

haw: espeak-ng-data/haw_dict
espeak-ng-data/haw_dict: dictsource/haw_list dictsource/haw_rules dictsource/haw_extra

he: espeak-ng-data/he_dict
espeak-ng-data/he_dict: dictsource/he_list dictsource/he_rules dictsource/he_extra

hi: espeak-ng-data/hi_dict
espeak-ng-data/hi_dict: dictsource/hi_list dictsource/hi_rules dictsource/hi_extra dictsource/hi_emoji

hr: espeak-ng-data/hr_dict
espeak-ng-data/hr_dict: dictsource/hr_list dictsource/hr_rules dictsource/hr_extra dictsource/hr_emoji

ht: espeak-ng-data/ht_dict
espeak-ng-data/ht_dict: dictsource/ht_list dictsource/ht_rules dictsource/ht_extra

hu: espeak-ng-data/hu_dict
espeak-ng-data/hu_dict: dictsource/hu_list dictsource/hu_rules dictsource/hu_extra dictsource/hu_emoji

hy: espeak-ng-data/hy_dict
espeak-ng-data/hy_dict: dictsource/hy_list dictsource/hy_rules dictsource/hy_extra dictsource/hy_emoji

ia: espeak-ng-data/ia_dict
espeak-ng-data/ia_dict: dictsource/ia_list dictsource/ia_listx dictsource/ia_rules dictsource/ia_extra

id: espeak-ng-data/id_dict
espeak-ng-data/id_dict: dictsource/id_list dictsource/id_rules dictsource/id_extra dictsource/id_emoji

io: espeak-ng-data/io_dict
espeak-ng-data/io_dict: dictsource/io_list dictsource/io_rules dictsource/io_extra

is: espeak-ng-data/is_dict
espeak-ng-data/is_dict: dictsource/is_list dictsource/is_rules dictsource/is_extra dictsource/is_emoji

it: espeak-ng-data/it_dict
espeak-ng-data/it_dict: dictsource/it_list dictsource/it_listx dictsource/it_rules dictsource/it_extra dictsource/it_emoji

ja: espeak-ng-data/ja_dict
espeak-ng-data/ja_dict: dictsource/ja_list dictsource/ja_rules dictsource/ja_extra dictsource/ja_emoji

jbo: espeak-ng-data/jbo_dict
espeak-ng-data/jbo_dict: dictsource/jbo_list dictsource/jbo_rules dictsource/jbo_extra

ka: espeak-ng-data/ka_dict
espeak-ng-data/ka_dict: dictsource/ka_list dictsource/ka_rules dictsource/ka_extra dictsource/ka_emoji

kaa: espeak-ng-data/kaa_dict
espeak-ng-data/kaa_dict: dictsource/kaa_list dictsource/kaa_rules dictsource/kaa_extra dictsource/kaa_emoji

kk: espeak-ng-data/kk_dict
espeak-ng-data/kk_dict: dictsource/kk_list dictsource/kk_rules dictsource/kk_extra dictsource/kk_emoji

kl: espeak-ng-data/kl_dict
espeak-ng-data/kl_dict: dictsource/kl_list dictsource/kl_rules dictsource/kl_extra

kn: espeak-ng-data/kn_dict
espeak-ng-data/kn_dict: dictsource/kn_list dictsource/kn_rules dictsource/kn_extra dictsource/kn_emoji

ko: espeak-ng-data/ko_dict
espeak-ng-data/ko_dict: dictsource/ko_list dictsource/ko_rules dictsource/ko_extra dictsource/ko_emoji

kok: espeak-ng-data/kok_dict
espeak-ng-data/kok_dict: dictsource/kok_list dictsource/kok_rules dictsource/kok_extra

ku: espeak-ng-data/ku_dict
espeak-ng-data/ku_dict: dictsource/ku_list dictsource/ku_rules dictsource/ku_extra

ky: espeak-ng-data/ky_dict
espeak-ng-data/ky_dict: dictsource/ky_list dictsource/ky_rules dictsource/ky_extra dictsource/ky_emoji

la: espeak-ng-data/la_dict
espeak-ng-data/la_dict: dictsource/la_list dictsource/la_rules dictsource/la_extra

lb: espeak-ng-data/lb_dict
espeak-ng-data/lb_dict: dictsource/lb_list dictsource/lb_rules dictsource/lb_emoji

lfn: espeak-ng-data/lfn_dict
espeak-ng-data/lfn_dict: dictsource/lfn_list dictsource/lfn_rules dictsource/lfn_extra

lt: espeak-ng-data/lt_dict
espeak-ng-data/lt_dict: dictsource/lt_list dictsource/lt_rules dictsource/lt_extra dictsource/lt_emoji

lv: espeak-ng-data/lv_dict
espeak-ng-data/lv_dict: dictsource/lv_list dictsource/lv_rules dictsource/lv_extra dictsource/lv_emoji

mi: espeak-ng-data/mi_dict
espeak-ng-data/mi_dict: dictsource/mi_list dictsource/mi_rules dictsource/mi_extra

mk: espeak-ng-data/mk_dict
espeak-ng-data/mk_dict: dictsource/mk_list dictsource/mk_rules dictsource/mk_extra dictsource/mk_emoji

ml: espeak-ng-data/ml_dict
espeak-ng-data/ml_dict: dictsource/ml_list dictsource/ml_rules dictsource/ml_extra dictsource/ml_emoji

mn: espeak-ng-data/mn_dict
espeak-ng-data/mn_dict: dictsource/mn_list dictsource/mn_rules dictsource/mn_extra dictsource/mn_emoji

mr: espeak-ng-data/mr_dict
espeak-ng-data/mr_dict: dictsource/mr_list dictsource/mr_rules dictsource/mr_extra dictsource/mr_emoji

ms: espeak-ng-data/ms_dict
espeak-ng-data/ms_dict: dictsource/ms_list dictsource/ms_rules dictsource/ms_extra dictsource/ms_emoji

mt: espeak-ng-data/mt_dict
espeak-ng-data/mt_dict: dictsource/mt_list dictsource/mt_rules dictsource/mt_extra

mto: espeak-ng-data/mto_dict
espeak-ng-data/mto_dict: dictsource/mto_list dictsource/mto_rules

my: espeak-ng-data/my_dict
espeak-ng-data/my_dict: dictsource/my_list dictsource/my_rules dictsource/my_extra dictsource/my_emoji

nci: espeak-ng-data/nci_dict
espeak-ng-data/nci_dict: dictsource/nci_list dictsource/nci_rules dictsource/nci_extra

ne: espeak-ng-data/ne_dict
espeak-ng-data/ne_dict: dictsource/ne_list dictsource/ne_rules dictsource/ne_extra dictsource/ne_emoji

nl: espeak-ng-data/nl_dict
espeak-ng-data/nl_dict: dictsource/nl_list dictsource/nl_rules dictsource/nl_extra dictsource/nl_emoji

no: espeak-ng-data/no_dict
espeak-ng-data/no_dict: dictsource/no_list dictsource/no_rules dictsource/no_extra

no: espeak-ng-data/nog_dict
espeak-ng-data/nog_dict: dictsource/nog_list dictsource/nog_rules dictsource/nog_extra

om: espeak-ng-data/om_dict
espeak-ng-data/om_dict: dictsource/om_list dictsource/om_rules dictsource/om_extra

or: espeak-ng-data/or_dict
espeak-ng-data/or_dict: dictsource/or_list dictsource/or_rules dictsource/or_extra dictsource/or_emoji

pa: espeak-ng-data/pa_dict
espeak-ng-data/pa_dict: dictsource/pa_list dictsource/pa_rules dictsource/pa_extra dictsource/pa_emoji

pap: espeak-ng-data/pap_dict
espeak-ng-data/pap_dict: dictsource/pap_list dictsource/pap_rules dictsource/pap_extra

piqd: espeak-ng-data/piqd_dict
espeak-ng-data/piqd_dict: dictsource/piqd_list dictsource/piqd_rules

pl: espeak-ng-data/pl_dict
espeak-ng-data/pl_dict: dictsource/pl_list dictsource/pl_rules dictsource/pl_extra dictsource/pl_emoji

ps: espeak-ng-data/ps_dict
espeak-ng-data/ps_dict: dictsource/ps_list dictsource/ps_rules dictsource/ps_extra dictsource/ps_emoji

pt: espeak-ng-data/pt_dict
espeak-ng-data/pt_dict: dictsource/pt_list dictsource/pt_rules dictsource/pt_extra dictsource/pt_emoji

py: espeak-ng-data/py_dict
espeak-ng-data/py_dict: dictsource/py_list dictsource/py_rules dictsource/py_extra

qdb: espeak-ng-data/qdb_dict
espeak-ng-data/qdb_dict: dictsource/qdb_list dictsource/qdb_rules

qu: espeak-ng-data/qu_dict
espeak-ng-data/qu_dict: dictsource/qu_list dictsource/qu_rules dictsource/qu_extra

quc: espeak-ng-data/quc_dict
espeak-ng-data/quc_dict: dictsource/quc_list dictsource/quc_rules dictsource/quc_extra dictsource/quc_emoji

qya: espeak-ng-data/qya_dict
espeak-ng-data/qya_dict: dictsource/qya_list dictsource/qya_rules

ro: espeak-ng-data/ro_dict
espeak-ng-data/ro_dict: dictsource/ro_list dictsource/ro_rules dictsource/ro_extra dictsource/ro_emoji

ru: espeak-ng-data/ru_dict
dictsource/ru_listx:
	ln -svf extra/ru_listx dictsource/
if HAVE_RU_EXTENDED_DICTIONARY
espeak-ng-data/ru_dict: dictsource/ru_list dictsource/ru_rules dictsource/ru_extra dictsource/ru_listx dictsource/ru_emoji
else
espeak-ng-data/ru_dict: dictsource/ru_list dictsource/ru_rules dictsource/ru_extra dictsource/ru_emoji
endif

rup: espeak-ng-data/rup_dict
espeak-ng-data/rup_dict: dictsource/rup_list dictsource/rup_rules dictsource/rup_extra

sd: espeak-ng-data/sd_dict
espeak-ng-data/sd_dict: dictsource/sd_list dictsource/sd_rules dictsource/sd_extra dictsource/sd_emoji

shn: espeak-ng-data/shn_dict
espeak-ng-data/shn_dict: dictsource/shn_list dictsource/shn_rules dictsource/shn_extra

si: espeak-ng-data/si_dict
espeak-ng-data/si_dict: dictsource/si_list dictsource/si_rules dictsource/si_extra dictsource/si_emoji

sjn: espeak-ng-data/sjn_dict
espeak-ng-data/sjn_dict: dictsource/sjn_list dictsource/sjn_rules

sk: espeak-ng-data/sk_dict
espeak-ng-data/sk_dict: dictsource/sk_list dictsource/sk_rules dictsource/sk_extra dictsource/sk_emoji

sl: espeak-ng-data/sl_dict
espeak-ng-data/sl_dict: dictsource/sl_list dictsource/sl_rules dictsource/sl_extra dictsource/sl_emoji

smj: espeak-ng-data/smj_dict
espeak-ng-data/smj_dict: dictsource/smj_list dictsource/smj_rules

sq: espeak-ng-data/sq_dict
espeak-ng-data/sq_dict: dictsource/sq_list dictsource/sq_rules dictsource/sq_extra dictsource/sq_emoji

dictsource/sr_emoji:
	@echo "  EMOJI     $@"
	@if test x"${CLDR_PATH}" = x ; then \
		touch $@ ; \
	else \
		tools/emoji dictsource/en_emoji sr_Latn ${CLDR_PATH} > $@ ; \
	fi

sr: espeak-ng-data/sr_dict
espeak-ng-data/sr_dict: dictsource/sr_list dictsource/sr_rules dictsource/sr_extra dictsource/sr_emoji

sv: espeak-ng-data/sv_dict
espeak-ng-data/sv_dict: dictsource/sv_list dictsource/sv_rules dictsource/sv_extra dictsource/sv_emoji

sw: espeak-ng-data/sw_dict
espeak-ng-data/sw_dict: dictsource/sw_list dictsource/sw_rules dictsource/sw_extra dictsource/sw_emoji

ta: espeak-ng-data/ta_dict
espeak-ng-data/ta_dict: dictsource/ta_list dictsource/ta_rules dictsource/ta_extra dictsource/ta_emoji

te: espeak-ng-data/te_dict
espeak-ng-data/te_dict: dictsource/te_list dictsource/te_rules dictsource/te_extra dictsource/te_emoji

ti: espeak-ng-data/ti_dict
espeak-ng-data/ti_dict: dictsource/ti_list dictsource/ti_rules dictsource/ti_extra dictsource/ti_emoji

th: espeak-ng-data/th_dict
espeak-ng-data/th_dict: dictsource/th_list dictsource/th_rules dictsource/th_extra

tk: espeak-ng-data/tk_dict
espeak-ng-data/tk_dict: dictsource/tk_list dictsource/tk_listx dictsource/tk_rules dictsource/tk_extra

tn: espeak-ng-data/tn_dict
espeak-ng-data/tn_dict: dictsource/tn_list dictsource/tn_rules dictsource/tn_extra

tr: espeak-ng-data/tr_dict
espeak-ng-data/tr_dict: dictsource/tr_list dictsource/tr_rules dictsource/tr_extra dictsource/tr_emoji

tt: espeak-ng-data/tt_dict
espeak-ng-data/tt_dict: dictsource/tt_list dictsource/tt_rules dictsource/tt_extra

ug: espeak-ng-data/ug_dict
espeak-ng-data/ug_dict: dictsource/ug_list dictsource/ug_rules dictsource/ug_extra

uk: espeak-ng-data/uk_dict
espeak-ng-data/uk_dict: dictsource/uk_list dictsource/uk_rules dictsource/uk_extra

ur: espeak-ng-data/ur_dict
espeak-ng-data/ur_dict: dictsource/ur_list dictsource/ur_rules dictsource/ur_extra dictsource/ur_emoji

uz: espeak-ng-data/uz_dict
espeak-ng-data/uz_dict: dictsource/uz_list dictsource/uz_rules dictsource/uz_extra

vi: espeak-ng-data/vi_dict
espeak-ng-data/vi_dict: dictsource/vi_list dictsource/vi_rules dictsource/vi_extra dictsource/vi_emoji

xex: espeak-ng-data/xex_dict
espeak-ng-data/xex_dict: dictsource/xex_list dictsource/xex_rules

cmn: espeak-ng-data/cmn_dict
dictsource/cmn_listx:
	ln -svf extra/cmn_listx dictsource/
if HAVE_CMN_EXTENDED_DICTIONARY
espeak-ng-data/cmn_dict: dictsource/cmn_list dictsource/cmn_rules dictsource/cmn_listx dictsource/cmn_extra dictsource/cmn_emoji
else
espeak-ng-data/cmn_dict: dictsource/cmn_list dictsource/cmn_rules dictsource/cmn_extra dictsource/cmn_emoji
endif

yue: espeak-ng-data/yue_dict
dictsource/yue_listx:
	ln -svf extra/yue_listx dictsource/

# NOTE: keep in sync with espeak-ng-data/%_dict more generic rule.
if HAVE_YUE_EXTENDED_DICTIONARY
espeak-ng-data/yue_dict: src/espeak-ng phsource/phonemes.stamp dictsource/yue_list dictsource/yue_rules dictsource/yue_extra dictsource/yue_listx dictsource/yue_emoji
else
espeak-ng-data/yue_dict: src/espeak-ng phsource/phonemes.stamp dictsource/yue_list dictsource/yue_rules dictsource/yue_extra dictsource/yue_emoji
endif
	@echo "  DICT      $@"
	rm -f $@
	cd dictsource && ESPEAK_DATA_PATH=$(CURDIR) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=yue && cd ..

dictsource/yue_emoji:
	@echo "  EMOJI     $@"
	@if test x"${CLDR_PATH}" = x ; then \
		touch $@ ; \
	else \
		tools/emoji dictsource/en_emoji yue ${CLDR_PATH} > $@ ; \
	fi

##### mbrola:

if OPT_MBROLA

PHSOURCE_MBROLA = \
	$(wildcard phsource/mbrola/*)

mbrola: \
	$(patsubst phsource/mbrola/%,espeak-ng-data/mbrola_ph/%_phtrans,$(PHSOURCE_MBROLA))

espeak-ng-data/mbrola_ph/%_phtrans: phsource/mbrola/% src/espeak-ng
	mkdir -p espeak-ng-data/mbrola_ph
	ESPEAK_DATA_PATH=$(CURDIR) src/espeak-ng --compile-mbrola=phsource/mbrola/$*

EXTRA_DIST += $(PHSOURCE_MBROLA)

all-local: mbrola

endif
