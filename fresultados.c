#include "fresultados.h"
#include <stdio.h>
#include <stdlib.h>

// Entradas: acumulador de votos, su width y height, y el tamaño v de la vecindad (impar)
// Salidas: nuevo arreglo (heap) con la supresion de no-maximos aplicada
// Descripcion: por cada pixel revisa su vecindad v x v sobre el acumulador ORIGINAL (nunca sobre el resultado, para no pisar valores mientras se recorre). Si algun vecino tiene mas votos que el, el pixel se apaga (0); si es el maximo de su vecindad, conserva su valor
int *non_max_suppression(int *accumulator, int width, int height, int v) {
    int *result = (int *)malloc((size_t)width * height * sizeof(int));
    if (!result) {
        fprintf(stderr, "Error al asignar memoria para la supresion de no-maximos\n");
        exit(EXIT_FAILURE);
    }

    int radio = v / 2; // v es impar, asi que el pixel queda exactamente al centro de la vecindad

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            int val = accumulator[i * width + j];
            int es_maximo = 1;

            for (int di = -radio; di <= radio && es_maximo; di++) {
                for (int dj = -radio; dj <= radio; dj++) {
                    int ni = i + di;
                    int nj = j + dj;
                    if (ni < 0 || ni >= height || nj < 0 || nj >= width) continue;
                    if (di == 0 && dj == 0) continue;

                    if (accumulator[ni * width + nj] > val) {
                        es_maximo = 0;
                        break;
                    }
                }
            }

            result[i * width + j] = es_maximo ? val : 0;
        }
    }

    return result;
}

// Entradas: arreglo con la supresion de no-maximos aplicada, su width y height, el umbral y la ruta del csv
// Salidas: ninguna
// Descripcion: crea el csv con cabecera X,Y y escribe una linea por cada posicion cuyo valor de votos sea >= al umbral
void write_csv(int *suppressed, int width, int height, int threshold, const char *csv_path) {
    FILE *csv = fopen(csv_path, "w");
    if (!csv) {
        fprintf(stderr, "Error al crear el archivo csv: %s\n", csv_path);
        exit(EXIT_FAILURE);
    }

    fprintf(csv, "X,Y\n");
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (suppressed[i * width + j] >= threshold) {
                fprintf(csv, "%d,%d\n", j, i);
            }
        }
    }

    fclose(csv);
}
