EXENAME = guard
#CC = gcc
LIBS = -lrt -lrados -lncurses
CFLAGS = -o2
objects = guard.o utils.o module.o ceph.o in_ceph.o input.o cJSON.o output.o out_curses.o \
		  group.o element.o

$(EXENAME): $(objects)
	$(CC) -o $@ $^ $(LIBS)

$(objects): config.h guard.h utils.h module.h list.h input.h cJSON.h output.h out_curses.h \
	group.h element.h

guard.o: guard.c
	$(CC) -c $<

utils.o: utils.c
	$(CC) -c $<

module.o: module.c
	$(CC) -c $<

ceph.o: ceph.c
	$(CC) -c $<

in_ceph.o: in_ceph.c
	$(CC) -c $<

input.o: input.c
	$(CC) -c $<

cJSON.o: cJSON.c
	$(CC) -c $<

output.o: output.c
	$(CC) -c $<

out_curses.o: out_curses.c
	$(CC) -c $<

group.o: group.c
	$(CC) -c $<

element.o: element.c
	$(CC) -c $<

clean:
	$(RM) guard $(objects) tags
#guard: guard.o
#	gcc -o guard guard.o
#
#guard.o: guard.c guard.h config.h
#	gcc -c guard.c
#
#clean:
#	rm guard guard.o
#
