AUTOMAKE_OPTIONS = subdir-objects

PREFIX=@prefix@
BINDIR=@bindir@
INCDIR=@includedir@/espeak
LIBDIR=@libdir@
DATADIR=$(PREFIX)/share/espeak-data

SHARED_VERSION=2:48:1 # = *.so.1.1.48

MKDIR=mkdir -p

AM_CXXFLAGS = -Wall

EXTRA_DIST=

bin_PROGRAMS =
lib_LTLIBRARIES =

##### ChangeLog:

ChangeLog:
	git log > ChangeLog

dist-hook: ChangeLog

.PHONY: ChangeLog

EXTRA_DIST += ChangeLog

##### standard build actions:

all-local: jni espeakdata apk-release

uninstall-hook:
	rm -rf $(DESTDIR)$(DATADIR)

install-data-hook: uninstall-hook
	$(MKDIR) $(DESTDIR)$(DATADIR)
	cp -prf espeak-data/* $(DESTDIR)$(DATADIR)

clean-local:
	cd src && rm -f *.o *~ && cd ..
	cd android && $(GRADLE) clean

distclean-local:
	rm -rf espeak-data/phondata-manifest
	rm -f espeak-data/*_dict

check: apk-check

##### build targets:

libespeak_includedir = $(includedir)/espeak
libespeak_include_HEADERS = \
	src/speak_lib.h

common_FLAGS  =
common_SOURCE = \
	src/compiledict.cpp \
	src/dictionary.cpp \
	src/intonation.cpp \
	src/numbers.cpp \
	src/readclause.cpp \
	src/phonemelist.cpp \
	src/setlengths.cpp \
	src/synthdata.cpp \
	src/synthesize.cpp \
	src/synth_mbrola.cpp \
	src/translate.cpp \
	src/tr_languages.cpp \
	src/voices.cpp \
	src/wavegen.cpp 

libespeak_FLAGS  =
libespeak_SOURCE = \
	src/speak_lib.cpp \
	src/debug.cpp

espeakedit_SOURCE = \
	src/compiledata.cpp \
	src/espeakedit.cpp \
	src/extras.cpp \
	src/formantdlg.cpp \
	src/menus.cpp \
	src/options.cpp \
	src/prosodydisplay.cpp \
	src/spect.cpp \
	src/spectdisplay.cpp \
	src/spectseq.cpp \
	src/transldlg.cpp \
	src/voicedlg.cpp \
	src/vowelchart.cpp

if OPT_KLATT
common_FLAGS  += -DINCLUDE_KLATT
common_SOURCE += src/klatt.cpp
endif

if OPT_MBROLA
common_FLAGS  += -DINCLUDE_MBROLA
common_SOURCE += src/mbrowrap.cpp
endif

if OPT_SONIC
common_FLAGS  += -DINCLUDE_SONIC
common_SOURCE += src/sonic.cpp
endif

if OPT_ASYNC
libespeak_FLAGS  += -DUSE_ASYNC
libespeak_SOURCE += \
	src/espeak_command.cpp \
	src/event.cpp \
	src/fifo.cpp
endif

if AUDIO_RUNTIME
wave_SOURCE = src/wave.cpp src/wave_pulse.cpp
wave_LIBS   = -lpulse -lpulse-simple -lportaudio
wave_FLAGS  = -DUSE_PULSEAUDIO -DUSE_PORTAUDIO
else
if AUDIO_PULSEAUDIO
wave_SOURCE = src/wave_pulse.cpp
wave_LIBS   = -lpulse
wave_FLAGS  = -DUSE_PULSEAUDIO
else
if AUDIO_PORTAUDIO
wave_SOURCE = src/wave.cpp
wave_LIBS   = -lportaudio
wave_FLAGS  = -DUSE_PORTAUDIO
else
if AUDIO_SADA
wave_SOURCE = src/wave_sada.cpp
wave_FLAGS  = -DUSE_SADA
else
wave_SOURCE = src/wave.cpp
endif
endif
endif
endif

docs/speak_lib.h: src/speak_lib.h
	cp $< $@

lib_LTLIBRARIES += src/libespeak.la

src_libespeak_la_LDFLAGS  = -version-info $(SHARED_VERSION) $(wave_LIBS) -lpthread
src_libespeak_la_CXXFLAGS = $(common_FLAGS) $(libespeak_FLAGS) $(wave_FLAGS) \
	-fpic -fvisibility=hidden -D LIBRARY \
	-pedantic -fno-exceptions -D PATH_ESPEAK_DATA=\"$(DATADIR)\"
src_libespeak_la_SOURCES  = $(common_SOURCE) $(libespeak_SOURCE) $(wave_SOURCE)

bin_PROGRAMS += src/speak

src_speak_LDFLAGS  = $(wave_LIBS) -lpthread
src_speak_CXXFLAGS = $(common_FLAGS) $(wave_FLAGS)
src_speak_SOURCES  = src/speak.cpp $(common_SOURCE) $(wave_SOURCE)

bin_PROGRAMS += src/espeak

src_espeak_LDADD   = src/libespeak.la
src_espeak_SOURCES = src/espeak.cpp

bin_PROGRAMS += src/espeakedit

src_espeakedit_LDFLAGS  = $(wave_LIBS) ${WX_LIBS}
src_espeakedit_CXXFLAGS = ${WX_CXXFLAGS} $(common_FLAGS) $(libespeak_FLAGS) $(wave_FLAGS) \
	-D_LARGEFILE_SOURCE=1 -DNO_GCC_PRAGMA -D_ESPEAKEDIT
src_espeakedit_SOURCES  = $(common_SOURCE) $(libespeak_SOURCE) $(espeakedit_SOURCE) $(wave_SOURCE)

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
	src/espeakedit
	ESPEAK_DATA_PATH=$(PWD) src/espeakedit --compile && touch $@

##### android targets:

jni:
	cd android && ndk-build

apk-release:
	cd android && $(GRADLE) assembleRelease

apk-debug:
	cd android && $(GRADLE) assembleDebug

apk-check:
	cd android && $(GRADLE) connectedCheck

android/res/raw/espeakdata.zip: espeak-data/phontab dictionaries
	mkdir -pv android/res/raw
	rm -f $@
	find espeak-data/{intonations,phondata,phonindex,phontab} | zip -@ $@
	find espeak-data/*_dict | zip -@ $@
	find espeak-data/voices -type f | grep -vF "/mb/" | zip -@ $@

android/res/raw/espeakdata_version: android/res/raw/espeakdata.zip
	sha1sum $< | awk '{ print $$1 }' > $@

espeakdata: \
	android/res/raw/espeakdata.zip \
	android/res/raw/espeakdata_version

##### dictionaries:

dictionaries: src/espeak \
	espeak-data/af_dict \
	espeak-data/am_dict \
	espeak-data/an_dict \
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
	espeak-data/tr_dict \
	espeak-data/ur_dict \
	espeak-data/vi_dict \
	espeak-data/zh_dict \
	espeak-data/zhy_dict

af: espeak-data/af_dict
dictsource/af_extra:
	touch dictsource/af_extra
espeak-data/af_dict: src/espeak phsource/phonemes.stamp dictsource/af_list dictsource/af_rules dictsource/af_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=af && cd ..

am: espeak-data/am_dict
dictsource/am_extra:
	touch dictsource/am_extra
espeak-data/am_dict: src/espeak phsource/phonemes.stamp dictsource/am_list dictsource/am_rules dictsource/am_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=am && cd ..

an: espeak-data/an_dict
dictsource/an_extra:
	touch dictsource/an_extra
espeak-data/an_dict: src/espeak phsource/phonemes.stamp dictsource/an_list dictsource/an_rules dictsource/an_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=an && cd ..

as: espeak-data/as_dict
dictsource/as_extra:
	touch dictsource/as_extra
espeak-data/as_dict: src/espeak phsource/phonemes.stamp dictsource/as_list dictsource/as_rules dictsource/as_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=as && cd ..

az: espeak-data/az_dict
dictsource/az_extra:
	touch dictsource/az_extra
espeak-data/az_dict: src/espeak phsource/phonemes.stamp dictsource/az_list dictsource/az_rules dictsource/az_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=az && cd ..

bg: espeak-data/bg_dict
dictsource/bg_extra:
	touch dictsource/bg_extra
espeak-data/bg_dict: src/espeak phsource/phonemes.stamp dictsource/bg_list dictsource/bg_listx dictsource/bg_rules dictsource/bg_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=bg && cd ..

bn: espeak-data/bn_dict
dictsource/bn_extra:
	touch dictsource/bn_extra
espeak-data/bn_dict: src/espeak phsource/phonemes.stamp dictsource/bn_list dictsource/bn_rules dictsource/bn_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=bn && cd ..

ca: espeak-data/ca_dict
dictsource/ca_extra:
	touch dictsource/ca_extra
espeak-data/ca_dict: src/espeak phsource/phonemes.stamp dictsource/ca_list dictsource/ca_rules dictsource/ca_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=ca && cd ..

cs: espeak-data/cs_dict
dictsource/cs_extra:
	touch dictsource/cs_extra
espeak-data/cs_dict: src/espeak phsource/phonemes.stamp dictsource/cs_list dictsource/cs_rules dictsource/cs_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=cs && cd ..

cy: espeak-data/cy_dict
dictsource/cy_extra:
	touch dictsource/cy_extra
espeak-data/cy_dict: src/espeak phsource/phonemes.stamp dictsource/cy_list dictsource/cy_rules dictsource/cy_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=cy && cd ..

da: espeak-data/da_dict
dictsource/da_extra:
	touch dictsource/da_extra
espeak-data/da_dict: src/espeak phsource/phonemes.stamp dictsource/da_list dictsource/da_rules dictsource/da_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=da && cd ..

de: espeak-data/de_dict
dictsource/de_extra:
	touch dictsource/de_extra
espeak-data/de_dict: src/espeak phsource/phonemes.stamp dictsource/de_list dictsource/de_rules dictsource/de_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=de && cd ..

el: espeak-data/el_dict
dictsource/el_extra:
	touch dictsource/el_extra
espeak-data/el_dict: src/espeak phsource/phonemes.stamp dictsource/el_list dictsource/el_rules dictsource/el_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=el && cd ..

en: espeak-data/en_dict
dictsource/en_extra:
	touch dictsource/en_extra
espeak-data/en_dict: src/espeak phsource/phonemes.stamp dictsource/en_list dictsource/en_rules dictsource/en_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=en && cd ..

eo: espeak-data/eo_dict
dictsource/eo_extra:
	touch dictsource/eo_extra
espeak-data/eo_dict: src/espeak phsource/phonemes.stamp dictsource/eo_list dictsource/eo_rules dictsource/eo_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=eo && cd ..

es: espeak-data/es_dict
dictsource/es_extra:
	touch dictsource/es_extra
espeak-data/es_dict: src/espeak phsource/phonemes.stamp dictsource/es_list dictsource/es_rules dictsource/es_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=es && cd ..

et: espeak-data/et_dict
dictsource/et_extra:
	touch dictsource/et_extra
espeak-data/et_dict: src/espeak phsource/phonemes.stamp dictsource/et_list dictsource/et_rules dictsource/et_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=et && cd ..

eu: espeak-data/eu_dict
dictsource/eu_extra:
	touch dictsource/eu_extra
espeak-data/eu_dict: src/espeak phsource/phonemes.stamp dictsource/eu_list dictsource/eu_rules dictsource/eu_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=eu && cd ..

fa: espeak-data/fa_dict
dictsource/fa_extra:
	touch dictsource/fa_extra
espeak-data/fa_dict: src/espeak phsource/phonemes.stamp dictsource/fa_list dictsource/fa_rules dictsource/fa_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=fa && cd ..

fi: espeak-data/fi_dict
dictsource/fi_extra:
	touch dictsource/fi_extra
espeak-data/fi_dict: src/espeak phsource/phonemes.stamp dictsource/fi_list dictsource/fi_rules dictsource/fi_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=fi && cd ..

fr: espeak-data/fr_dict
dictsource/fr_extra:
	touch dictsource/fr_extra
espeak-data/fr_dict: src/espeak phsource/phonemes.stamp dictsource/fr_list dictsource/fr_rules dictsource/fr_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=fr && cd ..

ga: espeak-data/ga_dict
dictsource/ga_extra:
	touch dictsource/ga_extra
espeak-data/ga_dict: src/espeak phsource/phonemes.stamp dictsource/ga_list dictsource/ga_rules dictsource/ga_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=ga && cd ..

gd: espeak-data/gd_dict
dictsource/gd_extra:
	touch dictsource/gd_extra
espeak-data/gd_dict: src/espeak phsource/phonemes.stamp dictsource/gd_list dictsource/gd_rules dictsource/gd_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=gd && cd ..

grc: espeak-data/grc_dict
dictsource/grc_extra:
	touch dictsource/grc_extra
espeak-data/grc_dict: src/espeak phsource/phonemes.stamp dictsource/grc_list dictsource/grc_rules dictsource/grc_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=grc && cd ..

gu: espeak-data/gu_dict
dictsource/gu_extra:
	touch dictsource/gu_extra
espeak-data/gu_dict: src/espeak phsource/phonemes.stamp dictsource/gu_list dictsource/gu_rules dictsource/gu_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=gu && cd ..

hbs: espeak-data/hbs_dict
dictsource/hbs_extra:
	touch dictsource/hbs_extra
espeak-data/hbs_dict: src/espeak phsource/phonemes.stamp dictsource/hbs_list dictsource/hbs_rules dictsource/hbs_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=hbs && cd ..

hi: espeak-data/hi_dict
dictsource/hi_extra:
	touch dictsource/hi_extra
espeak-data/hi_dict: src/espeak phsource/phonemes.stamp dictsource/hi_list dictsource/hi_rules dictsource/hi_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=hi && cd ..

hu: espeak-data/hu_dict
dictsource/hu_extra:
	touch dictsource/hu_extra
espeak-data/hu_dict: src/espeak phsource/phonemes.stamp dictsource/hu_list dictsource/hu_rules dictsource/hu_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=hu && cd ..

hy: espeak-data/hy_dict
dictsource/hy_extra:
	touch dictsource/hy_extra
espeak-data/hy_dict: src/espeak phsource/phonemes.stamp dictsource/hy_list dictsource/hy_rules dictsource/hy_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=hy && cd ..

ia: espeak-data/ia_dict
dictsource/ia_extra:
	touch dictsource/ia_extra
espeak-data/ia_dict: src/espeak phsource/phonemes.stamp dictsource/ia_list dictsource/ia_rules dictsource/ia_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=ia && cd ..

id: espeak-data/id_dict
dictsource/id_extra:
	touch dictsource/id_extra
espeak-data/id_dict: src/espeak phsource/phonemes.stamp dictsource/id_list dictsource/id_rules dictsource/id_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=id && cd ..

is: espeak-data/is_dict
dictsource/is_extra:
	touch dictsource/is_extra
espeak-data/is_dict: src/espeak phsource/phonemes.stamp dictsource/is_list dictsource/is_rules dictsource/is_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=is && cd ..

it: espeak-data/it_dict
dictsource/it_extra:
	touch dictsource/it_extra
espeak-data/it_dict: src/espeak phsource/phonemes.stamp dictsource/it_list dictsource/it_listx dictsource/it_rules dictsource/it_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=it && cd ..

jbo: espeak-data/jbo_dict
dictsource/jbo_extra:
	touch dictsource/jbo_extra
espeak-data/jbo_dict: src/espeak phsource/phonemes.stamp dictsource/jbo_list dictsource/jbo_rules dictsource/jbo_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=jbo && cd ..

ka: espeak-data/ka_dict
dictsource/ka_extra:
	touch dictsource/ka_extra
espeak-data/ka_dict: src/espeak phsource/phonemes.stamp dictsource/ka_list dictsource/ka_rules dictsource/ka_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=ka && cd ..

kl: espeak-data/kl_dict
dictsource/kl_extra:
	touch dictsource/kl_extra
espeak-data/kl_dict: src/espeak phsource/phonemes.stamp dictsource/kl_list dictsource/kl_rules dictsource/kl_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=kl && cd ..

kn: espeak-data/kn_dict
dictsource/kn_extra:
	touch dictsource/kn_extra
espeak-data/kn_dict: src/espeak phsource/phonemes.stamp dictsource/kn_list dictsource/kn_rules dictsource/kn_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=kn && cd ..

ko: espeak-data/ko_dict
dictsource/ko_extra:
	touch dictsource/ko_extra
espeak-data/ko_dict: src/espeak phsource/phonemes.stamp dictsource/ko_list dictsource/ko_rules dictsource/ko_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=ko && cd ..

ku: espeak-data/ku_dict
dictsource/ku_extra:
	touch dictsource/ku_extra
espeak-data/ku_dict: src/espeak phsource/phonemes.stamp dictsource/ku_list dictsource/ku_rules dictsource/ku_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=ku && cd ..

la: espeak-data/la_dict
dictsource/la_extra:
	touch dictsource/la_extra
espeak-data/la_dict: src/espeak phsource/phonemes.stamp dictsource/la_list dictsource/la_rules dictsource/la_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=la && cd ..

lfn: espeak-data/lfn_dict
dictsource/lfn_extra:
	touch dictsource/lfn_extra
espeak-data/lfn_dict: src/espeak phsource/phonemes.stamp dictsource/lfn_list dictsource/lfn_rules dictsource/lfn_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=lfn && cd ..

lt: espeak-data/lt_dict
dictsource/lt_extra:
	touch dictsource/lt_extra
espeak-data/lt_dict: src/espeak phsource/phonemes.stamp dictsource/lt_list dictsource/lt_rules dictsource/lt_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=lt && cd ..

lv: espeak-data/lv_dict
dictsource/lv_extra:
	touch dictsource/lv_extra
espeak-data/lv_dict: src/espeak phsource/phonemes.stamp dictsource/lv_list dictsource/lv_rules dictsource/lv_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=lv && cd ..

mk: espeak-data/mk_dict
dictsource/mk_extra:
	touch dictsource/mk_extra
espeak-data/mk_dict: src/espeak phsource/phonemes.stamp dictsource/mk_list dictsource/mk_rules dictsource/mk_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=mk && cd ..

ml: espeak-data/ml_dict
dictsource/ml_extra:
	touch dictsource/ml_extra
espeak-data/ml_dict: src/espeak phsource/phonemes.stamp dictsource/ml_list dictsource/ml_rules dictsource/ml_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=ml && cd ..

mr: espeak-data/mr_dict
dictsource/mr_extra:
	touch dictsource/mr_extra
espeak-data/mr_dict: src/espeak phsource/phonemes.stamp dictsource/mr_list dictsource/mr_rules dictsource/mr_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=mr && cd ..

ms: espeak-data/ms_dict
dictsource/ms_extra:
	touch dictsource/ms_extra
espeak-data/ms_dict: src/espeak phsource/phonemes.stamp dictsource/ms_list dictsource/ms_rules dictsource/ms_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=ms && cd ..

nci: espeak-data/nci_dict
dictsource/nci_extra:
	touch dictsource/nci_extra
espeak-data/nci_dict: src/espeak phsource/phonemes.stamp dictsource/nci_list dictsource/nci_rules dictsource/nci_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=nci && cd ..

ne: espeak-data/ne_dict
dictsource/ne_extra:
	touch dictsource/ne_extra
espeak-data/ne_dict: src/espeak phsource/phonemes.stamp dictsource/ne_list dictsource/ne_rules dictsource/ne_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=ne && cd ..

nl: espeak-data/nl_dict
dictsource/nl_extra:
	touch dictsource/nl_extra
espeak-data/nl_dict: src/espeak phsource/phonemes.stamp dictsource/nl_list dictsource/nl_rules dictsource/nl_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=nl && cd ..

no: espeak-data/no_dict
dictsource/no_extra:
	touch dictsource/no_extra
espeak-data/no_dict: src/espeak phsource/phonemes.stamp dictsource/no_list dictsource/no_rules dictsource/no_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=no && cd ..

om: espeak-data/om_dict
dictsource/om_extra:
	touch dictsource/om_extra
espeak-data/om_dict: src/espeak phsource/phonemes.stamp dictsource/om_list dictsource/om_rules dictsource/om_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=om && cd ..

or: espeak-data/or_dict
dictsource/or_extra:
	touch dictsource/or_extra
espeak-data/or_dict: src/espeak phsource/phonemes.stamp dictsource/or_list dictsource/or_rules dictsource/or_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=or && cd ..

pa: espeak-data/pa_dict
dictsource/pa_extra:
	touch dictsource/pa_extra
espeak-data/pa_dict: src/espeak phsource/phonemes.stamp dictsource/pa_list dictsource/pa_rules dictsource/pa_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=pa && cd ..

pap: espeak-data/pap_dict
dictsource/pap_extra:
	touch dictsource/pap_extra
espeak-data/pap_dict: src/espeak phsource/phonemes.stamp dictsource/pap_list dictsource/pap_rules dictsource/pap_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=pap && cd ..

pl: espeak-data/pl_dict
dictsource/pl_extra:
	touch dictsource/pl_extra
espeak-data/pl_dict: src/espeak phsource/phonemes.stamp dictsource/pl_list dictsource/pl_rules dictsource/pl_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=pl && cd ..

pt: espeak-data/pt_dict
dictsource/pt_extra:
	touch dictsource/pt_extra
espeak-data/pt_dict: src/espeak phsource/phonemes.stamp dictsource/pt_list dictsource/pt_rules dictsource/pt_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=pt && cd ..

ro: espeak-data/ro_dict
dictsource/ro_extra:
	touch dictsource/ro_extra
espeak-data/ro_dict: src/espeak phsource/phonemes.stamp dictsource/ro_list dictsource/ro_rules dictsource/ro_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=ro && cd ..

ru: espeak-data/ru_dict
dictsource/ru_extra:
	touch dictsource/ru_extra
dictsource/ru_listx:
	ln -svf extra/ru_listx dictsource/
if HAVE_RU_EXTENDED_DICTIONARY
espeak-data/ru_dict: src/espeak phsource/phonemes.stamp dictsource/ru_list dictsource/ru_rules dictsource/ru_extra dictsource/ru_listx
else
espeak-data/ru_dict: src/espeak phsource/phonemes.stamp dictsource/ru_list dictsource/ru_rules dictsource/ru_extra
endif
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=ru && cd ..

si: espeak-data/si_dict
dictsource/si_extra:
	touch dictsource/si_extra
espeak-data/si_dict: src/espeak phsource/phonemes.stamp dictsource/si_list dictsource/si_rules dictsource/si_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=si && cd ..

sk: espeak-data/sk_dict
dictsource/sk_extra:
	touch dictsource/sk_extra
espeak-data/sk_dict: src/espeak phsource/phonemes.stamp dictsource/sk_list dictsource/sk_rules dictsource/sk_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=sk && cd ..

sl: espeak-data/sl_dict
dictsource/sl_extra:
	touch dictsource/sl_extra
espeak-data/sl_dict: src/espeak phsource/phonemes.stamp dictsource/sl_list dictsource/sl_rules dictsource/sl_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=sl && cd ..

sq: espeak-data/sq_dict
dictsource/sq_extra:
	touch dictsource/sq_extra
espeak-data/sq_dict: src/espeak phsource/phonemes.stamp dictsource/sq_list dictsource/sq_rules dictsource/sq_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=sq && cd ..

sv: espeak-data/sv_dict
dictsource/sv_extra:
	touch dictsource/sv_extra
espeak-data/sv_dict: src/espeak phsource/phonemes.stamp dictsource/sv_list dictsource/sv_rules dictsource/sv_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=sv && cd ..

sw: espeak-data/sw_dict
dictsource/sw_extra:
	touch dictsource/sw_extra
espeak-data/sw_dict: src/espeak phsource/phonemes.stamp dictsource/sw_list dictsource/sw_rules dictsource/sw_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=sw && cd ..

ta: espeak-data/ta_dict
dictsource/ta_extra:
	touch dictsource/ta_extra
espeak-data/ta_dict: src/espeak phsource/phonemes.stamp dictsource/ta_list dictsource/ta_rules dictsource/ta_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=ta && cd ..

te: espeak-data/te_dict
dictsource/te_extra:
	touch dictsource/te_extra
espeak-data/te_dict: src/espeak phsource/phonemes.stamp dictsource/te_list dictsource/te_rules dictsource/te_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=te && cd ..

tr: espeak-data/tr_dict
dictsource/tr_extra:
	touch dictsource/tr_extra
espeak-data/tr_dict: src/espeak phsource/phonemes.stamp dictsource/tr_list dictsource/tr_rules dictsource/tr_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=tr && cd ..

ur: espeak-data/ur_dict
dictsource/ur_extra:
	touch dictsource/ur_extra
espeak-data/ur_dict: src/espeak phsource/phonemes.stamp dictsource/ur_list dictsource/ur_rules dictsource/ur_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=ur && cd ..

vi: espeak-data/vi_dict
dictsource/vi_extra:
	touch dictsource/vi_extra
espeak-data/vi_dict: src/espeak phsource/phonemes.stamp dictsource/vi_list dictsource/vi_rules dictsource/vi_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=vi && cd ..

zh: espeak-data/zh_dict
dictsource/zh_extra:
	touch dictsource/zh_extra
dictsource/zh_listx:
	ln -svf extra/zh_listx dictsource/
if HAVE_ZH_EXTENDED_DICTIONARY
espeak-data/zh_dict: src/espeak phsource/phonemes.stamp dictsource/zh_list dictsource/zh_rules dictsource/zh_extra dictsource/zh_listx
else
espeak-data/zh_dict: src/espeak phsource/phonemes.stamp dictsource/zh_list dictsource/zh_rules dictsource/zh_extra
endif
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=zh && cd ..

zhy: espeak-data/zhy_dict
dictsource/zhy_extra:
	touch dictsource/zhy_extra
dictsource/zhy_list:
	ln -svf extra/zhy_list dictsource/
if HAVE_ZHY_EXTENDED_DICTIONARY
espeak-data/zhy_dict: src/espeak phsource/phonemes.stamp dictsource/zhy_rules dictsource/zhy_extra dictsource/zhy_list
else
espeak-data/zhy_dict: src/espeak phsource/phonemes.stamp dictsource/zhy_rules dictsource/zhy_extra
endif
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=zhy && cd ..

##### mbrola:

mbrola: \
	espeak-data/mbrola_ph/af1_phtrans \
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

espeak-data/mbrola_ph/af1_phtrans: phsource/mbrola/af1 src/espeakedit
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeakedit --compile-mbrola ${PWD}/$<

espeak-data/mbrola_ph/cr1_phtrans: phsource/mbrola/cr1 src/espeakedit
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeakedit --compile-mbrola ${PWD}/$<

espeak-data/mbrola_ph/cs_phtrans: phsource/mbrola/cs src/espeakedit
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeakedit --compile-mbrola ${PWD}/$<

espeak-data/mbrola_ph/de2_phtrans: phsource/mbrola/de2 src/espeakedit
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeakedit --compile-mbrola ${PWD}/$<

espeak-data/mbrola_ph/de4_phtrans: phsource/mbrola/de4 src/espeakedit
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeakedit --compile-mbrola ${PWD}/$<

espeak-data/mbrola_ph/de6_phtrans: phsource/mbrola/de6 src/espeakedit
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeakedit --compile-mbrola ${PWD}/$<

espeak-data/mbrola_ph/ee1_phtrans: phsource/mbrola/ee1 src/espeakedit
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeakedit --compile-mbrola ${PWD}/$<

espeak-data/mbrola_ph/en1_phtrans: phsource/mbrola/en1 src/espeakedit
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeakedit --compile-mbrola ${PWD}/$<

espeak-data/mbrola_ph/es_phtrans: phsource/mbrola/es src/espeakedit
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeakedit --compile-mbrola ${PWD}/$<

espeak-data/mbrola_ph/fr1_phtrans: phsource/mbrola/fr1 src/espeakedit
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeakedit --compile-mbrola ${PWD}/$<

espeak-data/mbrola_ph/gr2_phtrans: phsource/mbrola/gr2 src/espeakedit
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeakedit --compile-mbrola ${PWD}/$<

espeak-data/mbrola_ph/grc-de6_phtrans: phsource/mbrola/grc-de6 src/espeakedit
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeakedit --compile-mbrola ${PWD}/$<

espeak-data/mbrola_ph/hn1_phtrans: phsource/mbrola/hn1 src/espeakedit
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeakedit --compile-mbrola ${PWD}/$<

espeak-data/mbrola_ph/hu1_phtrans: phsource/mbrola/hu1 src/espeakedit
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeakedit --compile-mbrola ${PWD}/$<

espeak-data/mbrola_ph/ic1_phtrans: phsource/mbrola/ic1 src/espeakedit
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeakedit --compile-mbrola ${PWD}/$<

espeak-data/mbrola_ph/id1_phtrans: phsource/mbrola/id1 src/espeakedit
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeakedit --compile-mbrola ${PWD}/$<

espeak-data/mbrola_ph/in1_phtrans: phsource/mbrola/in1 src/espeakedit
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeakedit --compile-mbrola ${PWD}/$<

espeak-data/mbrola_ph/ir1_phtrans: phsource/mbrola/ir1 src/espeakedit
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeakedit --compile-mbrola ${PWD}/$<

espeak-data/mbrola_ph/it3_phtrans: phsource/mbrola/it3 src/espeakedit
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeakedit --compile-mbrola ${PWD}/$<

espeak-data/mbrola_ph/la1_phtrans: phsource/mbrola/la1 src/espeakedit
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeakedit --compile-mbrola ${PWD}/$<

espeak-data/mbrola_ph/lt1_phtrans: phsource/mbrola/lt1 src/espeakedit
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeakedit --compile-mbrola ${PWD}/$<

espeak-data/mbrola_ph/lt2_phtrans: phsource/mbrola/lt2 src/espeakedit
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeakedit --compile-mbrola ${PWD}/$<

espeak-data/mbrola_ph/mx1_phtrans: phsource/mbrola/mx1 src/espeakedit
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeakedit --compile-mbrola ${PWD}/$<

espeak-data/mbrola_ph/mx2_phtrans: phsource/mbrola/mx2 src/espeakedit
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeakedit --compile-mbrola ${PWD}/$<

espeak-data/mbrola_ph/nl_phtrans: phsource/mbrola/nl src/espeakedit
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeakedit --compile-mbrola ${PWD}/$<

espeak-data/mbrola_ph/pl1_phtrans: phsource/mbrola/pl1 src/espeakedit
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeakedit --compile-mbrola ${PWD}/$<

espeak-data/mbrola_ph/pt1_phtrans: phsource/mbrola/pt1 src/espeakedit
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeakedit --compile-mbrola ${PWD}/$<

espeak-data/mbrola_ph/ptbr_phtrans: phsource/mbrola/ptbr src/espeakedit
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeakedit --compile-mbrola ${PWD}/$<

espeak-data/mbrola_ph/ptbr4_phtrans: phsource/mbrola/ptbr4 src/espeakedit
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeakedit --compile-mbrola ${PWD}/$<

espeak-data/mbrola_ph/ro1_phtrans: phsource/mbrola/ro1 src/espeakedit
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeakedit --compile-mbrola ${PWD}/$<

espeak-data/mbrola_ph/sv_phtrans: phsource/mbrola/sv src/espeakedit
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeakedit --compile-mbrola ${PWD}/$<

espeak-data/mbrola_ph/sv2_phtrans: phsource/mbrola/sv2 src/espeakedit
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeakedit --compile-mbrola ${PWD}/$<

espeak-data/mbrola_ph/tr1_phtrans: phsource/mbrola/tr1 src/espeakedit
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeakedit --compile-mbrola ${PWD}/$<

espeak-data/mbrola_ph/us_phtrans: phsource/mbrola/us src/espeakedit
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeakedit --compile-mbrola ${PWD}/$<

espeak-data/mbrola_ph/us3_phtrans: phsource/mbrola/us3 src/espeakedit
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeakedit --compile-mbrola ${PWD}/$<

espeak-data/mbrola_ph/vz_phtrans: phsource/mbrola/vz src/espeakedit
	mkdir -p espeak-data/mbrola_ph
	ESPEAK_DATA_PATH=$(PWD) src/espeakedit --compile-mbrola ${PWD}/$<
