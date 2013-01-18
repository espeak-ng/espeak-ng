PREFIX=@prefix@
BINDIR=@bindir@
INCDIR=@includedir@/espeak
LIBDIR=@libdir@
DATADIR=${PREFIX}/share/espeak-data

# remove -fno-exceptions  if it causes problems with stdio library on some platforms (ARM)

RELEASE = 1.46
SPEAK = speak
ESPEAK = espeak
ESPEAKEDIT = espeakedit
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

# AUDIO='runtime' uses pulseaudio if it is running, else uses portaudio

if AUDIO_RUNTIME
WAVE=wave.cpp wave_pulse.cpp
LIB_AUDIO=-lpulse -lpulse-simple -lportaudio
USE_AUDIO=-DUSE_PULSEAUDIO -DUSE_PORTAUDIO
else
if AUDIO_PULSEAUDIO
WAVE=wave_pulse.cpp
LIB_AUDIO=-lpulse
USE_AUDIO=-DUSE_PULSEAUDIO
else
if AUDIO_PORTAUDIO
WAVE=wave.cpp
LIB_AUDIO=-lportaudio
USE_AUDIO=-DUSE_PORTAUDIO
else
if AUDIO_PORTAUDIO0
WAVE=wave.cpp
LIB_AUDIO=/usr/lib/libportaudio.so.0
USE_AUDIO=-DUSE_PORTAUDIO
else
if AUDIO_PORTAUDIO2
WAVE=wave.cpp
LIB_AUDIO=/usr/lib/libportaudio.so.2
USE_AUDIO=-DUSE_PORTAUDIO
else
if AUDIO_SADA
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

CXXFLAGS = -O2 -Wall -c
CPPFLAGS = -pedantic -fno-exceptions -D PATH_ESPEAK_DATA=\"$(DATADIR)\"

.cpp.o:
	$(CXX) $(CXXFLAGS) $(USE_AUDIO) -I. $(CPPFLAGS) $<

e_%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(espeakedit_CXXFLAGS) $(USE_AUDIO) $(CPPFLAGS) -I. $< -o e_$*.o

x_%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(USE_AUDIO) -fpic -fvisibility=hidden \
	-I. -D LIBRARY $(CPPFLAGS) $< -o x_$*.o

all: $(SPEAK) $(LIBESPEAK) $(STATIC_LIBESPEAK) $(ESPEAK) $(ESPEAKEDIT)

common_SOURCE = compiledict.cpp dictionary.cpp intonation.cpp \
	klatt.cpp mbrowrap.cpp numbers.cpp readclause.cpp \
	phonemelist.cpp setlengths.cpp sonic.cpp \
	synthdata.cpp synthesize.cpp synth_mbrola.cpp \
	translate.cpp tr_languages.cpp voices.cpp wavegen.cpp 

speak_SOURCE = speak.cpp $(common_SOURCE)
speak_OBJS = ${speak_SOURCE:%.cpp=%.o}
speak_LIBS = -lstdc++ $(LIB_AUDIO) -lpthread $(EXTRA_LIBS)
$(SPEAK): $(speak_OBJS)
	$(CXX) $(LDFLAGS) -o $@ $(speak_OBJS) $(speak_LIBS)

espeak_SOURCE = espeak.cpp
espeak_OBJS = ${espeak_SOURCE:%.cpp=%.o}
espeak_LIBS = -lstdc++ -L . -lespeak
$(ESPEAK): $(espeak_OBJS) $(LIB_NAME)
	$(CXX) $(LDFLAGS) -o $@ $(espeak_OBJS) $(espeak_LIBS)

libespeak_SOURCE = speak_lib.cpp $(common_SOURCE) \
	espeak_command.cpp event.cpp fifo.cpp $(WAVE) debug.cpp
libespeak_OBJS = ${libespeak_SOURCE:%.cpp=x_%.o}
libespeak_LIBS = -lstdc++ $(LIB_AUDIO) -lpthread
$(LIBESPEAK): $(libespeak_OBJS)
	$(CXX) $(LDFLAGS) -shared $(SONAME_OPT)$(LIBESPEAK).$(LIB_VERSION) -o $@ \
	$(libespeak_OBJS) $(libespeak_LIBS)
	mv $(LIBESPEAK) $(LIBESPEAK).$(LIBTAG)
	$(LN_SF) $(LIBESPEAK).$(LIBTAG) $(LIBESPEAK).$(LIB_VERSION)
	$(LN_SF) $(LIBESPEAK).$(LIB_VERSION) $(LIBESPEAK)

$(STATIC_LIBESPEAK): $(libespeak_OBJS)
	$(AR) cqs $(STATIC_LIBESPEAK) $(libespeak_OBJS)


espeakedit_SOURCE = $(common_SOURCE) compiledata.cpp debug.cpp espeakedit.cpp \
	event.cpp extras.cpp fifo.cpp formantdlg.cpp \
	menus.cpp options.cpp prosodydisplay.cpp \
	espeak_command.cpp speak_lib.cpp spect.cpp \
	spectdisplay.cpp spectseq.cpp \
	transldlg.cpp voicedlg.cpp vowelchart.cpp $(WAVE)
espeakedit_OBJS=${espeakedit_SOURCE:%.cpp=e_%.o}
espeakedit_LIBS=-lstdc++ $(LIB_AUDIO)
espeakedit_CXXFLAGS = ${WX_CXXFLAGS} -D_LARGEFILE_SOURCE=1 -DNO_GCC_PRAGMA -D_ESPEAKEDIT
$(ESPEAKEDIT): $(espeakedit_OBJS)
	$(CXX) $(LDFLAGS) -o $@ $(espeakedit_OBJS) $(espeakedit_LIBS) ${WX_LIBS}

clean:
	rm -f *.o *.a *~

distclean: clean
	rm -f $(SPEAK)
	rm -f $(ESPEAK)
	rm -f $(LIBESPEAK)*
	rm -f $(STATIC_LIBESPEAK)
	rm -f $(ESPEAKEDIT)

install:
	rm -rf $(DESTDIR)$(DATADIR)
	$(MKDIR) $(DESTDIR)$(BINDIR)
	$(MKDIR) $(DESTDIR)$(LIBDIR)
	$(MKDIR) $(DESTDIR)$(INCDIR)
	$(MKDIR) $(DESTDIR)$(DATADIR)
	$(INSTALL) -m 755 $(ESPEAK) $(DESTDIR)$(BINDIR)
	$(INSTALL) -m 755 $(LIBESPEAK).$(LIBTAG) $(DESTDIR)$(LIBDIR)
	$(INSTALL) -m 755 $(STATIC_LIBESPEAK) $(DESTDIR)$(LIBDIR)
	$(LN_SF) $(LIBESPEAK).$(LIBTAG) $(DESTDIR)$(LIBDIR)/$(LIBESPEAK).$(LIB_VERSION)
	$(LN_SF) $(LIBESPEAK).$(LIB_VERSION) $(DESTDIR)$(LIBDIR)/$(LIBESPEAK)
	$(INSTALL) -pm 644 speak_lib.h $(DESTDIR)$(INCDIR)
	cp -prf ../espeak-data/* $(DESTDIR)$(DATADIR)

