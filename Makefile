CC = gcc
CFLAGS = -Wall -Wextra

all: lab2 cargaDatos

lab2: lab2.c
	$(CC) $(CFLAGS) -o lab2 lab2.c

cargaDatos: cargaDatos.c fcargaDatos.c fcargaDatos.h
	$(CC) $(CFLAGS) -o cargaDatos cargaDatos.c fcargaDatos.c

clean:
	rm -f lab2 cargaDatos
