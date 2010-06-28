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
	cp -a espeak-data $(HOME)/espeak-data
	cp -a phsource $(HOME)/espeak-data/phsource
	cp -a dictsource $(HOME)/espeak-data/dictsource
	src/espeakedit --compile

espeak-data-local: espeak-phoneme-data espeak-data
	cp -a $(HOME)/espeak-data espeak-data-local
	platforms/$(PLATFORM)/espeak-phoneme-data espeak-data-local espeak-data-local espeak-data-local/phondata-manifest

