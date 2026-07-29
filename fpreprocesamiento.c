#include "fpreprocesamiento.h"
#include <stdio.h>
#include <stdlib.h>

// Entradas: imagen original con ruido
// Salidas: nueva imagen erosionada con memoria propia en el heap
// Descripcion: por cada pixel revisa sus 5 vecinos segun el SE en cruz (centro, arriba, abajo, izquierda, derecha). Si alguno esta fuera del borde o es 0, el pixel resultado es 0. Asi elimina los pixels aislados de ruido
Image erosion(Image img) {
    Image result;
    result.width = img.width;
    result.height = img.height;

    result.pixels = (uint8_t *)malloc(result.width * result.height);
    if (!result.pixels) {
        fprintf(stderr, "Error al reservar memoria para la erosion\n");
        exit(EXIT_FAILURE);
    }

    // movimientos del SE en cruz: centro, arriba, abajo, izquierda, derecha
    int se_row[] = { 0, -1, 1,  0, 0};
    int se_col[] = { 0,  0, 0, -1, 1};

    for (int i = 0; i < img.height; i++) {
        for (int j = 0; j < img.width; j++) {

            uint8_t val = 1; // se asume que el pixel sobrevive; si algun vecino es 0 o esta fuera del borde, muere

            for (int k = 0; k < 5; k++) {
                int neigh_i = i + se_row[k]; // fila del vecino
                int neigh_j = j + se_col[k]; // columna del vecino

                if (neigh_i < 0 || neigh_i >= img.height || neigh_j < 0 || neigh_j >= img.width || img.pixels[neigh_i * img.width + neigh_j] == 0) {
                    val = 0;
                    break;
                }
            }

            result.pixels[i * result.width + j] = val;
        }
    }
    return result;
}

// Entradas: imagen erosionada
// Salidas: nueva imagen dilatada con memoria propia en el heap
// Descripcion: por cada pixel revisa sus 5 vecinos segun el SE en cruz. Si el o alguno de sus vecinos es 1, el pixel se enciende, restaurando el grosor de los bordes que la erosion adelgazo
Image dilation(Image img) {
    Image result;
    result.width = img.width;
    result.height = img.height;

    result.pixels = (uint8_t *)malloc(result.width * result.height);
    if (!result.pixels) {
        fprintf(stderr, "Error al reservar memoria para la dilatacion\n");
        exit(EXIT_FAILURE);
    }

    // movimientos del SE en cruz: centro, arriba, abajo, izquierda, derecha
    int se_row[] = { 0, -1, 1,  0, 0};
    int se_col[] = { 0,  0, 0, -1, 1};

    for (int i = 0; i < img.height; i++) {
        for (int j = 0; j < img.width; j++) {

            uint8_t val = 0; // se asume que el pixel muere; si algun vecino es 1, se enciende

            for (int k = 0; k < 5; k++) {
                int neigh_i = i + se_row[k]; // fila del vecino
                int neigh_j = j + se_col[k]; // columna del vecino

                if (neigh_i >= 0 && neigh_i < img.height && neigh_j >= 0 && neigh_j < img.width && img.pixels[neigh_i * img.width + neigh_j] == 1) {
                    val = 1;
                    break;
                }
            }

            result.pixels[i * result.width + j] = val;
        }
    }
    return result;
}
