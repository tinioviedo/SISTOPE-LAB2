#include "imagen.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Entradas: descriptor de escritura, buffer origen y cantidad de bytes n
// Salidas: ninguna
// Descripcion: escribe exactamente n bytes en el descriptor. Se hace en un bucle porque un pipe puede aceptar menos bytes por llamada, asi nos aseguramos de mandar todo
static void write_full(int fd, const void *buf, int n) {
    const char *p = (const char *)buf;
    int escritos = 0;
    while (escritos < n) {
        int r = write(fd, p + escritos, n - escritos);
        if (r <= 0) {
            fprintf(stderr, "Error al escribir en el pipe\n");
            exit(EXIT_FAILURE);
        }
        escritos += r;
    }
}

// Entradas: descriptor de lectura, buffer destino y cantidad de bytes n
// Salidas: ninguna
// Descripcion: lee exactamente n bytes del descriptor. Igual que write_full, se hace en un bucle porque un pipe puede entregar los bytes de a poco
static void read_full(int fd, void *buf, int n) {
    char *p = (char *)buf;
    int leidos = 0;
    while (leidos < n) {
        int r = read(fd, p + leidos, n - leidos);
        if (r <= 0) {
            fprintf(stderr, "Error al leer del pipe\n");
            exit(EXIT_FAILURE);
        }
        leidos += r;
    }
}

// Entradas: la imagen a enviar y un descriptor de escritura
// Salidas: ninguna
// Descripcion: manda por el descriptor primero el width, luego el height y por ultimo todos los pixels
void send_image(Image img, int fd) {
    write_full(fd, &img.width, sizeof(int));
    write_full(fd, &img.height, sizeof(int));
    write_full(fd, img.pixels, img.width * img.height);
}

// Entradas: un descriptor de lectura
// Salidas: Image reconstruida con los pixels en el heap
// Descripcion: lee el width, el height y los pixels desde el descriptor y arma la imagen de vuelta
Image recv_image(int fd) {
    Image img;
    read_full(fd, &img.width, sizeof(int));
    read_full(fd, &img.height, sizeof(int));

    // reserva width*height bytes en el heap para los pixels que vienen a continuacion
    img.pixels = (uint8_t *)malloc(img.width * img.height);
    if (!img.pixels) {
        fprintf(stderr, "Error al reservar memoria para los pixels\n");
        exit(EXIT_FAILURE);
    }

    read_full(fd, img.pixels, img.width * img.height);
    return img;
}

// Entradas: imagen a liberar
// Salidas: ninguna
// Descripcion: libera la memoria del arreglo de pixels
void free_image(Image img) {
    free(img.pixels);
}
