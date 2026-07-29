#include "faDeRuido.h"
#include <stdio.h>
#include <stdlib.h>

// Entradas: imagen original con ruido, imagen preprocesada (resultado de la apertura)
// Salidas: nueva imagen con los pixels que fueron eliminados durante el preprocesamiento (el ruido)
// Descripcion: resta pixel a pixel original - preprocesada. Si la resta es positiva el pixel existia en la original pero no en la preprocesada (era ruido), y queda en 1; si no, queda en 0. Se clampea a 0 para no dejar valores negativos
Image subtract_images(Image original, Image preprocessed) {
    Image result;
    result.width = original.width;
    result.height = original.height;

    result.pixels = (uint8_t *)malloc(result.width * result.height);
    if (!result.pixels) {
        fprintf(stderr, "Error al reservar memoria para la resta\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < original.height; i++) {
        for (int j = 0; j < original.width; j++) {
            int idx = i * original.width + j; // indice en el arreglo plano

            int diff = original.pixels[idx] - preprocessed.pixels[idx];
            result.pixels[idx] = (diff > 0) ? 1 : 0;
        }
    }

    return result;
}
