PLATFORM=big_endian

all: espeak espeakedit espeak-phoneme-data

espeak:
	cd src && make && cd ..

espeakedit:
	cd src && make -f Makefile.espeakedit && cd ..

espeak-phoneme-data:
	cd platforms/${PLATFORM} && make && cd ../..

clean:
	cd src && rm -f *.o *~ && cd ..

distclean: clean
	cd src && rm -f libespeak.a libespeak.so.* speak espeak espeakedit && cd ..

