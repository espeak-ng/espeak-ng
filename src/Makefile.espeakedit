# Makefile for 'espeakedit' program

SRCS= compiledata.cpp compiledict.cpp debug.cpp dictionary.cpp espeak_command.cpp \
    espeakedit.cpp event.cpp extras.cpp fifo.cpp formantdlg.cpp intonation.cpp \
    klatt.cpp mbrowrap.cpp menus.cpp numbers.cpp options.cpp phonemelist.cpp \
    prosodydisplay.cpp readclause.cpp setlengths.cpp speak_lib.cpp spect.cpp \
    spectdisplay.cpp spectseq.cpp synthdata.cpp synthesize.cpp synth_mbrola.cpp \
    translate.cpp transldlg.cpp tr_languages.cpp voicedlg.cpp voices.cpp vowelchart.cpp \
    wave.cpp wavegen.cpp sonic.cpp

OBJS=$(patsubst %.cpp,%.o,$(SRCS))

WX_LIBS = -pthread   `wx-config --libs`

LIBS=-lstdc++ -lportaudio
#LIBS=-lstdc++ /usr/lib/x86_64-linux-gnu/libportaudio.so.2

CPPFLAGS =  -Wall -g -fexceptions `wx-config --cflags`
	-I/usr/include/wx-2.8 \
	-DGTK_NO_CHECK_CASTS -D__WXGTK__ -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES
	-D_LARGEFILE_SOURCE=1 -DNO_GCC_PRAGMA -D_ESPEAKEDIT

CXXFLAGS = -O2  -Wall -fexceptions `wx-config --cflags` \
   -DGTK_NO_CHECK_CASTS -D__WXGTK__ -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES -D_LARGEFILE_SOURCE=1 -DNO_GCC_PRAGMA -D_ESPEAKEDIT

all: espeakedit

.cpp.o:
	$(CXX) $(CXXFLAGS) -I. -c -fno-exceptions $<

espeakedit: $(OBJS)
	$(CXX) -o $@ $(OBJS) $(LIBS) $(WX_LIBS)

clean:
	rm -f *.o *~

distclean: clean
	rm -f espeakedit

