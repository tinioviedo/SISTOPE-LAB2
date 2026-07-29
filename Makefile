CC = gcc
CFLAGS = -Wall -Wextra

all: lab2 cargaDatos preprocesamiento

lab2: lab2.c
	$(CC) $(CFLAGS) -o lab2 lab2.c

cargaDatos: cargaDatos.c fcargaDatos.c imagen.c fcargaDatos.h imagen.h
	$(CC) $(CFLAGS) -o cargaDatos cargaDatos.c fcargaDatos.c imagen.c

preprocesamiento: preprocesamiento.c fpreprocesamiento.c imagen.c fpreprocesamiento.h imagen.h
	$(CC) $(CFLAGS) -o preprocesamiento preprocesamiento.c fpreprocesamiento.c imagen.c

clean:
	rm -f lab2 cargaDatos preprocesamiento
