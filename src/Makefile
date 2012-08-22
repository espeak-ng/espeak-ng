SRCS=$(wildcard *.cpp)
OBJS=$(patsubst %.cpp,%.o,$(SRCS))
LIBS=-lstdc++ -lportaudio
all: speak

.cpp.o:
	$(CXX) $(CXXFLAGS) -O2 -Wall -pedantic -I. -c -fno-exceptions $<

speak: $(OBJS)
	$(CXX) -o $@ $(OBJS) $(LIBS)

clean:
	rm -f *.o *~

distclean: clean
	rm -f speak
