#ifndef FCARGADATOS_H
#define FCARGADATOS_H

#include "imagen.h"

// Entradas: ruta del archivo .bin
// Salidas: Image con los datos cargados (pixels en el heap)
// Descripcion: lee el encabezado (width, height) y los pixels desde el archivo binario
Image read_image(const char *path);

#endif
