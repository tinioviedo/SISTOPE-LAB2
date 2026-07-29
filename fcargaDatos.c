#include "fcargaDatos.h"
#include <stdio.h>
#include <stdlib.h>

// Entradas: ruta del archivo .bin a leer
// Salidas: Image con width, height y pixels cargados en memoria heap
// Descripcion: abre el archivo binario, lee 4 bytes para el width y 4 para el height, reserva memoria dinamica para width*height pixels y los carga
Image read_image(const char *path) {
    FILE *file = fopen(path, "rb");
    if (!file) {
        fprintf(stderr, "Error al abrir el archivo: %s\n", path);
        exit(EXIT_FAILURE);
    }

    Image img;
    fread(&img.width, sizeof(int), 1, file);   // lee 4 bytes correspondientes al ancho
    fread(&img.height, sizeof(int), 1, file);  // lee 4 bytes correspondientes al alto

    // reserva width*height bytes en el heap para almacenar los pixels
    img.pixels = (uint8_t *)malloc(img.width * img.height);
    if (!img.pixels) {
        fprintf(stderr, "Error al reservar memoria para los pixels\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    fread(img.pixels, sizeof(uint8_t), img.width * img.height, file); // carga todos los pixels
    fclose(file);
    return img;
}

// Entradas: imagen a liberar
// Salidas: ninguna
// Descripcion: libera la memoria del arreglo de pixels reservada con malloc
void free_image(Image img) {
    free(img.pixels);
}
