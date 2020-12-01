# Very unsophisticated Makefile
DEBUG=-g

test: test.o my_malloc.o
	gcc $(DEBUG) -o test test.o my_malloc.o

test.o: my_malloc.h test.c
	gcc $(DEBUG) -c test.c

my_malloc.o: my_malloc.h my_malloc.c
	gcc $(DEBUG) -c my_malloc.c 

clean: 
	rm test test.o my_malloc.o
