PREFIX=/usr
BINDIR=$(PREFIX)/bin
DATADIR=$(PREFIX)/share/espeak-data

PLATFORM=big_endian

.PHONY: all clean distclean espeak espeakedit espeak-phoneme-data espeak-data

##### standard build actions:

all: speak libespeak.so libespeak.a espeak espeakedit espeak-data dictionaries

install: all
	cd src && make DESTDIR=$(DESTDIR) PREFIX=$(PREFIX) install && cd ..
	install -m 755 src/espeakedit $(DESTDIR)$(BINDIR)

clean:
	cd src && rm -f *.o *~ && cd ..

distclean: clean
	cd src && rm -f libespeak.a libespeak.so.* speak espeak espeakedit && cd ..
	cd platforms/$(PLATFORM) && rm -f espeak-phoneme-data && cd ../..
	rm -rf espeak-data/dictsource espeak-data/phsource espeak-data/phondata-manifest
	cd espeak-data && rm -f *_dict && cd ..

##### build targets:

libespeak.a:
	cd src && make libespeak.a PREFIX=$(PREFIX) && cd ..

libespeak.so:
	cd src && make libespeak.so PREFIX=$(PREFIX) && cd ..

speak:
	cd src && make speak PREFIX=$(PREFIX) && cd ..

espeak: libespeak.so
	cd src && make espeak PREFIX=$(PREFIX) && cd ..

espeakedit:
	cd src && make espeakedit PREFIX=$(PREFIX) && cd ..

espeak-phoneme-data:
	cd platforms/$(PLATFORM) && make PREFIX=$(PREFIX) && cd ../..

espeak-data-dir:
	rm -rf espeak-data/dictsource espeak-data/phsource espeak-data/phondata-manifest
	cp -a phsource espeak-data/phsource
	cp -a dictsource espeak-data/dictsource

espeak-data: espeakedit espeak-data-dir
	rm -rf $(HOME)/espeak-data
	ln -sv $(PWD)/espeak-data $(HOME)/espeak-data
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

dictsource/af_extra:
	touch dictsource/af_extra
espeak-data/af_dict: dictsource/af_list dictsource/af_rules dictsource/af_extra
	cd dictsource && ../src/espeak --compile=af && cd ..

dictsource/ak_extra:
	touch dictsource/ak_extra
espeak-data/ak_dict: dictsource/ak_rules dictsource/ak_extra
	cd dictsource && ../src/espeak --compile=ak && cd ..

dictsource/am_extra:
	touch dictsource/am_extra
espeak-data/am_dict: dictsource/am_list dictsource/am_rules dictsource/am_extra
	cd dictsource && ../src/espeak --compile=am && cd ..

dictsource/az_extra:
	touch dictsource/az_extra
espeak-data/az_dict: dictsource/az_list dictsource/az_rules dictsource/az_extra
	cd dictsource && ../src/espeak --compile=az && cd ..

dictsource/bg_extra:
	touch dictsource/bg_extra
espeak-data/bg_dict: dictsource/bg_list dictsource/bg_listx dictsource/bg_rules dictsource/bg_extra
	cd dictsource && ../src/espeak --compile=bg && cd ..

dictsource/bo_extra:
	touch dictsource/bo_extra
espeak-data/bo_dict: dictsource/bo_rules dictsource/bo_extra
	cd dictsource && ../src/espeak --compile=bo && cd ..

dictsource/ca_extra:
	touch dictsource/ca_extra
espeak-data/ca_dict: dictsource/ca_list dictsource/ca_rules dictsource/ca_extra
	cd dictsource && ../src/espeak --compile=ca && cd ..

dictsource/cs_extra:
	touch dictsource/cs_extra
espeak-data/cs_dict: dictsource/cs_list dictsource/cs_rules dictsource/cs_extra
	cd dictsource && ../src/espeak --compile=cs && cd ..

dictsource/cy_extra:
	touch dictsource/cy_extra
espeak-data/cy_dict: dictsource/cy_list dictsource/cy_rules dictsource/cy_extra
	cd dictsource && ../src/espeak --compile=cy && cd ..

dictsource/da_extra:
	touch dictsource/da_extra
espeak-data/da_dict: dictsource/da_list dictsource/da_rules dictsource/da_extra
	cd dictsource && ../src/espeak --compile=da && cd ..

dictsource/de_extra:
	touch dictsource/de_extra
espeak-data/de_dict: dictsource/de_list dictsource/de_rules dictsource/de_extra
	cd dictsource && ../src/espeak --compile=de && cd ..

dictsource/dv_extra:
	touch dictsource/dv_extra
espeak-data/dv_dict: dictsource/dv_list dictsource/dv_rules dictsource/dv_extra
	cd dictsource && ../src/espeak --compile=dv && cd ..

dictsource/el_extra:
	touch dictsource/el_extra
espeak-data/el_dict: dictsource/el_list dictsource/el_rules dictsource/el_extra
	cd dictsource && ../src/espeak --compile=el && cd ..

dictsource/en_extra:
	touch dictsource/en_extra
espeak-data/en_dict: dictsource/en_list dictsource/en_rules dictsource/en_extra
	cd dictsource && ../src/espeak --compile=en && cd ..

dictsource/eo_extra:
	touch dictsource/eo_extra
espeak-data/eo_dict: dictsource/eo_list dictsource/eo_rules dictsource/eo_extra
	cd dictsource && ../src/espeak --compile=eo && cd ..

dictsource/es_extra:
	touch dictsource/es_extra
espeak-data/es_dict: dictsource/es_list dictsource/es_rules dictsource/es_extra
	cd dictsource && ../src/espeak --compile=es && cd ..

dictsource/et_extra:
	touch dictsource/et_extra
espeak-data/et_dict: dictsource/et_list dictsource/et_rules dictsource/et_extra
	cd dictsource && ../src/espeak --compile=et && cd ..

dictsource/fi_extra:
	touch dictsource/fi_extra
espeak-data/fi_dict: dictsource/fi_list dictsource/fi_rules dictsource/fi_extra
	cd dictsource && ../src/espeak --compile=fi && cd ..

dictsource/fr_extra:
	touch dictsource/fr_extra
espeak-data/fr_dict: dictsource/fr_list dictsource/fr_rules dictsource/fr_extra
	cd dictsource && ../src/espeak --compile=fr && cd ..

dictsource/grc_extra:
	touch dictsource/grc_extra
espeak-data/grc_dict: dictsource/grc_list dictsource/grc_rules dictsource/grc_extra
	cd dictsource && ../src/espeak --compile=grc && cd ..

dictsource/hbs_extra:
	touch dictsource/hbs_extra
espeak-data/hbs_dict: dictsource/hbs_list dictsource/hbs_rules dictsource/hbs_extra
	cd dictsource && ../src/espeak --compile=hbs && cd ..

dictsource/hi_extra:
	touch dictsource/hi_extra
espeak-data/hi_dict: dictsource/hi_list dictsource/hi_rules dictsource/hi_extra
	cd dictsource && ../src/espeak --compile=hi && cd ..

dictsource/ht_extra:
	touch dictsource/ht_extra
espeak-data/ht_dict: dictsource/ht_list dictsource/ht_rules dictsource/ht_extra
	cd dictsource && ../src/espeak --compile=ht && cd ..

dictsource/hu_extra:
	touch dictsource/hu_extra
espeak-data/hu_dict: dictsource/hu_list dictsource/hu_rules dictsource/hu_extra
	cd dictsource && ../src/espeak --compile=hu && cd ..

dictsource/hy_extra:
	touch dictsource/hy_extra
espeak-data/hy_dict: dictsource/hy_list dictsource/hy_rules dictsource/hy_extra
	cd dictsource && ../src/espeak --compile=hy && cd ..

dictsource/id_extra:
	touch dictsource/id_extra
espeak-data/id_dict: dictsource/id_list dictsource/id_rules dictsource/id_extra
	cd dictsource && ../src/espeak --compile=id && cd ..

dictsource/is_extra:
	touch dictsource/is_extra
espeak-data/is_dict: dictsource/is_list dictsource/is_rules dictsource/is_extra
	cd dictsource && ../src/espeak --compile=is && cd ..

dictsource/it_extra:
	touch dictsource/it_extra
espeak-data/it_dict: dictsource/it_list dictsource/it_listx dictsource/it_rules dictsource/it_extra
	cd dictsource && ../src/espeak --compile=it && cd ..

dictsource/jbo_extra:
	touch dictsource/jbo_extra
espeak-data/jbo_dict: dictsource/jbo_list dictsource/jbo_rules dictsource/jbo_extra
	cd dictsource && ../src/espeak --compile=jbo && cd ..

dictsource/ka_extra:
	touch dictsource/ka_extra
espeak-data/ka_dict: dictsource/ka_list dictsource/ka_rules dictsource/ka_extra
	cd dictsource && ../src/espeak --compile=ka && cd ..

dictsource/kk_extra:
	touch dictsource/kk_extra
espeak-data/kk_dict: dictsource/kk_list dictsource/kk_rules dictsource/kk_extra
	cd dictsource && ../src/espeak --compile=kk && cd ..

dictsource/kn_extra:
	touch dictsource/kn_extra
espeak-data/kn_dict: dictsource/kn_list dictsource/kn_rules dictsource/kn_extra
	cd dictsource && ../src/espeak --compile=kn && cd ..

dictsource/ku_extra:
	touch dictsource/ku_extra
espeak-data/ku_dict: dictsource/ku_list dictsource/ku_rules dictsource/ku_extra
	cd dictsource && ../src/espeak --compile=ku && cd ..

dictsource/la_extra:
	touch dictsource/la_extra
espeak-data/la_dict: dictsource/la_list dictsource/la_rules dictsource/la_extra
	cd dictsource && ../src/espeak --compile=la && cd ..

dictsource/lv_extra:
	touch dictsource/lv_extra
espeak-data/lv_dict: dictsource/lv_list dictsource/lv_rules dictsource/lv_extra
	cd dictsource && ../src/espeak --compile=lv && cd ..

dictsource/mk_extra:
	touch dictsource/mk_extra
espeak-data/mk_dict: dictsource/mk_list dictsource/mk_rules dictsource/mk_extra
	cd dictsource && ../src/espeak --compile=mk && cd ..

dictsource/ml_extra:
	touch dictsource/ml_extra
espeak-data/ml_dict: dictsource/ml_list dictsource/ml_rules dictsource/ml_extra
	cd dictsource && ../src/espeak --compile=ml && cd ..

dictsource/mt_extra:
	touch dictsource/mt_extra
espeak-data/mt_dict: dictsource/mt_list dictsource/mt_rules dictsource/mt_extra
	cd dictsource && ../src/espeak --compile=mt && cd ..

dictsource/nci_extra:
	touch dictsource/nci_extra
espeak-data/nci_dict: dictsource/nci_list dictsource/nci_rules dictsource/nci_extra
	cd dictsource && ../src/espeak --compile=nci && cd ..

dictsource/ne_extra:
	touch dictsource/ne_extra
espeak-data/ne_dict: dictsource/ne_list dictsource/ne_rules dictsource/ne_extra
	cd dictsource && ../src/espeak --compile=ne && cd ..

dictsource/nl_extra:
	touch dictsource/nl_extra
espeak-data/nl_dict: dictsource/nl_list dictsource/nl_rules dictsource/nl_extra
	cd dictsource && ../src/espeak --compile=nl && cd ..

dictsource/no_extra:
	touch dictsource/no_extra
espeak-data/no_dict: dictsource/no_list dictsource/no_rules dictsource/no_extra
	cd dictsource && ../src/espeak --compile=no && cd ..

dictsource/nso_extra:
	touch dictsource/nso_extra
espeak-data/nso_dict: dictsource/nso_list dictsource/nso_rules dictsource/nso_extra
	cd dictsource && ../src/espeak --compile=nso && cd ..

dictsource/pa_extra:
	touch dictsource/pa_extra
espeak-data/pa_dict: dictsource/pa_list dictsource/pa_rules dictsource/pa_extra
	cd dictsource && ../src/espeak --compile=pa && cd ..

dictsource/pap_extra:
	touch dictsource/pap_extra
espeak-data/pap_dict: dictsource/pap_list dictsource/pap_rules dictsource/pap_extra
	cd dictsource && ../src/espeak --compile=pap && cd ..

dictsource/pl_extra:
	touch dictsource/pl_extra
espeak-data/pl_dict: dictsource/pl_list dictsource/pl_rules dictsource/pl_extra
	cd dictsource && ../src/espeak --compile=pl && cd ..

dictsource/prs_extra:
	touch dictsource/prs_extra
espeak-data/prs_dict: dictsource/prs_list dictsource/prs_rules dictsource/prs_extra
	cd dictsource && ../src/espeak --compile=prs && cd ..

dictsource/pt_extra:
	touch dictsource/pt_extra
espeak-data/pt_dict: dictsource/pt_list dictsource/pt_rules dictsource/pt_extra
	cd dictsource && ../src/espeak --compile=pt && cd ..

dictsource/ro_extra:
	touch dictsource/ro_extra
espeak-data/ro_dict: dictsource/ro_list dictsource/ro_rules dictsource/ro_extra
	cd dictsource && ../src/espeak --compile=ro && cd ..

dictsource/ru_extra:
	touch dictsource/ru_extra
espeak-data/ru_dict: dictsource/ru_list dictsource/ru_rules dictsource/ru_extra
	cd dictsource && ../src/espeak --compile=ru && cd ..

dictsource/rw_extra:
	touch dictsource/rw_extra
espeak-data/rw_dict: dictsource/rw_list dictsource/rw_rules dictsource/rw_extra
	cd dictsource && ../src/espeak --compile=rw && cd ..

dictsource/si_extra:
	touch dictsource/si_extra
espeak-data/si_dict: dictsource/si_list dictsource/si_rules dictsource/si_extra
	cd dictsource && ../src/espeak --compile=si && cd ..

dictsource/sk_extra:
	touch dictsource/sk_extra
espeak-data/sk_dict: dictsource/sk_list dictsource/sk_rules dictsource/sk_extra
	cd dictsource && ../src/espeak --compile=sk && cd ..

dictsource/sl_extra:
	touch dictsource/sl_extra
espeak-data/sl_dict: dictsource/sl_list dictsource/sl_rules dictsource/sl_extra
	cd dictsource && ../src/espeak --compile=sl && cd ..

dictsource/sq_extra:
	touch dictsource/sq_extra
espeak-data/sq_dict: dictsource/sq_list dictsource/sq_rules dictsource/sq_extra
	cd dictsource && ../src/espeak --compile=sq && cd ..

dictsource/sv_extra:
	touch dictsource/sv_extra
espeak-data/sv_dict: dictsource/sv_list dictsource/sv_rules dictsource/sv_extra
	cd dictsource && ../src/espeak --compile=sv && cd ..

dictsource/sw_extra:
	touch dictsource/sw_extra
espeak-data/sw_dict: dictsource/sw_list dictsource/sw_rules dictsource/sw_extra
	cd dictsource && ../src/espeak --compile=sw && cd ..

dictsource/ta_extra:
	touch dictsource/ta_extra
espeak-data/ta_dict: dictsource/ta_list dictsource/ta_rules dictsource/ta_extra
	cd dictsource && ../src/espeak --compile=ta && cd ..

dictsource/te_extra:
	touch dictsource/te_extra
espeak-data/te_dict: dictsource/te_list dictsource/te_rules dictsource/te_extra
	cd dictsource && ../src/espeak --compile=te && cd ..

dictsource/tn_extra:
	touch dictsource/tn_extra
espeak-data/tn_dict: dictsource/tn_list dictsource/tn_rules dictsource/tn_extra
	cd dictsource && ../src/espeak --compile=tn && cd ..

dictsource/tr_extra:
	touch dictsource/tr_extra
espeak-data/tr_dict: dictsource/tr_list dictsource/tr_rules dictsource/tr_extra
	cd dictsource && ../src/espeak --compile=tr && cd ..

dictsource/ur_extra:
	touch dictsource/ur_extra
espeak-data/ur_dict: dictsource/ur_list dictsource/ur_rules dictsource/ur_extra
	cd dictsource && ../src/espeak --compile=ur && cd ..

dictsource/vi_extra:
	touch dictsource/vi_extra
espeak-data/vi_dict: dictsource/vi_list dictsource/vi_rules dictsource/vi_extra
	cd dictsource && ../src/espeak --compile=vi && cd ..

dictsource/wo_extra:
	touch dictsource/wo_extra
espeak-data/wo_dict: dictsource/wo_list dictsource/wo_rules dictsource/wo_extra
	cd dictsource && ../src/espeak --compile=wo && cd ..

dictsource/zh_extra:
	touch dictsource/zh_extra
espeak-data/zh_dict: dictsource/zh_list dictsource/zh_rules dictsource/zh_extra
	cd dictsource && ../src/espeak --compile=zh && cd ..

dictsource/zhy_extra:
	touch dictsource/zhy_extra
espeak-data/zhy_dict: dictsource/zhy_rules dictsource/zhy_extra
	cd dictsource && ../src/espeak --compile=zhy && cd ..
