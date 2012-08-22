SRCS=$(wildcard *.cpp)
OBJS=$(patsubst %.cpp,%.o,$(SRCS))
LIBS=-lstdc++ -lportaudio
all: speak

.cpp.o:
	$(CXX) $(CXXFLAGS) -I. -c $<

speak: $(OBJS)
	$(CXX) -o $@ $(OBJS) $(LIBS)

clean:
	rm -f *.o

distclean: clean
	rm -f speak
