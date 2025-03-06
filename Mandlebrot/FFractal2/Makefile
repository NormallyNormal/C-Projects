CC = gcc
CFLAGS = -lm

ffractalop: ffractal.o ppmrender.o gui.o
	gcc   ffractal.o ppmrender.o gui.o createspectrum.o -o ffractal -lm -lpthread -lncurses -O3
ffractal: ffractal.o ppmrender.o gui.o
	gcc   ffractal.o ppmrender.o gui.o createspectrum.o -o ffractal -lm -lpthread -lncurses -O0
ffractal.o: ffractal.c gui.h ppmrender.h
ppmrender.o: ppmrender.c createspectrum.o ppmrender.h createspectrum.h
createspectrum.o: createspectrum.c createspectrum.h
gui.o: gui.c gui.h
