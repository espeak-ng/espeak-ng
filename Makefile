PLATFORM=big_endian

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

espeak-data-local: espeak-phoneme-data
	cp -a espeak-data espeak-data-local
	platforms/big_endian/espeak-phoneme-data espeak-data-local espeak-data-local platforms/${PLATFORM}/phondata-manifest

