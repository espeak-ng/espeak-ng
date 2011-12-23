# remove -fno-exceptions  if it causes problems with stdio library on some platforms (ARM)

DATADIR=/usr/share/espeak-data

PREFIX=/usr
BINDIR=$(PREFIX)/bin
INCDIR=$(PREFIX)/include/espeak
LIBDIR=$(PREFIX)/lib

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

CXXFLAGS = -O2 -Wall -c
CPPFLAGS = -pedantic -fno-exceptions -D PATH_ESPEAK_DATA=\"$(DATADIR)\"

.cpp.o:
	# Compiling $@
	$(CXX) $(CXXFLAGS) $(USE_AUDIO) -I. $(CPPFLAGS) $<

e_%.o: %.cpp
	# Compiling $@
	$(CXX) $(CXXFLAGS) $(espeakedit_CXXFLAGS) $(USE_AUDIO) $(CPPFLAGS) -I. $< -o e_$*.o

x_%.o: %.cpp
	# Compiling $@
	$(CXX) $(CXXFLAGS) $(USE_AUDIO) -fpic -fvisibility=hidden \
	-I. -D LIBRARY $(CPPFLAGS) $< -o x_$*.o

all: $(SPEAK) $(LIBESPEAK) $(STATIC_LIBESPEAK) $(ESPEAK) $(ESPEAKEDIT)

common_SOURCES = compiledict.cpp dictionary.cpp intonation.cpp \
	klatt.cpp mbrowrap.cpp numbers.cpp readclause.cpp \
	phonemelist.cpp setlengths.cpp sonic.cpp \
	synthdata.cpp synthesize.cpp synth_mbrola.cpp \
	translate.cpp tr_languages.cpp voices.cpp wavegen.cpp 

speak_SOURCES = speak.cpp $(common_SOURCES)
speak_OBJS = $(patsubst %.cpp,%.o,$(speak_SOURCES))
speak_LIBS = -lstdc++ $(LIB_AUDIO) -lpthread $(EXTRA_LIBS)
$(SPEAK): $(speak_OBJS)
	# Linking $@
	$(CXX) $(LDFLAGS) -o $@ $(speak_OBJS) $(speak_LIBS)

espeak_SOURCES = espeak.cpp
espeak_OBJS = $(patsubst %.cpp,%.o,$(espeak_SOURCES))
espeak_LIBS = -lstdc++ -L . -lespeak
$(ESPEAK): $(espeak_OBJS) $(LIB_NAME)
	# Linking $@
	$(CXX) $(LDFLAGS) -o $@ $(espeak_OBJS) $(espeak_LIBS)

libespeak_SOURCES = speak_lib.cpp $(common_SOURCES) \
	espeak_command.cpp event.cpp fifo.cpp $(WAVE) debug.cpp
libespeak_OBJS = $(patsubst %.cpp,x_%.o,$(libespeak_SOURCES))
libespeak_LIBS = -lstdc++ $(LIB_AUDIO) -lpthread
$(LIBESPEAK): $(libespeak_OBJS)
	# Linking $@
	$(CXX) $(LDFLAGS) -shared $(SONAME_OPT)$(LIBESPEAK).$(LIB_VERSION) -o $@ \
	$(libespeak_OBJS) $(libespeak_LIBS)
	mv $(LIBESPEAK) $(LIBESPEAK).$(LIBTAG)
	$(LN_SF) $(LIBESPEAK).$(LIBTAG) $(LIBESPEAK).$(LIB_VERSION)
	$(LN_SF) $(LIBESPEAK).$(LIB_VERSION) $(LIBESPEAK)

$(STATIC_LIBESPEAK): $(libespeak_OBJS)
	# Linking $@
	$(AR) cqs $(STATIC_LIBESPEAK) $(libespeak_OBJS)


espeakedit_SOURCES = $(common_SOURCES) compiledata.cpp debug.cpp espeakedit.cpp \
	event.cpp extras.cpp fifo.cpp formantdlg.cpp \
	menus.cpp options.cpp prosodydisplay.cpp \
	espeak_command.cpp speak_lib.cpp spect.cpp \
	spectdisplay.cpp spectseq.cpp \
	transldlg.cpp voicedlg.cpp vowelchart.cpp $(WAVE)
espeakedit_OBJS=$(patsubst %.cpp,e_%.o,$(espeakedit_SOURCES))
espeakedit_WX_LIBS = -pthread   -lwx_gtk2u_xrc-2.6 -lwx_gtk2u_qa-2.6 -lwx_gtk2u_html-2.6 \
	-lwx_gtk2u_adv-2.6 -lwx_gtk2u_core-2.6 -lwx_baseu_xml-2.6 -lwx_baseu_net-2.6 -lwx_baseu-2.6
espeakedit_LIBS=-lstdc++ $(LIB_AUDIO)
espeakedit_CXXFLAGS = -I/usr/lib/wx/include/gtk2-unicode-release-2.6 \
	-I/usr/include/wx-2.6 \
	-DGTK_NO_CHECK_CASTS -D__WXGTK__ -D_FILE_OFFSET_BITS=64 \
	-D_LARGE_FILES -D_LARGEFILE_SOURCE=1 -DNO_GCC_PRAGMA -D_ESPEAKEDIT
$(ESPEAKEDIT): $(espeakedit_OBJS)
	# Linking $@
	$(CXX) $(LDFLAGS) -o $@ $(espeakedit_OBJS) $(espeakedit_LIBS) $(espeakedit_WX_LIBS)

clean:
	rm -f *.o *.a *~

distclean: clean
	rm -f $(SPEAK)
	rm -f $(ESPEAK)
	rm -f $(LIBESPEAK)*
	rm -f $(STATIC_LIBESPEAK)
	rm -f $(ESPEAKEDIT)

install: all
	# Create directories
	rm -rf $(DESTDIR)$(DATADIR)
	$(MKDIR) $(DESTDIR)$(BINDIR)
	$(MKDIR) $(DESTDIR)$(LIBDIR)
	$(MKDIR) $(DESTDIR)$(INCDIR)
	$(MKDIR) $(DESTDIR)$(DATADIR)
	# Install espeak executable
	$(INSTALL) -m 755 $(ESPEAK) $(DESTDIR)$(BINDIR)
	# Should we install espeakedit executable?
	# $(INSTALL) -m 755 $(ESPEAKEDIT) $(DESTDIR)$(BINDIR)
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

