# Very unsophisticated Makefile
DEBUG=-g

Pipesort: pipesort.o
	gcc $(DEBUG) -o pipesort pipesort.o

pipesort.o: pipesort.c
	gcc $(DEBUG) -c pipesort.c

clean: 
	rm pipesort pipesort.o