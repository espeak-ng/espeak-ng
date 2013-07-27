PREFIX=@prefix@
BINDIR=@bindir@
INCDIR=@includedir@/espeak
LIBDIR=@libdir@
DATADIR=$(PREFIX)/share/espeak-data

PLATFORM=big_endian

.PHONY: android all clean distclean espeak-phoneme-data

EXTRA_DIST=

##### ChangeLog:

ChangeLog:
	git log > ChangeLog

dist-hook: ChangeLog

.PHONY: ChangeLog

EXTRA_DIST += ChangeLog

##### standard build actions:

all: jni espeakdata apk-release

install:
	cd src && make DESTDIR=$(DESTDIR) PREFIX=$(PREFIX) BINDIR=$(BINDIR) INCDIR=$(INCDIR) LIBDIR=$(LIBDIR) install && cd ..
	install -m 755 src/espeakedit $(DESTDIR)$(BINDIR)

clean:
	cd src && rm -f *.o *~ && cd ..
	cd android && gradle clean

distclean: clean
	cd src && rm -f libespeak.a libespeak.so.* speak espeak espeakedit && cd ..
	cd platforms/$(PLATFORM) && rm -f espeak-phoneme-data && cd ../..
	rm -rf espeak-data/dictsource espeak-data/phsource espeak-data/phondata-manifest
	cd espeak-data && rm -f *_dict && cd ..

##### build targets:

common_SOURCE = \
	src/compiledict.cpp \
	src/dictionary.cpp \
	src/intonation.cpp \
	src/klatt.cpp \
	src/mbrowrap.cpp \
	src/numbers.cpp \
	src/readclause.cpp \
	src/phonemelist.cpp \
	src/setlengths.cpp \
	src/sonic.cpp \
	src/synthdata.cpp \
	src/synthesize.cpp \
	src/synth_mbrola.cpp \
	src/translate.cpp \
	src/tr_languages.cpp \
	src/voices.cpp \
	src/wavegen.cpp 

libespeak_SOURCE = \
	src/speak_lib.cpp \
	src/espeak_command.cpp \
	src/event.cpp \
	src/fifo.cpp \
	src/wave.cpp \
	src/wave_pulse.cpp \
	src/wave_sada.cpp \
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

docs/speak_lib.h: src/speak_lib.h
	cp $< $@

src/libespeak.a: $(common_SOURCE) $(libespeak_SOURCE)
	cd src && make libespeak.a && cd ..

src/libespeak.so: $(common_SOURCE) $(libespeak_SOURCE)
	cd src && make libespeak.so && cd ..

src/speak: $(common_SOURCE) src/speak.cpp
	cd src && make speak && cd ..

src/espeak: src/libespeak.so src/espeak.cpp
	cd src && make espeak && cd ..

src/espeakedit: $(common_SOURCE) $(libespeak_SOURCE) $(espeakedit_SOURCE)
	cd src && make espeakedit && cd ..

espeak-phoneme-data:
	cd platforms/$(PLATFORM) && make PREFIX=$(PREFIX) && cd ../..

espeak-data/dictsource/dir.stamp: dictsource/*
	rm -rf espeak-data/dictsource
	./shadowdir $(PWD)/dictsource $(PWD)/espeak-data/dictsource
	touch espeak-data/dictsource/dir.stamp

espeak-data/phsource/dir.stamp: phsource/ph_* phsource/phonemes phsource/intonation
	rm -rf espeak-data/phsource
	./shadowdir $(PWD)/phsource $(PWD)/espeak-data/phsource
	touch espeak-data/phsource/dir.stamp

espeak-data/phontab: src/espeakedit espeak-data/dictsource/dir.stamp espeak-data/phsource/dir.stamp
	ESPEAK_DATA_PATH=$(PWD) src/espeakedit --compile

##### android targets:

jni:
	cd android && ndk-build

apk-release:
	cd android && gradle assembleRelease

apk-debug:
	cd android && gradle assembleDebug

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
	espeak-data/fa_dict \
	espeak-data/fi_dict \
	espeak-data/fr_dict \
	espeak-data/ga_dict \
	espeak-data/grc_dict \
	espeak-data/hi_dict \
	espeak-data/hu_dict \
	espeak-data/hy_dict \
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
	espeak-data/lt_dict \
	espeak-data/lv_dict \
	espeak-data/mk_dict \
	espeak-data/ml_dict \
	espeak-data/ms_dict \
	espeak-data/nci_dict \
	espeak-data/ne_dict \
	espeak-data/nl_dict \
	espeak-data/no_dict \
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
espeak-data/af_dict: src/espeak espeak-data/phontab dictsource/af_list dictsource/af_rules dictsource/af_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=af && cd ..

am: espeak-data/am_dict
dictsource/am_extra:
	touch dictsource/am_extra
espeak-data/am_dict: src/espeak espeak-data/phontab dictsource/am_list dictsource/am_rules dictsource/am_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=am && cd ..

an: espeak-data/an_dict
dictsource/an_extra:
	touch dictsource/an_extra
espeak-data/an_dict: src/espeak espeak-data/phontab dictsource/an_list dictsource/an_rules dictsource/an_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=an && cd ..

az: espeak-data/az_dict
dictsource/az_extra:
	touch dictsource/az_extra
espeak-data/az_dict: src/espeak espeak-data/phontab dictsource/az_list dictsource/az_rules dictsource/az_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=az && cd ..

bg: espeak-data/bg_dict
dictsource/bg_extra:
	touch dictsource/bg_extra
espeak-data/bg_dict: src/espeak espeak-data/phontab dictsource/bg_list dictsource/bg_listx dictsource/bg_rules dictsource/bg_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=bg && cd ..

bn: espeak-data/bn_dict
dictsource/bn_extra:
	touch dictsource/bn_extra
espeak-data/bn_dict: src/espeak espeak-data/phontab dictsource/bn_list dictsource/bn_rules dictsource/bn_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=bn && cd ..

ca: espeak-data/ca_dict
dictsource/ca_extra:
	touch dictsource/ca_extra
espeak-data/ca_dict: src/espeak espeak-data/phontab dictsource/ca_list dictsource/ca_rules dictsource/ca_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=ca && cd ..

cs: espeak-data/cs_dict
dictsource/cs_extra:
	touch dictsource/cs_extra
espeak-data/cs_dict: src/espeak espeak-data/phontab dictsource/cs_list dictsource/cs_rules dictsource/cs_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=cs && cd ..

cy: espeak-data/cy_dict
dictsource/cy_extra:
	touch dictsource/cy_extra
espeak-data/cy_dict: src/espeak espeak-data/phontab dictsource/cy_list dictsource/cy_rules dictsource/cy_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=cy && cd ..

da: espeak-data/da_dict
dictsource/da_extra:
	touch dictsource/da_extra
espeak-data/da_dict: src/espeak espeak-data/phontab dictsource/da_list dictsource/da_rules dictsource/da_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=da && cd ..

de: espeak-data/de_dict
dictsource/de_extra:
	touch dictsource/de_extra
espeak-data/de_dict: src/espeak espeak-data/phontab dictsource/de_list dictsource/de_rules dictsource/de_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=de && cd ..

el: espeak-data/el_dict
dictsource/el_extra:
	touch dictsource/el_extra
espeak-data/el_dict: src/espeak espeak-data/phontab dictsource/el_list dictsource/el_rules dictsource/el_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=el && cd ..

en: espeak-data/en_dict
dictsource/en_extra:
	touch dictsource/en_extra
espeak-data/en_dict: src/espeak espeak-data/phontab dictsource/en_list dictsource/en_rules dictsource/en_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=en && cd ..

eo: espeak-data/eo_dict
dictsource/eo_extra:
	touch dictsource/eo_extra
espeak-data/eo_dict: src/espeak espeak-data/phontab dictsource/eo_list dictsource/eo_rules dictsource/eo_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=eo && cd ..

es: espeak-data/es_dict
dictsource/es_extra:
	touch dictsource/es_extra
espeak-data/es_dict: src/espeak espeak-data/phontab dictsource/es_list dictsource/es_rules dictsource/es_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=es && cd ..

et: espeak-data/et_dict
dictsource/et_extra:
	touch dictsource/et_extra
espeak-data/et_dict: src/espeak espeak-data/phontab dictsource/et_list dictsource/et_rules dictsource/et_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=et && cd ..

fa: espeak-data/fa_dict
dictsource/fa_extra:
	touch dictsource/fa_extra
espeak-data/fa_dict: src/espeak espeak-data/phontab dictsource/fa_list dictsource/fa_rules dictsource/fa_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=fa && cd ..

fi: espeak-data/fi_dict
dictsource/fi_extra:
	touch dictsource/fi_extra
espeak-data/fi_dict: src/espeak espeak-data/phontab dictsource/fi_list dictsource/fi_rules dictsource/fi_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=fi && cd ..

fr: espeak-data/fr_dict
dictsource/fr_extra:
	touch dictsource/fr_extra
espeak-data/fr_dict: src/espeak espeak-data/phontab dictsource/fr_list dictsource/fr_rules dictsource/fr_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=fr && cd ..

ga: espeak-data/ga_dict
dictsource/ga_extra:
	touch dictsource/ga_extra
espeak-data/ga_dict: src/espeak espeak-data/phontab dictsource/ga_list dictsource/ga_rules dictsource/ga_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=ga && cd ..

grc: espeak-data/grc_dict
dictsource/grc_extra:
	touch dictsource/grc_extra
espeak-data/grc_dict: src/espeak espeak-data/phontab dictsource/grc_list dictsource/grc_rules dictsource/grc_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=grc && cd ..

hi: espeak-data/hi_dict
dictsource/hi_extra:
	touch dictsource/hi_extra
espeak-data/hi_dict: src/espeak espeak-data/phontab dictsource/hi_list dictsource/hi_rules dictsource/hi_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=hi && cd ..

hu: espeak-data/hu_dict
dictsource/hu_extra:
	touch dictsource/hu_extra
espeak-data/hu_dict: src/espeak espeak-data/phontab dictsource/hu_list dictsource/hu_rules dictsource/hu_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=hu && cd ..

hy: espeak-data/hy_dict
dictsource/hy_extra:
	touch dictsource/hy_extra
espeak-data/hy_dict: src/espeak espeak-data/phontab dictsource/hy_list dictsource/hy_rules dictsource/hy_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=hy && cd ..

id: espeak-data/id_dict
dictsource/id_extra:
	touch dictsource/id_extra
espeak-data/id_dict: src/espeak espeak-data/phontab dictsource/id_list dictsource/id_rules dictsource/id_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=id && cd ..

is: espeak-data/is_dict
dictsource/is_extra:
	touch dictsource/is_extra
espeak-data/is_dict: src/espeak espeak-data/phontab dictsource/is_list dictsource/is_rules dictsource/is_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=is && cd ..

it: espeak-data/it_dict
dictsource/it_extra:
	touch dictsource/it_extra
espeak-data/it_dict: src/espeak espeak-data/phontab dictsource/it_list dictsource/it_listx dictsource/it_rules dictsource/it_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=it && cd ..

jbo: espeak-data/jbo_dict
dictsource/jbo_extra:
	touch dictsource/jbo_extra
espeak-data/jbo_dict: src/espeak espeak-data/phontab dictsource/jbo_list dictsource/jbo_rules dictsource/jbo_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=jbo && cd ..

ka: espeak-data/ka_dict
dictsource/ka_extra:
	touch dictsource/ka_extra
espeak-data/ka_dict: src/espeak espeak-data/phontab dictsource/ka_list dictsource/ka_rules dictsource/ka_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=ka && cd ..

kl: espeak-data/kl_dict
dictsource/kl_extra:
	touch dictsource/kl_extra
espeak-data/kl_dict: src/espeak espeak-data/phontab dictsource/kl_list dictsource/kl_rules dictsource/kl_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=kl && cd ..

kn: espeak-data/kn_dict
dictsource/kn_extra:
	touch dictsource/kn_extra
espeak-data/kn_dict: src/espeak espeak-data/phontab dictsource/kn_list dictsource/kn_rules dictsource/kn_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=kn && cd ..

ko: espeak-data/ko_dict
dictsource/ko_extra:
	touch dictsource/ko_extra
espeak-data/ko_dict: src/espeak espeak-data/phontab dictsource/ko_list dictsource/ko_rules dictsource/ko_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=ko && cd ..

ku: espeak-data/ku_dict
dictsource/ku_extra:
	touch dictsource/ku_extra
espeak-data/ku_dict: src/espeak espeak-data/phontab dictsource/ku_list dictsource/ku_rules dictsource/ku_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=ku && cd ..

la: espeak-data/la_dict
dictsource/la_extra:
	touch dictsource/la_extra
espeak-data/la_dict: src/espeak espeak-data/phontab dictsource/la_list dictsource/la_rules dictsource/la_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=la && cd ..

lt: espeak-data/lt_dict
dictsource/lt_extra:
	touch dictsource/lt_extra
espeak-data/lt_dict: src/espeak espeak-data/phontab dictsource/lt_list dictsource/lt_rules dictsource/lt_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=lt && cd ..

lv: espeak-data/lv_dict
dictsource/lv_extra:
	touch dictsource/lv_extra
espeak-data/lv_dict: src/espeak espeak-data/phontab dictsource/lv_list dictsource/lv_rules dictsource/lv_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=lv && cd ..

mk: espeak-data/mk_dict
dictsource/mk_extra:
	touch dictsource/mk_extra
espeak-data/mk_dict: src/espeak espeak-data/phontab dictsource/mk_list dictsource/mk_rules dictsource/mk_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=mk && cd ..

ml: espeak-data/ml_dict
dictsource/ml_extra:
	touch dictsource/ml_extra
espeak-data/ml_dict: src/espeak espeak-data/phontab dictsource/ml_list dictsource/ml_rules dictsource/ml_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=ml && cd ..

ms: espeak-data/ms_dict
dictsource/ms_extra:
	touch dictsource/ms_extra
espeak-data/ms_dict: src/espeak espeak-data/phontab dictsource/ms_list dictsource/ms_rules dictsource/ms_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=ms && cd ..

nci: espeak-data/nci_dict
dictsource/nci_extra:
	touch dictsource/nci_extra
espeak-data/nci_dict: src/espeak espeak-data/phontab dictsource/nci_list dictsource/nci_rules dictsource/nci_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=nci && cd ..

ne: espeak-data/ne_dict
dictsource/ne_extra:
	touch dictsource/ne_extra
espeak-data/ne_dict: src/espeak espeak-data/phontab dictsource/ne_list dictsource/ne_rules dictsource/ne_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=ne && cd ..

nl: espeak-data/nl_dict
dictsource/nl_extra:
	touch dictsource/nl_extra
espeak-data/nl_dict: src/espeak espeak-data/phontab dictsource/nl_list dictsource/nl_rules dictsource/nl_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=nl && cd ..

no: espeak-data/no_dict
dictsource/no_extra:
	touch dictsource/no_extra
espeak-data/no_dict: src/espeak espeak-data/phontab dictsource/no_list dictsource/no_rules dictsource/no_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=no && cd ..

pa: espeak-data/pa_dict
dictsource/pa_extra:
	touch dictsource/pa_extra
espeak-data/pa_dict: src/espeak espeak-data/phontab dictsource/pa_list dictsource/pa_rules dictsource/pa_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=pa && cd ..

pap: espeak-data/pap_dict
dictsource/pap_extra:
	touch dictsource/pap_extra
espeak-data/pap_dict: src/espeak espeak-data/phontab dictsource/pap_list dictsource/pap_rules dictsource/pap_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=pap && cd ..

pl: espeak-data/pl_dict
dictsource/pl_extra:
	touch dictsource/pl_extra
espeak-data/pl_dict: src/espeak espeak-data/phontab dictsource/pl_list dictsource/pl_rules dictsource/pl_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=pl && cd ..

pt: espeak-data/pt_dict
dictsource/pt_extra:
	touch dictsource/pt_extra
espeak-data/pt_dict: src/espeak espeak-data/phontab dictsource/pt_list dictsource/pt_rules dictsource/pt_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=pt && cd ..

ro: espeak-data/ro_dict
dictsource/ro_extra:
	touch dictsource/ro_extra
espeak-data/ro_dict: src/espeak espeak-data/phontab dictsource/ro_list dictsource/ro_rules dictsource/ro_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=ro && cd ..

ru: espeak-data/ru_dict
dictsource/ru_extra:
	touch dictsource/ru_extra
espeak-data/ru_dict: src/espeak espeak-data/phontab dictsource/ru_list dictsource/ru_rules dictsource/ru_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=ru && cd ..

si: espeak-data/si_dict
dictsource/si_extra:
	touch dictsource/si_extra
espeak-data/si_dict: src/espeak espeak-data/phontab dictsource/si_list dictsource/si_rules dictsource/si_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=si && cd ..

sk: espeak-data/sk_dict
dictsource/sk_extra:
	touch dictsource/sk_extra
espeak-data/sk_dict: src/espeak espeak-data/phontab dictsource/sk_list dictsource/sk_rules dictsource/sk_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=sk && cd ..

sl: espeak-data/sl_dict
dictsource/sl_extra:
	touch dictsource/sl_extra
espeak-data/sl_dict: src/espeak espeak-data/phontab dictsource/sl_list dictsource/sl_rules dictsource/sl_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=sl && cd ..

sq: espeak-data/sq_dict
dictsource/sq_extra:
	touch dictsource/sq_extra
espeak-data/sq_dict: src/espeak espeak-data/phontab dictsource/sq_list dictsource/sq_rules dictsource/sq_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=sq && cd ..

sv: espeak-data/sv_dict
dictsource/sv_extra:
	touch dictsource/sv_extra
espeak-data/sv_dict: src/espeak espeak-data/phontab dictsource/sv_list dictsource/sv_rules dictsource/sv_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=sv && cd ..

sw: espeak-data/sw_dict
dictsource/sw_extra:
	touch dictsource/sw_extra
espeak-data/sw_dict: src/espeak espeak-data/phontab dictsource/sw_list dictsource/sw_rules dictsource/sw_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=sw && cd ..

ta: espeak-data/ta_dict
dictsource/ta_extra:
	touch dictsource/ta_extra
espeak-data/ta_dict: src/espeak espeak-data/phontab dictsource/ta_list dictsource/ta_rules dictsource/ta_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=ta && cd ..

te: espeak-data/te_dict
dictsource/te_extra:
	touch dictsource/te_extra
espeak-data/te_dict: src/espeak espeak-data/phontab dictsource/te_list dictsource/te_rules dictsource/te_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=te && cd ..

tr: espeak-data/tr_dict
dictsource/tr_extra:
	touch dictsource/tr_extra
espeak-data/tr_dict: src/espeak espeak-data/phontab dictsource/tr_list dictsource/tr_rules dictsource/tr_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=tr && cd ..

ur: espeak-data/ur_dict
dictsource/ur_extra:
	touch dictsource/ur_extra
espeak-data/ur_dict: src/espeak espeak-data/phontab dictsource/ur_list dictsource/ur_rules dictsource/ur_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=ur && cd ..

vi: espeak-data/vi_dict
dictsource/vi_extra:
	touch dictsource/vi_extra
espeak-data/vi_dict: src/espeak espeak-data/phontab dictsource/vi_list dictsource/vi_rules dictsource/vi_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=vi && cd ..

zh: espeak-data/zh_dict
dictsource/zh_extra:
	touch dictsource/zh_extra
espeak-data/zh_dict: src/espeak espeak-data/phontab dictsource/zh_list dictsource/zh_rules dictsource/zh_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=zh && cd ..

zhy: espeak-data/zhy_dict
dictsource/zhy_extra:
	touch dictsource/zhy_extra
espeak-data/zhy_dict: src/espeak espeak-data/phontab dictsource/zhy_rules dictsource/zhy_extra
	cd dictsource && ESPEAK_DATA_PATH=$(PWD) LD_LIBRARY_PATH=../src:${LD_LIBRARY_PATH} ../src/espeak --compile=zhy && cd ..
