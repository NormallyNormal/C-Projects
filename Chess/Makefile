CC = gcc
CFLAGS = -lm -O3

chess: main.o perft.o boardbuilder.o moves.o ai.o opening_book.o
	gcc   main.o perft.o boardbuilder.o moves.o ai.o opening_book.o -o chess -lm -O3
main.o: main.c perft.h boardbuilder.h
moves.o: moves.c moves.h board.h
perft.o: perft.c moves.o
boardbuilder.o: boardbuilder.c boardbuilder.h board.h
ai.o: ai.c ai.h board.h moves.h
opening_book.o: opening_book.c opening_book.h board.h moves.h

clean:
	rm -f *.o
	rm -f chess
