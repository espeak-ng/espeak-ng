AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = -I m4

PREFIX=@prefix@
BINDIR=@bindir@
INCDIR=@includedir@/espeak
LIBDIR=@libdir@
DATADIR=$(PREFIX)/share/espeak-data
VIMDIR=$(PREFIX)/share/vim

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = espeak-ng.pc

SHARED_VERSION=2:48:1 # = *.so.1.1.48

MKDIR=mkdir -p

AM_CFLAGS = -Wall

EXTRA_DIST=

bin_PROGRAMS =
lib_LTLIBRARIES =
man1_MANS =

##### ChangeLog:

ChangeLog:
	git log > ChangeLog

dist-hook: ChangeLog

.PHONY: ChangeLog

EXTRA_DIST += ChangeLog

##### standard build actions:

all-local: \
	espeak-data/phontab \
	dictionaries \
	mbrola

uninstall-hook:
	rm -rf $(DESTDIR)$(DATADIR)

install-data-hook: uninstall-hook
	$(MKDIR) $(DESTDIR)$(DATADIR)
	cp -prf espeak-data/* $(DESTDIR)$(DATADIR)

install-exec-hook:
	cd $(DESTDIR)$(BINDIR) && rm -f espeak && $(LN_S) espeak-ng espeak
	cd $(DESTDIR)$(BINDIR) && rm -f speak  && $(LN_S) speak-ng speak
	cd $(DESTDIR)$(LIBDIR) && rm -f libespeak.la && $(LN_S) libespeak-ng.la libespeak.la

clean-local:
	cd src && rm -f *.o *~ && cd ..

distclean-local:
	rm -rf espeak-data/phondata-manifest
	rm -f espeak-data/*_dict

##### custom rules:

SUFFIXES=.html .md .ronn

.md.html: _layouts/webpage.html
	cat $< | sed -e 's/\.md)/.html)/g' -e 's/\.ronn/.html/g' | \
		kramdown --template _layouts/webpage.html > $@

.ronn.html:
	ronn --html $<

##### vim:

vim_registrydir = $(VIMDIR)/registry
vim_registry_DATA = vim/registry/*.yaml
EXTRA_DIST += $(vim_registry_DATA)

vim_addons_ftdetectdir = $(VIMDIR)/addons/ftdetect
vim_addons_ftdetect_DATA = vim/ftdetect/*.vim
EXTRA_DIST += $(vim_addons_ftdetect_DATA)

vim_addons_syntaxdir = $(VIMDIR)/addons/syntax
vim_addons_syntax_DATA = vim/syntax/*.vim
EXTRA_DIST += $(vim_addons_syntax_DATA)

##### documentation:

src/espeak-ng.1: src/espeak-ng.1.ronn
	ronn --roff $<

src/speak-ng.1: src/speak-ng.1.ronn
	ronn --roff $<

docs:	docs/index.html \
	docs/add_language.html \
	docs/dictionary.html \
	docs/intonation.html \
	docs/markup.html \
	docs/mbrola.html \
	docs/phontab.html \
	docs/voices.html \
	src/espeak-ng.1.html \
	src/speak-ng.1.html \
	README.html

##### build targets:

espeak_includedir = $(includedir)/espeak
espeak_include_HEADERS = \
	src/include/espeak/speak_lib.h

espeak_ng_includedir = $(includedir)/espeak-ng
espeak_ng_include_HEADERS = \
	src/include/espeak-ng/espeak_ng.h

lib_LTLIBRARIES += src/libespeak-ng.la

src_libespeak_ng_la_LDFLAGS = -version-info $(SHARED_VERSION) -lpthread -lm \
	${PCAUDIOLIB_LIBS}

src_libespeak_ng_la_CFLAGS = -Isrc/include -Isrc/include/compat \
	-fPIC -fvisibility=hidden -D _BSD_SOURCE -D_DEFAULT_SOURCE -D _POSIX_C_SOURCE=200112L \
	-pedantic -fno-exceptions -D PATH_ESPEAK_DATA=\"$(DATADIR)\" -DLIBESPEAK_NG_EXPORT \
	${PCAUDIOLIB_CFLAGS}

src_libespeak_ng_la_SOURCES = \
	src/libespeak-ng/compiledata.c \
	src/libespeak-ng/compiledict.c \
	src/libespeak-ng/compilembrola.c \
	src/libespeak-ng/dictionary.c \
	src/libespeak-ng/error.c \
	src/libespeak-ng/espeak_api.c \
	src/libespeak-ng/ieee80.c \
	src/libespeak-ng/intonation.c \
	src/libespeak-ng/numbers.c \
	src/libespeak-ng/readclause.c \
	src/libespeak-ng/phonemelist.c \
	src/libespeak-ng/setlengths.c \
	src/libespeak-ng/spect.c \
	src/libespeak-ng/speech.c \
	src/libespeak-ng/synthdata.c \
	src/libespeak-ng/synthesize.c \
	src/libespeak-ng/synth_mbrola.c \
	src/libespeak-ng/translate.c \
	src/libespeak-ng/tr_languages.c \
	src/libespeak-ng/voices.c \
	src/libespeak-ng/wavegen.c

if OPT_KLATT
src_libespeak_ng_la_CFLAGS  += -DINCLUDE_KLATT
src_libespeak_ng_la_SOURCES += src/libespeak-ng/klatt.c
endif

if OPT_MBROLA
src_libespeak_ng_la_CFLAGS  += -DINCLUDE_MBROLA
src_libespeak_ng_la_SOURCES += src/libespeak-ng/mbrowrap.c
endif

if OPT_ASYNC
src_libespeak_ng_la_CFLAGS  += -DUSE_ASYNC
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
src_speak_ng_LDFLAGS = -static -lm ${PCAUDIOLIB_LIBS}
src_speak_ng_CFLAGS  = -Isrc/libespeak-ng -Isrc/include -D _POSIX_C_SOURCE=200112L
src_speak_ng_SOURCES = src/speak-ng.c

bin_PROGRAMS += src/espeak-ng

if HAVE_RONN
man1_MANS += src/espeak-ng.1
endif

src_espeak_ng_LDADD   = src/libespeak-ng.la ${PCAUDIOLIB_LIBS}
src_espeak_ng_CFLAGS  = -Isrc/include
src_espeak_ng_SOURCES = src/espeak-ng.c

##### phoneme data:

espeak-data/phondata: phsource/phonemes.stamp
espeak-data/phondata-manifest: phsource/phonemes.stamp
espeak-data/phonindex: phsource/phonemes.stamp
espeak-data/phontab: phsource/phonemes.stamp
espeak-data/intonations: phsource/phonemes.stamp

phsource/phonemes.stamp: \
	phsource/ph_afrikaans \
	phsource/ph_akan \
	phsource/ph_albanian \
	phsource/ph_amhari \
	phsource/ph_arabic \
	phsource/ph_aragon \
	phsource/ph_armenian \
	phsource/ph_assamese \
	phsource/ph_azerbaijani \
	phsource/ph_base2 \
	phsource/ph_basque \
	phsource/ph_bengali \
	phsource/ph_bulgarian \
	phsource/ph_burmese \
	phsource/ph_catalan \
	phsource/ph_consonants \
	phsource/ph_croatian \
	phsource/ph_czech \
	phsource/ph_danish \
	phsource/ph_dari \
	phsource/ph_divehi \
	phsource/ph_dutch \
	phsource/ph_english \
	phsource/ph_english_n \
	phsource/ph_english_rp \
	phsource/ph_english_sc \
	phsource/ph_english_us \
	phsource/ph_english_wi \
	phsource/ph_english_wm \
	phsource/ph_esperanto \
	phsource/ph_estonian \
	phsource/ph_farsi \
	phsource/ph_finnish \
	phsource/ph_french \
	phsource/ph_french_ca \
	phsource/ph_georgian \
	phsource/ph_german \
	phsource/ph_greek \
	phsource/ph_greek_ancient \
	phsource/ph_greenlandic \
	phsource/ph_gujarati \
	phsource/ph_haitian \
	phsource/ph_hindi \
	phsource/ph_hindi_base \
	phsource/ph_hungarian \
	phsource/ph_icelandic \
	phsource/ph_indonesian \
	phsource/ph_interlingua \
	phsource/ph_irish \
	phsource/ph_italian \
	phsource/ph_kannada \
	phsource/ph_kazakh \
	phsource/ph_kinyarwanda \
	phsource/ph_korean \
	phsource/ph_kurdish \
	phsource/ph_latin \
	phsource/ph_latvian \
	phsource/ph_lithuanian \
	phsource/ph_lojban \
	phsource/ph_macedonian \
	phsource/ph_malayalam \
	phsource/ph_maltese \
	phsource/ph_manipuri \
	phsource/ph_marathi \
	phsource/ph_nahuatl \
	phsource/ph_nepali \
	phsource/ph_northern-sotho \
	phsource/ph_norwegian \
	phsource/ph_oriya \
	phsource/ph_oromo \
	phsource/ph_polish \
	phsource/ph_portugal \
	phsource/ph_pt_brazil \
	phsource/ph_punjabi \
	phsource/ph_romanian \
	phsource/ph_russian \
	phsource/ph_s_gaelic \
	phsource/ph_serbian \
	phsource/ph_setswana \
	phsource/ph_sinhala \
	phsource/ph_slovak \
	phsource/ph_slovenian \
	phsource/ph_spanish \
	phsource/ph_spanish_la \
	phsource/ph_swahili \
	phsource/ph_swedish \
	phsource/ph_tamil \
	phsource/ph_tatar \
	phsource/ph_telugu \
	phsource/ph_tibetan \
	phsource/ph_turkish \
	phsource/ph_urdu \
	phsource/ph_vietnam \
	phsource/ph_vietnam_hue \
	phsource/ph_vietnam_sgn \
	phsource/ph_welsh \
	phsource/ph_wolof \
	phsource/ph_yoruba \
	phsource/ph_zh \
	phsource/ph_zh_yue \
	phsource/phonemes \
	src/espeak-ng
	ESPEAK_DATA_PATH=$(PWD) src/espeak-ng --compile-intonations && \
		ESPEAK_DATA_PATH=$(PWD) src/espeak-ng --compile-phonemes && \
		touch $@

##### dictionaries:

dictionaries: \
	espeak-data/af_dict \
	espeak-data/am_dict \
	espeak-data/an_dict \
	espeak-data/ar_dict \
	espeak-data/as_dict \
	espeak-data/az_dict \
	espeak-data/bg_dict \
	espeak-data/bn_dict \
	espeak-data/ca_dict \
	espeak-data/cs_dict \
	espeak-data/cy_dict \
	espeak-data/da_dict \
	espeak-data/de_dict \
	espeak-data/el_dict \
	espeak-data/en_dict \
	espeak-data/eo_dict \
	espeak-data/es_dict \
	espeak-data/et_dict \
	espeak-data/eu_dict \
	espeak-data/fa_dict \
	espeak-data/fi_dict \
	espeak-data/fr_dict \
	espeak-data/ga_dict \
	espeak-data/gd_dict \
	espeak-data/grc_dict \
	espeak-data/gu_dict \
	espeak-data/hbs_dict \
	espeak-data/hi_dict \
	espeak-data/hu_dict \
	espeak-data/hy_dict \
	espeak-data/ia_dict \
	espeak-data/id_dict \
	espeak-data/is_dict \
	espeak-data/it_dict \
	espeak-data/jbo_dict \
	espeak-data/ka_dict \
	espeak-data/kl_dict \
	espeak-data/kn_dict \
	espeak-data/ko_dict \
	espeak-data/ku_dict \
	espeak-data/la_dict \
	espeak-data/lfn_dict \
	espeak-data/lt_dict \
	espeak-data/lv_dict \
	espeak-data/mk_dict \
	espeak-data/ml_dict \
	espeak-data/mr_dict \
	espeak-data/ms_dict \
	espeak-data/mt_dict \
	espeak-data/my_dict \
	espeak-data/nci_dict \
	espeak-data/ne_dict \
	espeak-data/nl_dict \
	espeak-data/no_dict \
	espeak-data/om_dict \
	espeak-data/or_dict \
	espeak-data/pa_dict \
	espeak-data/pap_dict \
	espeak-data/pl_dict \
	espeak-data/pt_dict \
	espeak-data/ro_dict \
	espeak-data/ru_dict \
	espeak-data/si_dict \
	espeak-data/sk_dict \
	espeak-data/sl_dict \
	espeak-data/sq_dict \
	espeak-data/sv_dict \
	espeak-data/sw_dict \
	espeak-data/ta_dict \
	espeak-data/te_dict \
	espeak-data/tn_dict \
	espeak-data/tr_dict \
	espeak-data/tt_dict \
	espeak-data/ur_dict \
	espeak-data/vi_dict \
	espeak-data/zh_dict \
	espeak-data/zhy_dict

af: espeak-data/af_dict
dictsource/af_extra:
	touch dictsource/af_extra
espeak-data/af_dict: src/espeak-ng phsource/phonemes.stamp dictsource/af_list dictsource/af_rules dictsource/af_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=af && cd ..

am: espeak-data/am_dict
dictsource/am_extra:
	touch dictsource/am_extra
espeak-data/am_dict: src/espeak-ng phsource/phonemes.stamp dictsource/am_list dictsource/am_rules dictsource/am_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=am && cd ..

an: espeak-data/an_dict
dictsource/an_extra:
	touch dictsource/an_extra
espeak-data/an_dict: src/espeak-ng phsource/phonemes.stamp dictsource/an_list dictsource/an_rules dictsource/an_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=an && cd ..

ar: espeak-data/ar_dict
dictsource/ar_extra:
	touch dictsource/ar_extra
espeak-data/ar_dict: src/espeak-ng phsource/phonemes.stamp dictsource/ar_list dictsource/ar_rules dictsource/ar_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=an && cd ..

as: espeak-data/as_dict
dictsource/as_extra:
	touch dictsource/as_extra
espeak-data/as_dict: src/espeak-ng phsource/phonemes.stamp dictsource/as_list dictsource/as_rules dictsource/as_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=as && cd ..

az: espeak-data/az_dict
dictsource/az_extra:
	touch dictsource/az_extra
espeak-data/az_dict: src/espeak-ng phsource/phonemes.stamp dictsource/az_list dictsource/az_rules dictsource/az_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=az && cd ..

bg: espeak-data/bg_dict
dictsource/bg_extra:
	touch dictsource/bg_extra
espeak-data/bg_dict: src/espeak-ng phsource/phonemes.stamp dictsource/bg_list dictsource/bg_listx dictsource/bg_rules dictsource/bg_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=bg && cd ..

bn: espeak-data/bn_dict
dictsource/bn_extra:
	touch dictsource/bn_extra
espeak-data/bn_dict: src/espeak-ng phsource/phonemes.stamp dictsource/bn_list dictsource/bn_rules dictsource/bn_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=bn && cd ..

ca: espeak-data/ca_dict
dictsource/ca_extra:
	touch dictsource/ca_extra
espeak-data/ca_dict: src/espeak-ng phsource/phonemes.stamp dictsource/ca_list dictsource/ca_rules dictsource/ca_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=ca && cd ..

cs: espeak-data/cs_dict
dictsource/cs_extra:
	touch dictsource/cs_extra
espeak-data/cs_dict: src/espeak-ng phsource/phonemes.stamp dictsource/cs_list dictsource/cs_rules dictsource/cs_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=cs && cd ..

cy: espeak-data/cy_dict
dictsource/cy_extra:
	touch dictsource/cy_extra
espeak-data/cy_dict: src/espeak-ng phsource/phonemes.stamp dictsource/cy_list dictsource/cy_rules dictsource/cy_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=cy && cd ..

da: espeak-data/da_dict
dictsource/da_extra:
	touch dictsource/da_extra
espeak-data/da_dict: src/espeak-ng phsource/phonemes.stamp dictsource/da_list dictsource/da_rules dictsource/da_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=da && cd ..

de: espeak-data/de_dict
dictsource/de_extra:
	touch dictsource/de_extra
espeak-data/de_dict: src/espeak-ng phsource/phonemes.stamp dictsource/de_list dictsource/de_rules dictsource/de_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=de && cd ..

el: espeak-data/el_dict
dictsource/el_extra:
	touch dictsource/el_extra
espeak-data/el_dict: src/espeak-ng phsource/phonemes.stamp dictsource/el_list dictsource/el_rules dictsource/el_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=el && cd ..

en: espeak-data/en_dict
dictsource/en_extra:
	touch dictsource/en_extra
espeak-data/en_dict: src/espeak-ng phsource/phonemes.stamp dictsource/en_list dictsource/en_rules dictsource/en_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=en && cd ..

eo: espeak-data/eo_dict
dictsource/eo_extra:
	touch dictsource/eo_extra
espeak-data/eo_dict: src/espeak-ng phsource/phonemes.stamp dictsource/eo_list dictsource/eo_rules dictsource/eo_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=eo && cd ..

es: espeak-data/es_dict
dictsource/es_extra:
	touch dictsource/es_extra
espeak-data/es_dict: src/espeak-ng phsource/phonemes.stamp dictsource/es_list dictsource/es_rules dictsource/es_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=es && cd ..

et: espeak-data/et_dict
dictsource/et_extra:
	touch dictsource/et_extra
espeak-data/et_dict: src/espeak-ng phsource/phonemes.stamp dictsource/et_list dictsource/et_rules dictsource/et_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=et && cd ..

eu: espeak-data/eu_dict
dictsource/eu_extra:
	touch dictsource/eu_extra
espeak-data/eu_dict: src/espeak-ng phsource/phonemes.stamp dictsource/eu_list dictsource/eu_rules dictsource/eu_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=eu && cd ..

fa: espeak-data/fa_dict
dictsource/fa_extra:
	touch dictsource/fa_extra
espeak-data/fa_dict: src/espeak-ng phsource/phonemes.stamp dictsource/fa_list dictsource/fa_rules dictsource/fa_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=fa && cd ..

fi: espeak-data/fi_dict
dictsource/fi_extra:
	touch dictsource/fi_extra
espeak-data/fi_dict: src/espeak-ng phsource/phonemes.stamp dictsource/fi_list dictsource/fi_rules dictsource/fi_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=fi && cd ..

fr: espeak-data/fr_dict
dictsource/fr_extra:
	touch dictsource/fr_extra
espeak-data/fr_dict: src/espeak-ng phsource/phonemes.stamp dictsource/fr_list dictsource/fr_rules dictsource/fr_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=fr && cd ..

ga: espeak-data/ga_dict
dictsource/ga_extra:
	touch dictsource/ga_extra
espeak-data/ga_dict: src/espeak-ng phsource/phonemes.stamp dictsource/ga_list dictsource/ga_rules dictsource/ga_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=ga && cd ..

gd: espeak-data/gd_dict
dictsource/gd_extra:
	touch dictsource/gd_extra
espeak-data/gd_dict: src/espeak-ng phsource/phonemes.stamp dictsource/gd_list dictsource/gd_rules dictsource/gd_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=gd && cd ..

gn: espeak-data/gn_dict
dictsource/gn_extra:
	touch dictsource/gn_extra
espeak-data/gn_dict: src/espeak-ng phsource/phonemes.stamp dictsource/gn_list dictsource/gn_rules dictsource/gn_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=gn && cd ..

grc: espeak-data/grc_dict
dictsource/grc_extra:
	touch dictsource/grc_extra
espeak-data/grc_dict: src/espeak-ng phsource/phonemes.stamp dictsource/grc_list dictsource/grc_rules dictsource/grc_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=grc && cd ..

gu: espeak-data/gu_dict
dictsource/gu_extra:
	touch dictsource/gu_extra
espeak-data/gu_dict: src/espeak-ng phsource/phonemes.stamp dictsource/gu_list dictsource/gu_rules dictsource/gu_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=gu && cd ..

hbs: espeak-data/hbs_dict
dictsource/hbs_extra:
	touch dictsource/hbs_extra
espeak-data/hbs_dict: src/espeak-ng phsource/phonemes.stamp dictsource/hbs_list dictsource/hbs_rules dictsource/hbs_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=hbs && cd ..

hi: espeak-data/hi_dict
dictsource/hi_extra:
	touch dictsource/hi_extra
espeak-data/hi_dict: src/espeak-ng phsource/phonemes.stamp dictsource/hi_list dictsource/hi_rules dictsource/hi_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=hi && cd ..

hu: espeak-data/hu_dict
dictsource/hu_extra:
	touch dictsource/hu_extra
espeak-data/hu_dict: src/espeak-ng phsource/phonemes.stamp dictsource/hu_list dictsource/hu_rules dictsource/hu_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=hu && cd ..

hy: espeak-data/hy_dict
dictsource/hy_extra:
	touch dictsource/hy_extra
espeak-data/hy_dict: src/espeak-ng phsource/phonemes.stamp dictsource/hy_list dictsource/hy_rules dictsource/hy_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=hy && cd ..

ia: espeak-data/ia_dict
dictsource/ia_extra:
	touch dictsource/ia_extra
espeak-data/ia_dict: src/espeak-ng phsource/phonemes.stamp dictsource/ia_list dictsource/ia_rules dictsource/ia_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=ia && cd ..

id: espeak-data/id_dict
dictsource/id_extra:
	touch dictsource/id_extra
espeak-data/id_dict: src/espeak-ng phsource/phonemes.stamp dictsource/id_list dictsource/id_rules dictsource/id_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=id && cd ..

is: espeak-data/is_dict
dictsource/is_extra:
	touch dictsource/is_extra
espeak-data/is_dict: src/espeak-ng phsource/phonemes.stamp dictsource/is_list dictsource/is_rules dictsource/is_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=is && cd ..

it: espeak-data/it_dict
dictsource/it_extra:
	touch dictsource/it_extra
espeak-data/it_dict: src/espeak-ng phsource/phonemes.stamp dictsource/it_list dictsource/it_listx dictsource/it_rules dictsource/it_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=it && cd ..

jbo: espeak-data/jbo_dict
dictsource/jbo_extra:
	touch dictsource/jbo_extra
espeak-data/jbo_dict: src/espeak-ng phsource/phonemes.stamp dictsource/jbo_list dictsource/jbo_rules dictsource/jbo_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=jbo && cd ..

ka: espeak-data/ka_dict
dictsource/ka_extra:
	touch dictsource/ka_extra
espeak-data/ka_dict: src/espeak-ng phsource/phonemes.stamp dictsource/ka_list dictsource/ka_rules dictsource/ka_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=ka && cd ..

kl: espeak-data/kl_dict
dictsource/kl_extra:
	touch dictsource/kl_extra
espeak-data/kl_dict: src/espeak-ng phsource/phonemes.stamp dictsource/kl_list dictsource/kl_rules dictsource/kl_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=kl && cd ..

kn: espeak-data/kn_dict
dictsource/kn_extra:
	touch dictsource/kn_extra
espeak-data/kn_dict: src/espeak-ng phsource/phonemes.stamp dictsource/kn_list dictsource/kn_rules dictsource/kn_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=kn && cd ..

ko: espeak-data/ko_dict
dictsource/ko_extra:
	touch dictsource/ko_extra
espeak-data/ko_dict: src/espeak-ng phsource/phonemes.stamp dictsource/ko_list dictsource/ko_rules dictsource/ko_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=ko && cd ..

ku: espeak-data/ku_dict
dictsource/ku_extra:
	touch dictsource/ku_extra
espeak-data/ku_dict: src/espeak-ng phsource/phonemes.stamp dictsource/ku_list dictsource/ku_rules dictsource/ku_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=ku && cd ..

la: espeak-data/la_dict
dictsource/la_extra:
	touch dictsource/la_extra
espeak-data/la_dict: src/espeak-ng phsource/phonemes.stamp dictsource/la_list dictsource/la_rules dictsource/la_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=la && cd ..

lfn: espeak-data/lfn_dict
dictsource/lfn_extra:
	touch dictsource/lfn_extra
espeak-data/lfn_dict: src/espeak-ng phsource/phonemes.stamp dictsource/lfn_list dictsource/lfn_rules dictsource/lfn_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=lfn && cd ..

lt: espeak-data/lt_dict
dictsource/lt_extra:
	touch dictsource/lt_extra
espeak-data/lt_dict: src/espeak-ng phsource/phonemes.stamp dictsource/lt_list dictsource/lt_rules dictsource/lt_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=lt && cd ..

lv: espeak-data/lv_dict
dictsource/lv_extra:
	touch dictsource/lv_extra
espeak-data/lv_dict: src/espeak-ng phsource/phonemes.stamp dictsource/lv_list dictsource/lv_rules dictsource/lv_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=lv && cd ..

mk: espeak-data/mk_dict
dictsource/mk_extra:
	touch dictsource/mk_extra
espeak-data/mk_dict: src/espeak-ng phsource/phonemes.stamp dictsource/mk_list dictsource/mk_rules dictsource/mk_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=mk && cd ..

ml: espeak-data/ml_dict
dictsource/ml_extra:
	touch dictsource/ml_extra
espeak-data/ml_dict: src/espeak-ng phsource/phonemes.stamp dictsource/ml_list dictsource/ml_rules dictsource/ml_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=ml && cd ..

mr: espeak-data/mr_dict
dictsource/mr_extra:
	touch dictsource/mr_extra
espeak-data/mr_dict: src/espeak-ng phsource/phonemes.stamp dictsource/mr_list dictsource/mr_rules dictsource/mr_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=mr && cd ..

ms: espeak-data/ms_dict
dictsource/ms_extra:
	touch dictsource/ms_extra
espeak-data/ms_dict: src/espeak-ng phsource/phonemes.stamp dictsource/ms_list dictsource/ms_rules dictsource/ms_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=ms && cd ..

mt: espeak-data/mt_dict
dictsource/mt_extra:
	touch dictsource/mt_extra
espeak-data/mt_dict: src/espeak-ng phsource/phonemes.stamp dictsource/mt_list dictsource/mt_rules dictsource/mt_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=mt && cd ..

my: espeak-data/my_dict
dictsource/my_extra:
	touch dictsource/my_extra
espeak-data/my_dict: src/espeak-ng phsource/phonemes.stamp dictsource/my_list dictsource/my_rules dictsource/my_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=my && cd ..

nci: espeak-data/nci_dict
dictsource/nci_extra:
	touch dictsource/nci_extra
espeak-data/nci_dict: src/espeak-ng phsource/phonemes.stamp dictsource/nci_list dictsource/nci_rules dictsource/nci_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=nci && cd ..

ne: espeak-data/ne_dict
dictsource/ne_extra:
	touch dictsource/ne_extra
espeak-data/ne_dict: src/espeak-ng phsource/phonemes.stamp dictsource/ne_list dictsource/ne_rules dictsource/ne_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=ne && cd ..

nl: espeak-data/nl_dict
dictsource/nl_extra:
	touch dictsource/nl_extra
espeak-data/nl_dict: src/espeak-ng phsource/phonemes.stamp dictsource/nl_list dictsource/nl_rules dictsource/nl_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=nl && cd ..

no: espeak-data/no_dict
dictsource/no_extra:
	touch dictsource/no_extra
espeak-data/no_dict: src/espeak-ng phsource/phonemes.stamp dictsource/no_list dictsource/no_rules dictsource/no_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=no && cd ..

om: espeak-data/om_dict
dictsource/om_extra:
	touch dictsource/om_extra
espeak-data/om_dict: src/espeak-ng phsource/phonemes.stamp dictsource/om_list dictsource/om_rules dictsource/om_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=om && cd ..

or: espeak-data/or_dict
dictsource/or_extra:
	touch dictsource/or_extra
espeak-data/or_dict: src/espeak-ng phsource/phonemes.stamp dictsource/or_list dictsource/or_rules dictsource/or_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=or && cd ..

pa: espeak-data/pa_dict
dictsource/pa_extra:
	touch dictsource/pa_extra
espeak-data/pa_dict: src/espeak-ng phsource/phonemes.stamp dictsource/pa_list dictsource/pa_rules dictsource/pa_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=pa && cd ..

pap: espeak-data/pap_dict
dictsource/pap_extra:
	touch dictsource/pap_extra
espeak-data/pap_dict: src/espeak-ng phsource/phonemes.stamp dictsource/pap_list dictsource/pap_rules dictsource/pap_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=pap && cd ..

pl: espeak-data/pl_dict
dictsource/pl_extra:
	touch dictsource/pl_extra
espeak-data/pl_dict: src/espeak-ng phsource/phonemes.stamp dictsource/pl_list dictsource/pl_rules dictsource/pl_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=pl && cd ..

pt: espeak-data/pt_dict
dictsource/pt_extra:
	touch dictsource/pt_extra
espeak-data/pt_dict: src/espeak-ng phsource/phonemes.stamp dictsource/pt_list dictsource/pt_rules dictsource/pt_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=pt && cd ..

ro: espeak-data/ro_dict
dictsource/ro_extra:
	touch dictsource/ro_extra
espeak-data/ro_dict: src/espeak-ng phsource/phonemes.stamp dictsource/ro_list dictsource/ro_rules dictsource/ro_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=ro && cd ..

ru: espeak-data/ru_dict
dictsource/ru_extra:
	touch dictsource/ru_extra
dictsource/ru_listx:
	ln -svf extra/ru_listx dictsource/
if HAVE_RU_EXTENDED_DICTIONARY
espeak-data/ru_dict: src/espeak-ng phsource/phonemes.stamp dictsource/ru_list dictsource/ru_rules dictsource/ru_extra dictsource/ru_listx
else
espeak-data/ru_dict: src/espeak-ng phsource/phonemes.stamp dictsource/ru_list dictsource/ru_rules dictsource/ru_extra
endif
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=ru && cd ..

si: espeak-data/si_dict
dictsource/si_extra:
	touch dictsource/si_extra
espeak-data/si_dict: src/espeak-ng phsource/phonemes.stamp dictsource/si_list dictsource/si_rules dictsource/si_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=si && cd ..

sk: espeak-data/sk_dict
dictsource/sk_extra:
	touch dictsource/sk_extra
espeak-data/sk_dict: src/espeak-ng phsource/phonemes.stamp dictsource/sk_list dictsource/sk_rules dictsource/sk_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=sk && cd ..

sl: espeak-data/sl_dict
dictsource/sl_extra:
	touch dictsource/sl_extra
espeak-data/sl_dict: src/espeak-ng phsource/phonemes.stamp dictsource/sl_list dictsource/sl_rules dictsource/sl_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=sl && cd ..

sq: espeak-data/sq_dict
dictsource/sq_extra:
	touch dictsource/sq_extra
espeak-data/sq_dict: src/espeak-ng phsource/phonemes.stamp dictsource/sq_list dictsource/sq_rules dictsource/sq_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=sq && cd ..

sv: espeak-data/sv_dict
dictsource/sv_extra:
	touch dictsource/sv_extra
espeak-data/sv_dict: src/espeak-ng phsource/phonemes.stamp dictsource/sv_list dictsource/sv_rules dictsource/sv_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=sv && cd ..

sw: espeak-data/sw_dict
dictsource/sw_extra:
	touch dictsource/sw_extra
espeak-data/sw_dict: src/espeak-ng phsource/phonemes.stamp dictsource/sw_list dictsource/sw_rules dictsource/sw_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=sw && cd ..

ta: espeak-data/ta_dict
dictsource/ta_extra:
	touch dictsource/ta_extra
espeak-data/ta_dict: src/espeak-ng phsource/phonemes.stamp dictsource/ta_list dictsource/ta_rules dictsource/ta_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=ta && cd ..

te: espeak-data/te_dict
dictsource/te_extra:
	touch dictsource/te_extra
espeak-data/te_dict: src/espeak-ng phsource/phonemes.stamp dictsource/te_list dictsource/te_rules dictsource/te_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=te && cd ..

tn: espeak-data/tn_dict
dictsource/tn_extra:
	touch dictsource/tn_extra
espeak-data/tn_dict: src/espeak-ng phsource/phonemes.stamp dictsource/tn_list dictsource/tn_rules dictsource/tn_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=tn && cd ..

tr: espeak-data/tr_dict
dictsource/tr_extra:
	touch dictsource/tr_extra
espeak-data/tr_dict: src/espeak-ng phsource/phonemes.stamp dictsource/tr_list dictsource/tr_rules dictsource/tr_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=tr && cd ..

tt: espeak-data/tt_dict
dictsource/tt_extra:
	touch dictsource/tt_extra
espeak-data/tt_dict: src/espeak-ng phsource/phonemes.stamp dictsource/tt_list dictsource/tt_rules dictsource/tt_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=tt && cd ..

ur: espeak-data/ur_dict
dictsource/ur_extra:
	touch dictsource/ur_extra
espeak-data/ur_dict: src/espeak-ng phsource/phonemes.stamp dictsource/ur_list dictsource/ur_rules dictsource/ur_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=ur && cd ..

vi: espeak-data/vi_dict
dictsource/vi_extra:
	touch dictsource/vi_extra
espeak-data/vi_dict: src/espeak-ng phsource/phonemes.stamp dictsource/vi_list dictsource/vi_rules dictsource/vi_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=vi && cd ..

zh: espeak-data/zh_dict
dictsource/zh_extra:
	touch dictsource/zh_extra
dictsource/zh_listx:
	ln -svf extra/zh_listx dictsource/
if HAVE_ZH_EXTENDED_DICTIONARY
espeak-data/zh_dict: src/espeak-ng phsource/phonemes.stamp dictsource/zh_list dictsource/zh_rules dictsource/zh_extra dictsource/zh_listx
else
espeak-data/zh_dict: src/espeak-ng phsource/phonemes.stamp dictsource/zh_list dictsource/zh_rules dictsource/zh_extra
endif
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=zh && cd ..

zhy: espeak-data/zhy_dict
dictsource/zhy_extra:
	touch dictsource/zhy_extra
dictsource/zhy_list:
	ln -svf extra/zhy_list dictsource/
if HAVE_ZHY_EXTENDED_DICTIONARY
espeak-data/zhy_dict: src/espeak-ng phsource/phonemes.stamp dictsource/zhy_rules dictsource/zhy_extra dictsource/zhy_list
else
espeak-data/zhy_dict: src/espeak-ng phsource/phonemes.stamp dictsource/zhy_rules dictsource/zhy_extra
endif
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak-ng --compile=zhy && cd ..

##### mbrola:

mbrola: \
	espeak-data/mbrola_ph/af1_phtrans \
	espeak-data/mbrola_ph/ar1_phtrans \
	espeak-data/mbrola_ph/ar2_phtrans \
	espeak-data/mbrola_ph/cr1_phtrans \
	espeak-data/mbrola_ph/cs_phtrans \
	espeak-data/mbrola_ph/de2_phtrans \
	espeak-data/mbrola_ph/de4_phtrans \
	espeak-data/mbrola_ph/de6_phtrans \
	espeak-data/mbrola_ph/ee1_phtrans \
	espeak-data/mbrola_ph/en1_phtrans \
	espeak-data/mbrola_ph/es_phtrans \
	espeak-data/mbrola_ph/fr1_phtrans \
	espeak-data/mbrola_ph/gr2_phtrans \
	espeak-data/mbrola_ph/grc-de6_phtrans \
	espeak-data/mbrola_ph/hn1_phtrans \
	espeak-data/mbrola_ph/hu1_phtrans \
	espeak-data/mbrola_ph/ic1_phtrans \
	espeak-data/mbrola_ph/id1_phtrans \
	espeak-data/mbrola_ph/in1_phtrans \
	espeak-data/mbrola_ph/ir1_phtrans \
	espeak-data/mbrola_ph/it3_phtrans \
	espeak-data/mbrola_ph/la1_phtrans \
	espeak-data/mbrola_ph/lt1_phtrans \
	espeak-data/mbrola_ph/lt2_phtrans \
	espeak-data/mbrola_ph/mx1_phtrans \
	espeak-data/mbrola_ph/mx2_phtrans \
	espeak-data/mbrola_ph/nl_phtrans \
	espeak-data/mbrola_ph/pl1_phtrans \
	espeak-data/mbrola_ph/pt1_phtrans \
	espeak-data/mbrola_ph/ptbr_phtrans \
	espeak-data/mbrola_ph/ptbr4_phtrans \
	espeak-data/mbrola_ph/ro1_phtrans \
	espeak-data/mbrola_ph/sv_phtrans \
	espeak-data/mbrola_ph/sv2_phtrans \
	espeak-data/mbrola_ph/tr1_phtrans \
	espeak-data/mbrola_ph/us_phtrans \
	espeak-data/mbrola_ph/us3_phtrans \
	espeak-data/mbrola_ph/vz_phtrans

espeak-data/mbrola_ph/af1_phtrans: phsource/mbrola/af1 src/espeak-ng
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeak-ng --compile-mbrola=phsource/mbrola/af1

espeak-data/mbrola_ph/ar1_phtrans: phsource/mbrola/ar1 src/espeak-ng
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeak-ng --compile-mbrola=phsource/mbrola/ar1
espeak-data/mbrola_ph/ar2_phtrans: phsource/mbrola/ar2 src/espeak-ng
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeak-ng --compile-mbrola=phsource/mbrola/ar2
	
espeak-data/mbrola_ph/cr1_phtrans: phsource/mbrola/cr1 src/espeak-ng
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeak-ng --compile-mbrola=phsource/mbrola/cr1

espeak-data/mbrola_ph/cs_phtrans: phsource/mbrola/cs src/espeak-ng
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeak-ng --compile-mbrola=phsource/mbrola/cs

espeak-data/mbrola_ph/de2_phtrans: phsource/mbrola/de2 src/espeak-ng
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeak-ng --compile-mbrola=phsource/mbrola/de2

espeak-data/mbrola_ph/de4_phtrans: phsource/mbrola/de4 src/espeak-ng
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeak-ng --compile-mbrola=phsource/mbrola/de4

espeak-data/mbrola_ph/de6_phtrans: phsource/mbrola/de6 src/espeak-ng
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeak-ng --compile-mbrola=phsource/mbrola/de6

espeak-data/mbrola_ph/ee1_phtrans: phsource/mbrola/ee1 src/espeak-ng
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeak-ng --compile-mbrola=phsource/mbrola/ee1

espeak-data/mbrola_ph/en1_phtrans: phsource/mbrola/en1 src/espeak-ng
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeak-ng --compile-mbrola=phsource/mbrola/en1

espeak-data/mbrola_ph/es_phtrans: phsource/mbrola/es src/espeak-ng
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeak-ng --compile-mbrola=phsource/mbrola/es

espeak-data/mbrola_ph/fr1_phtrans: phsource/mbrola/fr1 src/espeak-ng
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeak-ng --compile-mbrola=phsource/mbrola/fr1

espeak-data/mbrola_ph/gr2_phtrans: phsource/mbrola/gr2 src/espeak-ng
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeak-ng --compile-mbrola=phsource/mbrola/gr2

espeak-data/mbrola_ph/grc-de6_phtrans: phsource/mbrola/grc-de6 src/espeak-ng
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeak-ng --compile-mbrola=phsource/mbrola/grc-de6

espeak-data/mbrola_ph/hn1_phtrans: phsource/mbrola/hn1 src/espeak-ng
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeak-ng --compile-mbrola=phsource/mbrola/hn1

espeak-data/mbrola_ph/hu1_phtrans: phsource/mbrola/hu1 src/espeak-ng
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeak-ng --compile-mbrola=phsource/mbrola/hu1

espeak-data/mbrola_ph/ic1_phtrans: phsource/mbrola/ic1 src/espeak-ng
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeak-ng --compile-mbrola=phsource/mbrola/ic1

espeak-data/mbrola_ph/id1_phtrans: phsource/mbrola/id1 src/espeak-ng
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeak-ng --compile-mbrola=phsource/mbrola/id1

espeak-data/mbrola_ph/in1_phtrans: phsource/mbrola/in1 src/espeak-ng
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeak-ng --compile-mbrola=phsource/mbrola/in1

espeak-data/mbrola_ph/ir1_phtrans: phsource/mbrola/ir1 src/espeak-ng
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeak-ng --compile-mbrola=phsource/mbrola/ir1

espeak-data/mbrola_ph/it3_phtrans: phsource/mbrola/it3 src/espeak-ng
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeak-ng --compile-mbrola=phsource/mbrola/it3

espeak-data/mbrola_ph/la1_phtrans: phsource/mbrola/la1 src/espeak-ng
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeak-ng --compile-mbrola=phsource/mbrola/la1

espeak-data/mbrola_ph/lt1_phtrans: phsource/mbrola/lt1 src/espeak-ng
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeak-ng --compile-mbrola=phsource/mbrola/lt1

espeak-data/mbrola_ph/lt2_phtrans: phsource/mbrola/lt2 src/espeak-ng
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeak-ng --compile-mbrola=phsource/mbrola/lt2

espeak-data/mbrola_ph/mx1_phtrans: phsource/mbrola/mx1 src/espeak-ng
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeak-ng --compile-mbrola=phsource/mbrola/mx1

espeak-data/mbrola_ph/mx2_phtrans: phsource/mbrola/mx2 src/espeak-ng
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeak-ng --compile-mbrola=phsource/mbrola/mx2

espeak-data/mbrola_ph/nl_phtrans: phsource/mbrola/nl src/espeak-ng
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeak-ng --compile-mbrola=phsource/mbrola/nl

espeak-data/mbrola_ph/pl1_phtrans: phsource/mbrola/pl1 src/espeak-ng
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeak-ng --compile-mbrola=phsource/mbrola/pl1

espeak-data/mbrola_ph/pt1_phtrans: phsource/mbrola/pt1 src/espeak-ng
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeak-ng --compile-mbrola=phsource/mbrola/pt1

espeak-data/mbrola_ph/ptbr_phtrans: phsource/mbrola/ptbr src/espeak-ng
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeak-ng --compile-mbrola=phsource/mbrola/ptbr

espeak-data/mbrola_ph/ptbr4_phtrans: phsource/mbrola/ptbr4 src/espeak-ng
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeak-ng --compile-mbrola=phsource/mbrola/ptbr4

espeak-data/mbrola_ph/ro1_phtrans: phsource/mbrola/ro1 src/espeak-ng
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeak-ng --compile-mbrola=phsource/mbrola/ro1

espeak-data/mbrola_ph/sv_phtrans: phsource/mbrola/sv src/espeak-ng
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeak-ng --compile-mbrola=phsource/mbrola/sv

espeak-data/mbrola_ph/sv2_phtrans: phsource/mbrola/sv2 src/espeak-ng
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeak-ng --compile-mbrola=phsource/mbrola/sv2

espeak-data/mbrola_ph/tr1_phtrans: phsource/mbrola/tr1 src/espeak-ng
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeak-ng --compile-mbrola=phsource/mbrola/tr1

espeak-data/mbrola_ph/us_phtrans: phsource/mbrola/us src/espeak-ng
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeak-ng --compile-mbrola=phsource/mbrola/us

espeak-data/mbrola_ph/us3_phtrans: phsource/mbrola/us3 src/espeak-ng
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeak-ng --compile-mbrola=phsource/mbrola/us3

espeak-data/mbrola_ph/vz_phtrans: phsource/mbrola/vz src/espeak-ng
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeak-ng --compile-mbrola=phsource/mbrola/vz
