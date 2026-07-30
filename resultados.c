#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>   // getopt
#include "imagen.h"
#include "fresultados.h"

// Entradas: argc y argv, usa -v (vecindad), -t (umbral) y -o (archivo csv de salida)
// Salidas: 0 si todo salio bien
// Descripcion: quinto y ultimo nodo del pipeline. Recibe el acumulador de votos de Hough por el pipe, le aplica supresion de no-maximos en una vecindad v x v y escribe en el csv los centros cuyos votos superen el umbral
int main(int argc, char *argv[]) {
    int vecindad = 7;             // -v (opcional, default 7)
    int threshold = 0;            // -t (obligatorio en lab2, pero por si acaso se deja un default)
    char *output_path = "reporte.csv"; // -o (opcional, default reporte.csv)

    int opt;
    while ((opt = getopt(argc, argv, "i:r:t:v:o:d")) != -1) {
        switch (opt) {
            case 't':
                if (!parse_entero_positivo(optarg, &threshold)) {
                    fprintf(stderr, "resultados: -t (umbral) debe ser un entero positivo. Recibido: %s\n", optarg);
                    exit(EXIT_FAILURE);
                }
                break;
            case 'v':
                // debe ser positivo y ademas impar, para que la vecindad tenga un centro exacto
                if (!parse_entero_positivo(optarg, &vecindad) || (vecindad % 2) == 0) {
                    fprintf(stderr, "resultados: -v (vecindad) debe ser un entero impar >= 1. Recibido: %s\n", optarg);
                    exit(EXIT_FAILURE);
                }
                break;
            case 'o': output_path = optarg; break;
            default: break; // los demas flags (-i, -r, -d) no le sirven a este nodo
        }
    }

    // recibe el acumulador por stdin (el pipe que dejo conectado tHough)
    int width, height;
    int *accumulator = recv_int_array(STDIN_FILENO, &width, &height);

    int *suppressed = non_max_suppression(accumulator, width, height, vecindad);
    write_csv(suppressed, width, height, threshold, output_path);

    fprintf(stderr, "resultados: supresion de no-maximos lista (vecindad %d), csv escrito en %s\n", vecindad, output_path);

    free(accumulator);
    free(suppressed);
    return 0;
}
