# remove -fno-exceptions  if it causes problems with stdio library on some platforms (ARM)

DATADIR=/usr/share/espeak-data

PREFIX=/usr
BINDIR=$(PREFIX)/bin
INCDIR=$(PREFIX)/include/espeak
LIBDIR=$(PREFIX)/lib

RELEASE = 1.48
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

# 'runtime' uses pulseaudio if it is running, else uses portaudio
#AUDIO = runtime
AUDIO = portaudio
#AUDIO = portaudio0
#AUDIO = portaudio2
#AUDIO = pulseaudio
#AUDIO = sada

ifeq ($(AUDIO),runtime)
WAVE=wave.cpp wave_pulse.cpp
LIB_AUDIO=-lpulse -lpulse-simple -lportaudio
USE_AUDIO=-DUSE_PULSEAUDIO -DUSE_PORTAUDIO
else
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
ifeq ($(AUDIO),portaudio0)
WAVE=wave.cpp
LIB_AUDIO=/usr/lib/libportaudio.so.0
USE_AUDIO=-DUSE_PORTAUDIO
else
ifeq ($(AUDIO),portaudio2)
WAVE=wave.cpp
LIB_AUDIO=/usr/lib/libportaudio.so.2
USE_AUDIO=-DUSE_PORTAUDIO
else
ifeq ($(AUDIO),sada)
WAVE=wave_sada.cpp
USE_AUDIO=-DUSE_SADA
else
WAVE=wave.cpp
endif
endif
endif
endif
endif
endif

speak_SOURCES = speak.cpp compiledict.cpp dictionary.cpp intonation.cpp \
	readclause.cpp setlengths.cpp numbers.cpp synth_mbrola.cpp \
	synthdata.cpp synthesize.cpp translate.cpp mbrowrap.cpp \
	tr_languages.cpp voices.cpp wavegen.cpp phonemelist.cpp klatt.cpp sonic.cpp

libespeak_SOURCES = speak_lib.cpp compiledict.cpp dictionary.cpp intonation.cpp \
	readclause.cpp setlengths.cpp numbers.cpp synth_mbrola.cpp \
	synthdata.cpp synthesize.cpp translate.cpp mbrowrap.cpp \
	tr_languages.cpp voices.cpp wavegen.cpp phonemelist.cpp \
	espeak_command.cpp event.cpp fifo.cpp $(WAVE) debug.cpp klatt.cpp sonic.cpp

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

.cpp.o:
	$(CXX) $(CXXFLAGS) $(USE_AUDIO) -D PATH_ESPEAK_DATA=\"$(DATADIR)\" -Wall -pedantic -I. -c -fno-exceptions  $<

$(BIN_NAME): $(OBJS1)
	$(CXX) $(LDFLAGS) -o $@ $(OBJS1) $(LIBS1)

$(BIN2_NAME): $(OBJS3) $(LIB_NAME)
	$(CXX) $(LDFLAGS) -o $@ $(OBJS3) $(LIBS3)



x_%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(USE_AUDIO) -D PATH_ESPEAK_DATA=\"$(DATADIR)\" -Wall -fpic -fvisibility=hidden -pedantic \
	-I. -D LIBRARY -c -fno-exceptions  $<  -o x_$*.o

$(LIB_NAME): $(OBJS2)
	$(CXX) $(LDFLAGS) -shared $(SONAME_OPT)$(LIB_NAME).$(LIB_VERSION) -o $@ \
	$(OBJS2) $(LIBS2)
	mv $(LIB_NAME) $(LIB_NAME).$(LIBTAG)
	$(LN_SF) $(LIB_NAME).$(LIBTAG) $(LIB_NAME).$(LIB_VERSION)
	$(LN_SF) $(LIB_NAME).$(LIB_VERSION) $(LIB_NAME)

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

