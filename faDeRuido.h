#ifndef FADERUIDO_H
#define FADERUIDO_H

#include "imagen.h"

// Entradas: imagen original y imagen preprocesada (del mismo tamano)
// Salidas: nueva imagen con el ruido (memoria propia en el heap)
// Descripcion: resta pixel a pixel original - preprocesada. El resultado son los pixels que estaban en la original pero que no sobrevivieron la apertura, es decir el ruido
Image subtract_images(Image original, Image preprocessed);

#endif
