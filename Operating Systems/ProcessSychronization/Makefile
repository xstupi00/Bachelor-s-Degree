 # File:		Makefile
 # Author:		Šimon Stupinský
 # University: 		Brno University of Technology
 # Faculty: 		Faculty of Information Technology
 # Course:		Operating Systems
 # Date:		23.04.2017
 # Last change:		1.5.2017			
 # Subscribe:	 	Makefile - gcc

CC = gcc
CFLAGS=-std=gnu99 -Wall -Wextra -Werror -pedantic
LFLAGS=-lpthread

all: proj2

proj2: proj2.o shr_mem.o semaphores.o
	$(CC) $(CFLAGS) $^ -o $@ $(LFLAGS)

proj2.o: proj2.c proj2.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LFLAGS)

shr_mem.o: shr_mem.c shr_mem.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LFLAGS)

semaphores.o: semaphores.c semaphores.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LFLAGS)

.PHONY:
	clean pack make
clean:
	rm -f *.o proj2
pack:
	zip proj2.zip *.c *.h  Makefile
