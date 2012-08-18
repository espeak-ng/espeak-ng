REFIX=/usr
BINDIR=$(PREFIX)/bin
INCDIR=$(PREFIX)/include/espeak
LIBDIR=$(PREFIX)/lib
DATADIR=$(PREFIX)/share/espeak-data

PLATFORM=big_endian
AUDIO=@AUDIO@

.PHONY: all clean distclean espeak-phoneme-data

EXTRA_DIST=

##### ChangeLog:

ChangeLog:
	git log > ChangeLog

dist-hook: ChangeLog

.PHONY: ChangeLog

EXTRA_DIST += ChangeLog

##### standard build actions:

all: src/speak src/libespeak.so src/libespeak.a src/espeak src/espeakedit espeak-data/phontab dictionaries docs/speak_lib.h

install: all
	cd src && make DESTDIR=$(DESTDIR) PREFIX=$(PREFIX) BINDIR=$(BINDIR) INCDIR=$(INCDIR) LIBDIR=$(LIBDIR) install && cd ..
	install -m 755 src/espeakedit $(DESTDIR)$(BINDIR)

clean:
	cd src && rm -f *.o *~ && cd ..

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
	cd src && make libespeak.a AUDIO=${AUDIO} PREFIX=$(PREFIX) && cd ..

src/libespeak.so: $(common_SOURCE) $(libespeak_SOURCE)
	cd src && make libespeak.so AUDIO=${AUDIO} PREFIX=$(PREFIX) && cd ..

src/speak: $(common_SOURCE) src/speak.cpp
	cd src && make speak AUDIO=${AUDIO} PREFIX=$(PREFIX) && cd ..

src/espeak: src/libespeak.so src/espeak.cpp
	cd src && make espeak AUDIO=${AUDIO} PREFIX=$(PREFIX) && cd ..

src/espeakedit: $(common_SOURCE) $(libespeak_SOURCE) $(espeakedit_SOURCE)
	cd src && make espeakedit AUDIO=${AUDIO} PREFIX=$(PREFIX) && cd ..

espeak-phoneme-data:
	cd platforms/$(PLATFORM) && make PREFIX=$(PREFIX) && cd ../..

espeak-data/dir.stamp:
	rm -rf $(HOME)/espeak-data
	ln -sv $(PWD)/espeak-data $(HOME)/espeak-data
	touch espeak-data/dir.stamp

espeak-data/dictsource/dir.stamp: dictsource/*
	rm -rf espeak-data/dictsource
	./shadowdir $(PWD)/dictsource $(PWD)/espeak-data/dictsource
	touch espeak-data/dictsource/dir.stamp

espeak-data/phsource/dir.stamp: phsource/ph_* phsource/phonemes phsource/intonation
	rm -rf espeak-data/phsource
	./shadowdir $(PWD)/phsource $(PWD)/espeak-data/phsource
	touch espeak-data/phsource/dir.stamp

espeak-data/phontab: src/espeakedit espeak-data/dir.stamp espeak-data/dictsource/dir.stamp espeak-data/phsource/dir.stamp
	src/espeakedit --compile

##### dictionaries:

dictionaries: \
	espeak-data/af_dict \
	espeak-data/ak_dict \
	espeak-data/am_dict \
	espeak-data/az_dict \
	espeak-data/bg_dict \
	espeak-data/ca_dict \
	espeak-data/cs_dict \
	espeak-data/cy_dict \
	espeak-data/da_dict \
	espeak-data/de_dict \
	espeak-data/dv_dict \
	espeak-data/el_dict \
	espeak-data/en_dict \
	espeak-data/eo_dict \
	espeak-data/es_dict \
	espeak-data/et_dict \
	espeak-data/fi_dict \
	espeak-data/fr_dict \
	espeak-data/grc_dict \
	espeak-data/hbs_dict \
	espeak-data/hi_dict \
	espeak-data/ht_dict \
	espeak-data/hu_dict \
	espeak-data/hy_dict \
	espeak-data/id_dict \
	espeak-data/is_dict \
	espeak-data/it_dict \
	espeak-data/jbo_dict \
	espeak-data/ka_dict \
	espeak-data/kk_dict \
	espeak-data/kn_dict \
	espeak-data/ku_dict \
	espeak-data/la_dict \
	espeak-data/lt_dict \
	espeak-data/lv_dict \
	espeak-data/mk_dict \
	espeak-data/ml_dict \
	espeak-data/mt_dict \
	espeak-data/nci_dict \
	espeak-data/ne_dict \
	espeak-data/nl_dict \
	espeak-data/no_dict \
	espeak-data/nso_dict \
	espeak-data/pa_dict \
	espeak-data/pap_dict \
	espeak-data/pl_dict \
	espeak-data/prs_dict \
	espeak-data/pt_dict \
	espeak-data/ro_dict \
	espeak-data/ru_dict \
	espeak-data/rw_dict \
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
	espeak-data/ur_dict \
	espeak-data/vi_dict \
	espeak-data/wo_dict \
	espeak-data/zh_dict \
	espeak-data/zhy_dict

af: espeak-data/af_dict
dictsource/af_extra:
	touch dictsource/af_extra
espeak-data/af_dict: dictsource/af_list dictsource/af_rules dictsource/af_extra
	cd dictsource && ../src/espeak --compile=af && cd ..

ak: espeak-data/ak_dict
dictsource/ak_extra:
	touch dictsource/ak_extra
espeak-data/ak_dict: dictsource/ak_rules dictsource/ak_extra
	cd dictsource && ../src/espeak --compile=ak && cd ..

am: espeak-data/am_dict
dictsource/am_extra:
	touch dictsource/am_extra
espeak-data/am_dict: dictsource/am_list dictsource/am_rules dictsource/am_extra
	cd dictsource && ../src/espeak --compile=am && cd ..

az: espeak-data/az_dict
dictsource/az_extra:
	touch dictsource/az_extra
espeak-data/az_dict: dictsource/az_list dictsource/az_rules dictsource/az_extra
	cd dictsource && ../src/espeak --compile=az && cd ..

bg: espeak-data/bg_dict
dictsource/bg_extra:
	touch dictsource/bg_extra
espeak-data/bg_dict: dictsource/bg_list dictsource/bg_listx dictsource/bg_rules dictsource/bg_extra
	cd dictsource && ../src/espeak --compile=bg && cd ..

bo: espeak-data/bo_dict
dictsource/bo_extra:
	touch dictsource/bo_extra
espeak-data/bo_dict: dictsource/bo_rules dictsource/bo_extra
	cd dictsource && ../src/espeak --compile=bo && cd ..

ca: espeak-data/ca_dict
dictsource/ca_extra:
	touch dictsource/ca_extra
espeak-data/ca_dict: dictsource/ca_list dictsource/ca_rules dictsource/ca_extra
	cd dictsource && ../src/espeak --compile=ca && cd ..

cs: espeak-data/cs_dict
dictsource/cs_extra:
	touch dictsource/cs_extra
espeak-data/cs_dict: dictsource/cs_list dictsource/cs_rules dictsource/cs_extra
	cd dictsource && ../src/espeak --compile=cs && cd ..

cy: espeak-data/cy_dict
dictsource/cy_extra:
	touch dictsource/cy_extra
espeak-data/cy_dict: dictsource/cy_list dictsource/cy_rules dictsource/cy_extra
	cd dictsource && ../src/espeak --compile=cy && cd ..

da: espeak-data/da_dict
dictsource/da_extra:
	touch dictsource/da_extra
espeak-data/da_dict: dictsource/da_list dictsource/da_rules dictsource/da_extra
	cd dictsource && ../src/espeak --compile=da && cd ..

de: espeak-data/de_dict
dictsource/de_extra:
	touch dictsource/de_extra
espeak-data/de_dict: dictsource/de_list dictsource/de_rules dictsource/de_extra
	cd dictsource && ../src/espeak --compile=de && cd ..

dv: espeak-data/dv_dict
dictsource/dv_extra:
	touch dictsource/dv_extra
espeak-data/dv_dict: dictsource/dv_list dictsource/dv_rules dictsource/dv_extra
	cd dictsource && ../src/espeak --compile=dv && cd ..

el: espeak-data/el_dict
dictsource/el_extra:
	touch dictsource/el_extra
espeak-data/el_dict: dictsource/el_list dictsource/el_rules dictsource/el_extra
	cd dictsource && ../src/espeak --compile=el && cd ..

en: espeak-data/en_dict
dictsource/en_extra:
	touch dictsource/en_extra
espeak-data/en_dict: dictsource/en_list dictsource/en_rules dictsource/en_extra
	cd dictsource && ../src/espeak --compile=en && cd ..

eo: espeak-data/eo_dict
dictsource/eo_extra:
	touch dictsource/eo_extra
espeak-data/eo_dict: dictsource/eo_list dictsource/eo_rules dictsource/eo_extra
	cd dictsource && ../src/espeak --compile=eo && cd ..

es: espeak-data/es_dict
dictsource/es_extra:
	touch dictsource/es_extra
espeak-data/es_dict: dictsource/es_list dictsource/es_rules dictsource/es_extra
	cd dictsource && ../src/espeak --compile=es && cd ..

et: espeak-data/et_dict
dictsource/et_extra:
	touch dictsource/et_extra
espeak-data/et_dict: dictsource/et_list dictsource/et_rules dictsource/et_extra
	cd dictsource && ../src/espeak --compile=et && cd ..

fi: espeak-data/fi_dict
dictsource/fi_extra:
	touch dictsource/fi_extra
espeak-data/fi_dict: dictsource/fi_list dictsource/fi_rules dictsource/fi_extra
	cd dictsource && ../src/espeak --compile=fi && cd ..

fr: espeak-data/fr_dict
dictsource/fr_extra:
	touch dictsource/fr_extra
espeak-data/fr_dict: dictsource/fr_list dictsource/fr_rules dictsource/fr_extra
	cd dictsource && ../src/espeak --compile=fr && cd ..

grc: espeak-data/grc_dict
dictsource/grc_extra:
	touch dictsource/grc_extra
espeak-data/grc_dict: dictsource/grc_list dictsource/grc_rules dictsource/grc_extra
	cd dictsource && ../src/espeak --compile=grc && cd ..

hbs: espeak-data/hbs_dict
dictsource/hbs_extra:
	touch dictsource/hbs_extra
espeak-data/hbs_dict: dictsource/hbs_list dictsource/hbs_rules dictsource/hbs_extra
	cd dictsource && ../src/espeak --compile=hbs && cd ..

hi: espeak-data/hi_dict
dictsource/hi_extra:
	touch dictsource/hi_extra
espeak-data/hi_dict: dictsource/hi_list dictsource/hi_rules dictsource/hi_extra
	cd dictsource && ../src/espeak --compile=hi && cd ..

ht: espeak-data/ht_dict
dictsource/ht_extra:
	touch dictsource/ht_extra
espeak-data/ht_dict: dictsource/ht_list dictsource/ht_rules dictsource/ht_extra
	cd dictsource && ../src/espeak --compile=ht && cd ..

hu: espeak-data/hu_dict
dictsource/hu_extra:
	touch dictsource/hu_extra
espeak-data/hu_dict: dictsource/hu_list dictsource/hu_rules dictsource/hu_extra
	cd dictsource && ../src/espeak --compile=hu && cd ..

hy: espeak-data/hy_dict
dictsource/hy_extra:
	touch dictsource/hy_extra
espeak-data/hy_dict: dictsource/hy_list dictsource/hy_rules dictsource/hy_extra
	cd dictsource && ../src/espeak --compile=hy && cd ..

id: espeak-data/id_dict
dictsource/id_extra:
	touch dictsource/id_extra
espeak-data/id_dict: dictsource/id_list dictsource/id_rules dictsource/id_extra
	cd dictsource && ../src/espeak --compile=id && cd ..

is: espeak-data/is_dict
dictsource/is_extra:
	touch dictsource/is_extra
espeak-data/is_dict: dictsource/is_list dictsource/is_rules dictsource/is_extra
	cd dictsource && ../src/espeak --compile=is && cd ..

it: espeak-data/it_dict
dictsource/it_extra:
	touch dictsource/it_extra
espeak-data/it_dict: dictsource/it_list dictsource/it_listx dictsource/it_rules dictsource/it_extra
	cd dictsource && ../src/espeak --compile=it && cd ..

jbo: espeak-data/jbo_dict
dictsource/jbo_extra:
	touch dictsource/jbo_extra
espeak-data/jbo_dict: dictsource/jbo_list dictsource/jbo_rules dictsource/jbo_extra
	cd dictsource && ../src/espeak --compile=jbo && cd ..

ka: espeak-data/ka_dict
dictsource/ka_extra:
	touch dictsource/ka_extra
espeak-data/ka_dict: dictsource/ka_list dictsource/ka_rules dictsource/ka_extra
	cd dictsource && ../src/espeak --compile=ka && cd ..

kk: espeak-data/kk_dict
dictsource/kk_extra:
	touch dictsource/kk_extra
espeak-data/kk_dict: dictsource/kk_list dictsource/kk_rules dictsource/kk_extra
	cd dictsource && ../src/espeak --compile=kk && cd ..

kn: espeak-data/kn_dict
dictsource/kn_extra:
	touch dictsource/kn_extra
espeak-data/kn_dict: dictsource/kn_list dictsource/kn_rules dictsource/kn_extra
	cd dictsource && ../src/espeak --compile=kn && cd ..

ku: espeak-data/ku_dict
dictsource/ku_extra:
	touch dictsource/ku_extra
espeak-data/ku_dict: dictsource/ku_list dictsource/ku_rules dictsource/ku_extra
	cd dictsource && ../src/espeak --compile=ku && cd ..

la: espeak-data/la_dict
dictsource/la_extra:
	touch dictsource/la_extra
espeak-data/la_dict: dictsource/la_list dictsource/la_rules dictsource/la_extra
	cd dictsource && ../src/espeak --compile=la && cd ..

lt: espeak-data/lt_dict
dictsource/lt_extra:
	touch dictsource/lt_extra
espeak-data/lt_dict: dictsource/lt_list dictsource/lt_rules dictsource/lt_extra
	cd dictsource && ../src/espeak --compile=lt && cd ..

lv: espeak-data/lv_dict
dictsource/lv_extra:
	touch dictsource/lv_extra
espeak-data/lv_dict: dictsource/lv_list dictsource/lv_rules dictsource/lv_extra
	cd dictsource && ../src/espeak --compile=lv && cd ..

mk: espeak-data/mk_dict
dictsource/mk_extra:
	touch dictsource/mk_extra
espeak-data/mk_dict: dictsource/mk_list dictsource/mk_rules dictsource/mk_extra
	cd dictsource && ../src/espeak --compile=mk && cd ..

ml: espeak-data/ml_dict
dictsource/ml_extra:
	touch dictsource/ml_extra
espeak-data/ml_dict: dictsource/ml_list dictsource/ml_rules dictsource/ml_extra
	cd dictsource && ../src/espeak --compile=ml && cd ..

mt: espeak-data/mt_dict
dictsource/mt_extra:
	touch dictsource/mt_extra
espeak-data/mt_dict: dictsource/mt_list dictsource/mt_rules dictsource/mt_extra
	cd dictsource && ../src/espeak --compile=mt && cd ..

nci: espeak-data/nci_dict
dictsource/nci_extra:
	touch dictsource/nci_extra
espeak-data/nci_dict: dictsource/nci_list dictsource/nci_rules dictsource/nci_extra
	cd dictsource && ../src/espeak --compile=nci && cd ..

ne: espeak-data/ne_dict
dictsource/ne_extra:
	touch dictsource/ne_extra
espeak-data/ne_dict: dictsource/ne_list dictsource/ne_rules dictsource/ne_extra
	cd dictsource && ../src/espeak --compile=ne && cd ..

nl: espeak-data/nl_dict
dictsource/nl_extra:
	touch dictsource/nl_extra
espeak-data/nl_dict: dictsource/nl_list dictsource/nl_rules dictsource/nl_extra
	cd dictsource && ../src/espeak --compile=nl && cd ..

no: espeak-data/no_dict
dictsource/no_extra:
	touch dictsource/no_extra
espeak-data/no_dict: dictsource/no_list dictsource/no_rules dictsource/no_extra
	cd dictsource && ../src/espeak --compile=no && cd ..

nso: espeak-data/nso_dict
dictsource/nso_extra:
	touch dictsource/nso_extra
espeak-data/nso_dict: dictsource/nso_list dictsource/nso_rules dictsource/nso_extra
	cd dictsource && ../src/espeak --compile=nso && cd ..

pa: espeak-data/pa_dict
dictsource/pa_extra:
	touch dictsource/pa_extra
espeak-data/pa_dict: dictsource/pa_list dictsource/pa_rules dictsource/pa_extra
	cd dictsource && ../src/espeak --compile=pa && cd ..

pap: espeak-data/pap_dict
dictsource/pap_extra:
	touch dictsource/pap_extra
espeak-data/pap_dict: dictsource/pap_list dictsource/pap_rules dictsource/pap_extra
	cd dictsource && ../src/espeak --compile=pap && cd ..

pl: espeak-data/pl_dict
dictsource/pl_extra:
	touch dictsource/pl_extra
espeak-data/pl_dict: dictsource/pl_list dictsource/pl_rules dictsource/pl_extra
	cd dictsource && ../src/espeak --compile=pl && cd ..

prs: espeak-data/prs_dict
dictsource/prs_extra:
	touch dictsource/prs_extra
espeak-data/prs_dict: dictsource/prs_list dictsource/prs_rules dictsource/prs_extra
	cd dictsource && ../src/espeak --compile=prs && cd ..

pt: espeak-data/pt_dict
dictsource/pt_extra:
	touch dictsource/pt_extra
espeak-data/pt_dict: dictsource/pt_list dictsource/pt_rules dictsource/pt_extra
	cd dictsource && ../src/espeak --compile=pt && cd ..

ro: espeak-data/ro_dict
dictsource/ro_extra:
	touch dictsource/ro_extra
espeak-data/ro_dict: dictsource/ro_list dictsource/ro_rules dictsource/ro_extra
	cd dictsource && ../src/espeak --compile=ro && cd ..

ru: espeak-data/ru_dict
dictsource/ru_extra:
	touch dictsource/ru_extra
espeak-data/ru_dict: dictsource/ru_list dictsource/ru_rules dictsource/ru_extra
	cd dictsource && ../src/espeak --compile=ru && cd ..

rw: espeak-data/rw_dict
dictsource/rw_extra:
	touch dictsource/rw_extra
espeak-data/rw_dict: dictsource/rw_list dictsource/rw_rules dictsource/rw_extra
	cd dictsource && ../src/espeak --compile=rw && cd ..

si: espeak-data/si_dict
dictsource/si_extra:
	touch dictsource/si_extra
espeak-data/si_dict: dictsource/si_list dictsource/si_rules dictsource/si_extra
	cd dictsource && ../src/espeak --compile=si && cd ..

sk: espeak-data/sk_dict
dictsource/sk_extra:
	touch dictsource/sk_extra
espeak-data/sk_dict: dictsource/sk_list dictsource/sk_rules dictsource/sk_extra
	cd dictsource && ../src/espeak --compile=sk && cd ..

sl: espeak-data/sl_dict
dictsource/sl_extra:
	touch dictsource/sl_extra
espeak-data/sl_dict: dictsource/sl_list dictsource/sl_rules dictsource/sl_extra
	cd dictsource && ../src/espeak --compile=sl && cd ..

sq: espeak-data/sq_dict
dictsource/sq_extra:
	touch dictsource/sq_extra
espeak-data/sq_dict: dictsource/sq_list dictsource/sq_rules dictsource/sq_extra
	cd dictsource && ../src/espeak --compile=sq && cd ..

sv: espeak-data/sv_dict
dictsource/sv_extra:
	touch dictsource/sv_extra
espeak-data/sv_dict: dictsource/sv_list dictsource/sv_rules dictsource/sv_extra
	cd dictsource && ../src/espeak --compile=sv && cd ..

sw: espeak-data/sw_dict
dictsource/sw_extra:
	touch dictsource/sw_extra
espeak-data/sw_dict: dictsource/sw_list dictsource/sw_rules dictsource/sw_extra
	cd dictsource && ../src/espeak --compile=sw && cd ..

ta: espeak-data/ta_dict
dictsource/ta_extra:
	touch dictsource/ta_extra
espeak-data/ta_dict: dictsource/ta_list dictsource/ta_rules dictsource/ta_extra
	cd dictsource && ../src/espeak --compile=ta && cd ..

te: espeak-data/te_dict
dictsource/te_extra:
	touch dictsource/te_extra
espeak-data/te_dict: dictsource/te_list dictsource/te_rules dictsource/te_extra
	cd dictsource && ../src/espeak --compile=te && cd ..

tn: espeak-data/tn_dict
dictsource/tn_extra:
	touch dictsource/tn_extra
espeak-data/tn_dict: dictsource/tn_list dictsource/tn_rules dictsource/tn_extra
	cd dictsource && ../src/espeak --compile=tn && cd ..

tr: espeak-data/tr_dict
dictsource/tr_extra:
	touch dictsource/tr_extra
espeak-data/tr_dict: dictsource/tr_list dictsource/tr_rules dictsource/tr_extra
	cd dictsource && ../src/espeak --compile=tr && cd ..

ur: espeak-data/ur_dict
dictsource/ur_extra:
	touch dictsource/ur_extra
espeak-data/ur_dict: dictsource/ur_list dictsource/ur_rules dictsource/ur_extra
	cd dictsource && ../src/espeak --compile=ur && cd ..

vi: espeak-data/vi_dict
dictsource/vi_extra:
	touch dictsource/vi_extra
espeak-data/vi_dict: dictsource/vi_list dictsource/vi_rules dictsource/vi_extra
	cd dictsource && ../src/espeak --compile=vi && cd ..

wo: espeak-data/wo_dict
dictsource/wo_extra:
	touch dictsource/wo_extra
espeak-data/wo_dict: dictsource/wo_list dictsource/wo_rules dictsource/wo_extra
	cd dictsource && ../src/espeak --compile=wo && cd ..

zh: espeak-data/zh_dict
dictsource/zh_extra:
	touch dictsource/zh_extra
espeak-data/zh_dict: dictsource/zh_list dictsource/zh_rules dictsource/zh_extra
	cd dictsource && ../src/espeak --compile=zh && cd ..

zhy: espeak-data/zhy_dict
dictsource/zhy_extra:
	touch dictsource/zhy_extra
espeak-data/zhy_dict: dictsource/zhy_rules dictsource/zhy_extra
	cd dictsource && ../src/espeak --compile=zhy && cd ..
