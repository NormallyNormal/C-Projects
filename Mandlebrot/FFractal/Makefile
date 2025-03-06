CC = gcc
CFLAGS = -lm

ffractal: ffractal.o ppmrender.o gui.o
	gcc   ffractal.o ppmrender.o gui.o  -o ffractal -lm -lpthread -lncurses
ffractal.o: ffractal.c gui.h ppmrender.h
ppmrender.o: ppmrender.c ppmrender.h
gui.o: gui.c gui.h
