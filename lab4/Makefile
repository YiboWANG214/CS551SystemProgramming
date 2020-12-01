# Very unsophisticated Makefile
#DEBUG=-g

all: manage compute report
.PHONY: clean

manage: manage.o
	gcc $(DEBUG) -o manage manage.o

manage.o: defs.h manage.c
	gcc $(DEBUG) -c manage.c

report: report.o
	gcc $(DEBUG) -o report report.o

report.o: defs.h report.c
	gcc $(DEBUG) -c report.c

compute: compute.o
	gcc $(DEBUG) -o compute compute.o

compute.o: defs.h compute.c
	gcc $(DEBUG) -c compute.c

clean: 
	rm compute compute.o manage manage.o report report.o
