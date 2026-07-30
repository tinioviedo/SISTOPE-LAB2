#ifndef FTHOUGH_H
#define FTHOUGH_H

#include "imagen.h"

// Entradas: imagen preprocesada (bordes que sobrevivieron la apertura) y el radio r de los circulos a buscar
// Salidas: arreglo de enteros de tamaño width*height reservado en el heap, con los votos acumulados en cada posicion (indice y*width+x)
// Descripcion: Transformada de Hough para circunferencias de radio fijo r. Por cada pixel de borde vota en 720 posiciones del acumulador segun theta = 2*pi*k/720
int *hough_accumulate(Image img, int radius);

#endif
