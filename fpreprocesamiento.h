#ifndef FPREPROCESAMIENTO_H
#define FPREPROCESAMIENTO_H

#include "imagen.h"

// Entradas: imagen original
// Salidas: nueva imagen erosionada (memoria propia en el heap)
// Descripcion: aplica erosion con el SE en cruz 3x3. Un pixel sobrevive solo si el y sus 4 vecinos (arriba, abajo, izquierda, derecha) son 1
Image erosion(Image img);

// Entradas: imagen erosionada
// Salidas: nueva imagen dilatada (memoria propia en el heap)
// Descripcion: aplica dilatacion con el SE en cruz 3x3. Un pixel se enciende si el o alguno de sus 4 vecinos es 1, restaurando el grosor que la erosion adelgazo
Image dilation(Image img);

#endif
