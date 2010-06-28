PREFIX=/usr
BINDIR=$(PREFIX)/bin
DATADIR=$(PREFIX)/share/espeak-data

PLATFORM=big_endian

.PHONY: all clean distclean espeak espeakedit espeak-phoneme-data espeak-data espeak-data-local

##### standard build actions:

all: espeak espeakedit espeak-data-local

install:
	cd src && make DESTDIR=$(DESTDIR) PREFIX=$(PREFIX) install && cd ..
	install -m 755 src/espeakedit $(DESTDIR)$(BINDIR)
	cp -prf espeak-data-local/* $(DESTDIR)$(DATADIR)

clean:
	cd src && rm -f *.o *~ && cd ..

distclean: clean
	cd src && rm -f libespeak.a libespeak.so.* speak espeak espeakedit && cd ..
	cd platforms/$(PLATFORM) && rm -f espeak-phoneme-data && cd ../..
	rm -rf espeak-data-local

##### build targets:

espeak:
	cd src && make PREFIX=$(PREFIX) && cd ..

espeakedit:
	cd src && make -f Makefile.espeakedit PREFIX=$(PREFIX) && cd ..

espeak-phoneme-data:
	cd platforms/$(PLATFORM) && make PREFIX=$(PREFIX) && cd ../..

espeak-data: espeakedit
	rm -rf $(HOME)/espeak-data
	cp -a espeak-data $(HOME)/espeak-data
	cp -a phsource $(HOME)/espeak-data/phsource
	cp -a dictsource $(HOME)/espeak-data/dictsource
	mv $(HOME)/espeak-data/phsource/intonation $(HOME)/espeak-data/intonations
	src/espeakedit --compile

espeak-data-local: espeak-phoneme-data espeak-data
	rm -rf espeak-data-local
	cp -a $(HOME)/espeak-data espeak-data-local
	platforms/$(PLATFORM)/espeak-phoneme-data espeak-data-local espeak-data-local espeak-data-local/phondata-manifest
	mkdir -p espeak-data-local/mbrola
	mkdir -p espeak-data-local/mbrola_ph
	mkdir -p espeak-data-local/soundicons
	mkdir -p espeak-data-local/voices/test
	rm -rf espeak-data-local/dictsource
	rm -rf espeak-data-local/phsource
	rm -rf espeak-data-local/phondata-manifest

