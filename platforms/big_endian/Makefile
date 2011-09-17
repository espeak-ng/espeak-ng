CC = gcc

CFLAGS += -g3 -Wall -O0

all: espeak-phoneme-data

espeak-phoneme-data: espeak-phoneme-data.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f espeak-phoneme-data
