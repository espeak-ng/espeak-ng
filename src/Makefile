# remove -fno-exceptions  if it causes problems with stdio library on some platforms (ARM)

PREFIX=/
BINDIR=$(PREFIX)/usr/bin
INCDIR=$(PREFIX)/usr/include/espeak
LIBDIR=$(PREFIX)/usr/lib
DATADIR=$(PREFIX)/usr/share/espeak-data

RELEASE = 1.36
BIN_NAME = speak
BIN2_NAME = espeak
LIB_NAME = libespeak.so
STATIC_LIB_NAME = libespeak.a
LIB_VERSION = 1
LIBTAG = $(LIB_VERSION).$(RELEASE)

# Use SONAME_OPT=-Wl,h, on Solaris
SONAME_OPT=-Wl,-soname,

# Use EXTRA_LIBS=-lm on Solaris
EXTRA_LIBS =

INSTALL = install
LN_SF = /bin/ln -sf
MKDIR = mkdir -p

#AUDIO = pulseaudio
AUDIO = portaudio


ifeq ($(AUDIO),pulseaudio)
WAVE=wave_pulse.cpp
LIB_AUDIO=-lpulse
USE_AUDIO=-DUSE_PULSEAUDIO
else
ifeq ($(AUDIO),portaudio)
WAVE=wave.cpp
LIB_AUDIO=-lportaudio
USE_AUDIO=-DUSE_PORTAUDIO
else
WAVE=wave.cpp
endif
endif

speak_SOURCES = speak.cpp compiledict.cpp dictionary.cpp intonation.cpp \
	readclause.cpp setlengths.cpp numbers.cpp synth_mbrola.cpp \
	synthdata.cpp synthesize.cpp translate.cpp tr_english.cpp \
	tr_languages.cpp voices.cpp wavegen.cpp phonemelist.cpp

libespeak_SOURCES = speak_lib.cpp compiledict.cpp dictionary.cpp intonation.cpp \
	readclause.cpp setlengths.cpp numbers.cpp synth_mbrola.cpp \
	synthdata.cpp synthesize.cpp translate.cpp tr_english.cpp \
	tr_languages.cpp voices.cpp wavegen.cpp phonemelist.cpp \
	espeak_command.cpp event.cpp fifo.cpp $(WAVE) debug.cpp

SRCS1=$(speak_SOURCES)
OBJS1=$(patsubst %.cpp,%.o,$(SRCS1))
LIBS1=-lstdc++ $(LIB_AUDIO) -lpthread $(EXTRA_LIBS)

SRCS2=$(libespeak_SOURCES)
OBJS2=$(patsubst %.cpp,x_%.o,$(SRCS2))
LIBS2=-lstdc++ $(LIB_AUDIO) -lpthread

SRCS3 = espeak.cpp
OBJS3=$(patsubst %.cpp,%.o,$(SRCS3))
LIBS3=-lstdc++ -L . -lespeak

CXXFLAGS=-O2


all: $(BIN_NAME) $(LIB_NAME) $(STATIC_LIB_NAME) $(BIN2_NAME)
	mv $(LIB_NAME) $(LIB_NAME).$(LIBTAG)

.cpp.o:
	$(CXX) $(CXXFLAGS) $(USE_AUDIO) -D PATH_ESPEAK_DATA=\"$(DATADIR)\" -Wall -pedantic -I. -c -fno-exceptions  $<

$(BIN_NAME): $(OBJS1)
	$(CXX) -o $@ $(OBJS1) $(LIBS1)

$(BIN2_NAME): $(OBJS3) $(LIB_NAME)
	$(CXX) -o $@ $(OBJS3) $(LIBS3)



x_%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(USE_AUDIO) -Wall -fpic -pedantic \
	-I. -D LIBRARY -c -fno-exceptions  $<  -o x_$*.o

$(LIB_NAME): $(OBJS2)
	$(CXX) -shared $(SONAME_OPT)$(LIB_NAME).$(LIB_VERSION) -o $@ $(OBJS2) $(LIBS2)

$(STATIC_LIB_NAME): $(OBJS2)
	$(AR) cqs $(STATIC_LIB_NAME) $(OBJS2)

clean:
	rm -f *.o *.a *~

distclean: clean
	rm -f $(BIN_NAME)
	rm -f $(BIN2_NAME)
	rm -f $(LIB_NAME)*

install: all
	# Create directories
	rm -rf $(DESTDIR)$(DATADIR)
	$(MKDIR) $(DESTDIR)$(BINDIR)
	$(MKDIR) $(DESTDIR)$(LIBDIR)
	$(MKDIR) $(DESTDIR)$(INCDIR)
	$(MKDIR) $(DESTDIR)$(DATADIR)
	# Install espeak executable
	$(INSTALL) -m 755 $(BIN2_NAME) $(DESTDIR)$(BINDIR)
	# Install shared library
	$(INSTALL) -m 755 $(LIB_NAME).$(LIBTAG) $(DESTDIR)$(LIBDIR)
	# Install static library
	$(INSTALL) -m 755 $(STATIC_LIB_NAME) $(DESTDIR)$(LIBDIR)
	$(LN_SF) $(LIB_NAME).$(LIBTAG) $(DESTDIR)$(LIBDIR)/$(LIB_NAME).$(LIB_VERSION)
	$(LN_SF) $(LIB_NAME).$(LIB_VERSION) $(DESTDIR)$(LIBDIR)/$(LIB_NAME)
	# Install development headers
	$(INSTALL) -pm 644 speak_lib.h $(DESTDIR)$(INCDIR)
	# Install data files
	cp -prf ../espeak-data/* $(DESTDIR)$(DATADIR)

