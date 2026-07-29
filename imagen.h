#ifndef IMAGEN_H
#define IMAGEN_H

#include <stdint.h>

// estructura que representa la imagen en memoria
typedef struct {
    int width;
    int height;
    uint8_t *pixels; // arreglo plano de width*height bytes, cada byte vale 0 o 1
} Image;

// Entradas: la imagen a enviar y un descriptor de escritura (por ejemplo el extremo de escritura de un pipe)
// Salidas: ninguna
// Descripcion: escribe por el descriptor el width, el height y todos los pixels, para pasarle la imagen al siguiente proceso del pipeline
void send_image(Image img, int fd);

// Entradas: un descriptor de lectura (por ejemplo el extremo de lectura de un pipe)
// Salidas: Image reconstruida con los pixels en el heap
// Descripcion: lee del descriptor el width, el height y los pixels, reconstruyendo la imagen que mando el proceso anterior
Image recv_image(int fd);

// Entradas: imagen a liberar
// Salidas: ninguna
// Descripcion: libera la memoria del arreglo de pixels reservada con malloc
void free_image(Image img);

#endif
