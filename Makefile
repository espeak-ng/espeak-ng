PLATFORM=big_endian

.PHONY: all clean distclean espeak espeakedit espeak-phoneme-data espeak-data

##### standard build actions:

all: espeak espeakedit espeak-data-local

clean:
	cd src && rm -f *.o *~ && cd ..

distclean: clean
	cd src && rm -f libespeak.a libespeak.so.* speak espeak espeakedit && cd ..
	cd platforms/${PLATFORM} && rm -f espeak-phoneme-data && cd ../..
	rm -rf espeak-data-local

##### build targets:

espeak:
	cd src && make && cd ..

espeakedit:
	cd src && make -f Makefile.espeakedit && cd ..

espeak-phoneme-data:
	cd platforms/${PLATFORM} && make && cd ../..

espeak-data: espeakedit
	cp -a espeak-data ${HOME}/espeak-data
	cp -a phsource ${HOME}/espeak-data/phsource
	cp -a dictsource ${HOME}/espeak-data/dictsource
	src/espeakedit --compile

espeak-data-local: espeak-phoneme-data espeak-data
	cp -a ${HOME}/espeak-data espeak-data-local
	platforms/big_endian/espeak-phoneme-data espeak-data-local espeak-data-local espeak-data-local/phondata-manifest

