# remove -fno-exceptions  if it causes problems with stdio library on some platforms (ARM)

DATADIR=/usr/share/espeak-data

PREFIX=/usr
BINDIR=$(PREFIX)/bin
INCDIR=$(PREFIX)/include/espeak
LIBDIR=$(PREFIX)/lib

RELEASE = 1.46
SPEAK = speak
ESPEAK = espeak
LIBESPEAK = libespeak.so
STATIC_LIBESPEAK = libespeak.a
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

common_SOURCES = compiledict.cpp dictionary.cpp intonation.cpp \
	readclause.cpp setlengths.cpp numbers.cpp synth_mbrola.cpp \
	synthdata.cpp synthesize.cpp translate.cpp mbrowrap.cpp \
	tr_languages.cpp voices.cpp wavegen.cpp phonemelist.cpp \
	klatt.cpp sonic.cpp

speak_SOURCES = speak.cpp $(common_SOURCES)

libespeak_SOURCES = speak_lib.cpp $(common_SOURCES) \
	espeak_command.cpp event.cpp fifo.cpp $(WAVE) debug.cpp

speak_OBJS = $(patsubst %.cpp,%.o,$(speak_SOURCES))
speak_LIBS = -lstdc++ $(LIB_AUDIO) -lpthread $(EXTRA_LIBS)

libespeak_OBJS = $(patsubst %.cpp,x_%.o,$(libespeak_SOURCES))
libespeak_LIBS = -lstdc++ $(LIB_AUDIO) -lpthread

espeak_SOURCES = espeak.cpp
espeak_OBJS = $(patsubst %.cpp,%.o,$(espeak_SOURCES))
espeak_LIBS = -lstdc++ -L . -lespeak

CXXFLAGS=-O2


.cpp.o:
	$(CXX) $(CXXFLAGS) $(USE_AUDIO) -D PATH_ESPEAK_DATA=\"$(DATADIR)\" -Wall -pedantic -I. -c -fno-exceptions  $<

x_%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(USE_AUDIO) -D PATH_ESPEAK_DATA=\"$(DATADIR)\" -Wall -fpic -fvisibility=hidden -pedantic \
	-I. -D LIBRARY -c -fno-exceptions  $<  -o x_$*.o

all: $(SPEAK) $(LIBESPEAK) $(STATIC_LIBESPEAK) $(ESPEAK)

$(SPEAK): $(speak_OBJS)
	$(CXX) $(LDFLAGS) -o $@ $(speak_OBJS) $(speak_LIBS)

$(ESPEAK): $(espeak_OBJS) $(LIB_NAME)
	$(CXX) $(LDFLAGS) -o $@ $(espeak_OBJS) $(espeak_LIBS)

$(LIBESPEAK): $(libespeak_OBJS)
	$(CXX) $(LDFLAGS) -shared $(SONAME_OPT)$(LIBESPEAK).$(LIB_VERSION) -o $@ \
	$(libespeak_OBJS) $(libespeak_LIBS)
	mv $(LIBESPEAK) $(LIBESPEAK).$(LIBTAG)
	$(LN_SF) $(LIBESPEAK).$(LIBTAG) $(LIBESPEAK).$(LIB_VERSION)
	$(LN_SF) $(LIBESPEAK).$(LIB_VERSION) $(LIBESPEAK)

$(STATIC_LIBESPEAK): $(libespeak_OBJS)
	$(AR) cqs $(STATIC_LIBESPEAK) $(libespeak_OBJS)

clean:
	rm -f *.o *.a *~

distclean: clean
	rm -f $(SPEAK)
	rm -f $(ESPEAK)
	rm -f $(LIBESPEAK)*
	rm -f $(STATIC_LIBESPEAK)

install: all
	# Create directories
	rm -rf $(DESTDIR)$(DATADIR)
	$(MKDIR) $(DESTDIR)$(BINDIR)
	$(MKDIR) $(DESTDIR)$(LIBDIR)
	$(MKDIR) $(DESTDIR)$(INCDIR)
	$(MKDIR) $(DESTDIR)$(DATADIR)
	# Install espeak executable
	$(INSTALL) -m 755 $(ESPEAK) $(DESTDIR)$(BINDIR)
	# Install shared library
	$(INSTALL) -m 755 $(LIBESPEAK).$(LIBTAG) $(DESTDIR)$(LIBDIR)
	# Install static library
	$(INSTALL) -m 755 $(STATIC_LIBESPEAK) $(DESTDIR)$(LIBDIR)
	$(LN_SF) $(LIBESPEAK).$(LIBTAG) $(DESTDIR)$(LIBDIR)/$(LIBESPEAK).$(LIB_VERSION)
	$(LN_SF) $(LIBESPEAK).$(LIB_VERSION) $(DESTDIR)$(LIBDIR)/$(LIBESPEAK)
	# Install development headers
	$(INSTALL) -pm 644 speak_lib.h $(DESTDIR)$(INCDIR)
	# Install data files
	cp -prf ../espeak-data/* $(DESTDIR)$(DATADIR)

