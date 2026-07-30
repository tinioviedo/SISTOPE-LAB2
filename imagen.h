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

// Entradas: la imagen a guardar y la ruta del archivo destino
// Salidas: ninguna
// Descripcion: escribe la imagen en formato .bin (el width, el height y luego todos los pixels), el mismo formato que se usa para leerla
void write_image(Image img, const char *path);

// Entradas: arreglo de enteros a enviar, su width y height, y un descriptor de escritura
// Salidas: ninguna
// Descripcion: manda por el descriptor el width, el height y luego los width*height enteros. Mismo patron que send_image pero para el acumulador de Hough (int en vez de uint8_t)
void send_int_array(int *data, int width, int height, int fd);

// Entradas: un descriptor de lectura y punteros donde dejar el width y height recibidos
// Salidas: arreglo de enteros reconstruido en el heap (tamaño width*height)
// Descripcion: lee el width, el height y los enteros desde el descriptor y arma de vuelta el arreglo que mando el proceso anterior
int *recv_int_array(int fd, int *out_width, int *out_height);

// Entradas: imagen a liberar
// Salidas: ninguna
// Descripcion: libera la memoria del arreglo de pixels reservada con malloc
void free_image(Image img);

#endif
