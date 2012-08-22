speak_SOURCES = speak.cpp compiledict.cpp dictionary.cpp intonation.cpp \
	readclause.cpp setlengths.cpp \
	synthdata.cpp synthesize.cpp translate.cpp tr_english.cpp \
	tr_languages.cpp voices.cpp wavegen.cpp

libespeak_SOURCES = speak_lib.cpp compiledict.cpp dictionary.cpp intonation.cpp \
	readclause.cpp setlengths.cpp \
	synthdata.cpp synthesize.cpp translate.cpp tr_english.cpp \
	tr_languages.cpp voices.cpp wavegen.cpp


SRCS1=$(speak_SOURCES)
OBJS1=$(patsubst %.cpp,%.o,$(SRCS1))
LIBS1=-lstdc++ -lportaudio

SRCS2=$(libespeak_SOURCES)
OBJS2=$(patsubst %.cpp,x_%.o,$(SRCS2))
LIBS2=-lstdc++



all: speak libespeak.so
	mv libespeak.so libespeak.so.1.1.16

.cpp.o:
	$(CXX) $(CXXFLAGS) -O2 -Wall -pedantic -I. -c -fno-exceptions $<

speak: $(OBJS1)
	$(CXX) -o $@ $(OBJS1) $(LIBS1)



x_%.o: %.cpp
	$(CXX) $(CXXFLAGS) -O2 -Wall -fpic -fvisibility=hidden -pedantic \
	-I. -D LIBRARY -c -fno-exceptions $<  -o x_$*.o

libespeak.so: $(OBJS2)
	$(CXX) -shared -Wl,-soname,libespeak.so.1 -o $@ $(OBJS2) $(LIBS2)

clean:
	rm -f *.o *~

distclean: clean
	rm -f speak
	rm -f libespeak.so*
