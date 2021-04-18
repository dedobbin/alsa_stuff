.PHONY: all clean

all: alsa.o
	gcc -g main.c *.o -lasound

alsa.o: alsa.c alsa.h
	gcc -g -c alsa.c -o alsa.o

clean:
	rm -f *.o *.out