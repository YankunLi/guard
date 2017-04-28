EXENAME = guard
CC = gcc
LIBS = -lrt
CFLAGS = -o2
objects = guard.o utils.o

$(EXENAME): $(objects)
	$(CC) -o $@ $^ $(LIBS)

$(objects): config.h guard.h utils.h

guard.o: guard.c
	$(CC) -c $<

utils.o: utils.c
	$(CC) -c $<

clean:
	rm guard $(objects)
#guard: guard.o
#	gcc -o guard guard.o
#
#guard.o: guard.c guard.h config.h
#	gcc -c guard.c
#
#clean:
#	rm guard guard.o
#
