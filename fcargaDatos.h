#ifndef FCARGADATOS_H
#define FCARGADATOS_H

#include <stdint.h>

// estructura que representa la imagen en memoria
typedef struct {
    int width;
    int height;
    uint8_t *pixels; // arreglo plano de width*height bytes, cada byte vale 0 o 1
} Image;

// Entradas: ruta del archivo .bin
// Salidas: Image con los datos cargados (pixels en el heap)
// Descripcion: lee el encabezado (width, height) y los pixels desde el archivo binario
Image read_image(const char *path);

// Entradas: imagen a liberar
// Salidas: ninguna
// Descripcion: libera la memoria del arreglo de pixels
void free_image(Image img);

#endif
