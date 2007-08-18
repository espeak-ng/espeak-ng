CC = gcc

CFLAGS += -g -Wall

all: espeak-phoneme-data

espeak-phoneme-data: espeak-phoneme-data.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f espeak-phoneme-data
