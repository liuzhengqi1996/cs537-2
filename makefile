CC = gcc
WARNING_FLAGS = -Wall -Wextra -lpthread
EXE = prodcomm


all: prodcomm.o Queue.o
	$(CC) -o $(EXE) prodcomm.o Queue.o 

prodcomm.o: prodcomm.c Queue.h
	$(CC) $(WARNING_FLAGS) -c prodcomm.c

Queue.o: Queue.c Queue.h
	$(CC) $(WARNING_FLAGS) -c Queue.c

clean:
	rm $(EXE) *.o
