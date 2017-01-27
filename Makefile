all: filter_chirp

filter_chirp: filter_chirp.c
	gcc -o filter_chirp filter_chirp.c -lc -lm -lliquid
