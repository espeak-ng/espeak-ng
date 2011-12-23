PREFIX=/usr
BINDIR=$(PREFIX)/bin
DATADIR=$(PREFIX)/share/espeak-data

PLATFORM=big_endian

.PHONY: all clean distclean espeak espeakedit espeak-phoneme-data espeak-data

##### standard build actions:

all: espeak espeakedit espeak-data

install:
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

espeak:
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

en: dictsource/en_rules dictsource/en_list
	rm -f dictsource/en_extra
	make all
	../cainteoir-engine/tests/dictionary.py ../cainteoir-english/data/reece --exception-dictionary > dictsource/en_extra
	make espeak-data
	../cainteoir-engine/tests/dictionary.py ../cainteoir-english/data/reece | grep -F fail
	wc -l dictsource/en_extra

