#include "ftHough.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Entradas: imagen preprocesada y el radio r
// Salidas: arreglo de enteros (heap) de tamaño width*height con los votos del acumulador
// Descripcion: portado del hough() del Lab 1, pero con 720 pasos de theta (en vez de 360) y sin escribir el CSV: solo arma y devuelve el acumulador, la supresion de no-maximos y la escritura del CSV las hace el nodo resultados (Paso 6)
int *hough_accumulate(Image img, int radius) {
    int W = img.width;
    int H = img.height;

    int *accumulator = (int *)calloc((size_t)W * H, sizeof(int));
    if (!accumulator) {
        fprintf(stderr, "Error al asignar memoria para el acumulador\n");
        exit(EXIT_FAILURE);
    }

    int grades = 720; // theta = 2*pi*k/720, segun el enunciado

    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
            if (img.pixels[i * W + j] == 0) continue;

            for (int k = 0; k < grades; k++) {
                double theta = 2.0 * M_PI * k / grades;

                int x = (int)round(j - radius * cos(theta));
                int y = (int)round(i - radius * sin(theta));

                if (x < 0 || x >= W || y < 0 || y >= H) continue;

                accumulator[y * W + x]++;
            }
        }
    }

    return accumulator;
}
