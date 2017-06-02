EXENAME = guard
#CC = gcc
LIBS = -lrt -lrados
CFLAGS = -o2
objects = guard.o utils.o module.o ceph.o in_ceph.o input.o cJSON.o output.o

$(EXENAME): $(objects)
	$(CC) -o $@ $^ $(LIBS)

$(objects): config.h guard.h utils.h module.h list.h input.h cJSON.h

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
