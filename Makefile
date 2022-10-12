CC = gcc
CFLAGS = -Wall -pedantic -g -Werror

all: mytalk
mytalk: mytalk.o mytalk.h
	gcc -L ~pn-cs357/Given/Talk/lib64 -o mytalk mytalk.o -ltalk -lncurses
mytalk.o: mytalk.c
	gcc $(CFLAGS) -c -I ~pn-cs357/Given/Talk/include -o mytalk.o mytalk.c
clean: 
	rm -f mytalk.o


