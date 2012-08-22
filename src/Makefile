BINDIR=/usr/bin
INCDIR=/usr/include/espeak
LIBDIR=/usr/lib
DATADIR=/usr/share/espeak-data

RELEASE = 1.18
BIN_NAME = speak
BIN2_NAME = espeak
LIB_NAME = libespeak.so
LIB_VERSION = 1
LIBTAG = $(LIB_VERSION).$(RELEASE)

INSTALL = /usr/bin/install
LN_SF = /bin/ln -sf
MKDIR = mkdir -p

speak_SOURCES = speak.cpp compiledict.cpp dictionary.cpp intonation.cpp \
	readclause.cpp setlengths.cpp numbers.cpp \
	synthdata.cpp synthesize.cpp translate.cpp tr_english.cpp \
	tr_languages.cpp voices.cpp wavegen.cpp

libespeak_SOURCES = speak_lib.cpp compiledict.cpp dictionary.cpp intonation.cpp \
	readclause.cpp setlengths.cpp numbers.cpp \
	synthdata.cpp synthesize.cpp translate.cpp tr_english.cpp \
	tr_languages.cpp voices.cpp wavegen.cpp \
	espeak_command.cpp event.cpp fifo.cpp wave.cpp debug.cpp


SRCS1=$(speak_SOURCES)
OBJS1=$(patsubst %.cpp,%.o,$(SRCS1))
LIBS1=-lstdc++ -lportaudio

SRCS2=$(libespeak_SOURCES)
OBJS2=$(patsubst %.cpp,x_%.o,$(SRCS2))
LIBS2=-lstdc++ -lportaudio

SRCS3 = espeak.cpp
OBJS3=$(patsubst %.cpp,%.o,$(SRCS3))
LIBS3=-lstdc++ ./libespeak.so

CXXFLAGS=-O2


all: $(BIN_NAME) $(LIB_NAME) $(BIN2_NAME)
	mv $(LIB_NAME) $(LIB_NAME).$(LIBTAG)

.cpp.o:
	$(CXX) $(CXXFLAGS) -D PATH_ESPEAK_DATA=\"$(DATADIR)\" -Wall -pedantic -I. -c -fno-exceptions $<

$(BIN_NAME): $(OBJS1)
	$(CXX) -o $@ $(OBJS1) $(LIBS1)

$(BIN2_NAME): $(OBJS3)
	$(CXX) -o $@ $(OBJS3) $(LIBS3)



x_%.o: %.cpp
	$(CXX) $(CXXFLAGS) -Wall -fpic -fvisibility=hidden -pedantic \
	-I. -D LIBRARY -c -fno-exceptions $<  -o x_$*.o

$(LIB_NAME): $(OBJS2)
	$(CXX) -shared -Wl,-soname,$(LIB_NAME).$(LIB_VERSION) -o $@ $(OBJS2) $(LIBS2)

clean:
	rm -f *.o *~

distclean: clean
	rm -f $(BIN_NAME)
	rm -f $(LIB_NAME)*

install: all
	# Create directories
	$(MKDIR) $(DESTDIR)$(BINDIR)
	$(MKDIR) $(DESTDIR)$(LIBDIR)
	$(MKDIR) $(DESTDIR)$(INCDIR)
	$(MKDIR) $(DESTDIR)$(DATADIR)
	# Install espeak executable
	$(INSTALL) -m 755 $(BIN2_NAME) $(DESTDIR)$(BINDIR)
	# Install shared library
	$(INSTALL) -m 755 $(LIB_NAME).$(LIBTAG) $(DESTDIR)$(LIBDIR)
	$(LN_SF) $(LIB_NAME).$(LIBTAG) $(DESTDIR)$(LIBDIR)/$(LIB_NAME).$(LIB_VERSION)
	$(LN_SF) $(LIB_NAME).$(LIB_VERSION) $(DESTDIR)$(LIBDIR)/$(LIB_NAME)
	# Install development headers
	$(INSTALL) -pm 644 speak_lib.h $(DESTDIR)$(INCDIR)
	# Install data files
	cp -prf ../espeak-data/* $(DESTDIR)$(DATADIR)

