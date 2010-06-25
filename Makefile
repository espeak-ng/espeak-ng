all: espeak espeakedit

espeak:
	cd src && make && cd ..

espeakedit:
	cd src && make -f Makefile.espeakedit && cd ..

clean:
	cd src && rm -f *.o *~ && cd ..

distclean: clean
	cd src && rm -f libespeak.a libespeak.so.* speak espeak espeakedit && cd ..

