#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>   // getopt
#include "imagen.h"
#include "faDeRuido.h"

// Entradas: argc y argv, usa el flag -d para saber si debe exportar los archivos de debug
// Salidas: 0 si todo salio bien
// Descripcion: tercer nodo del pipeline. Recibe la original y la preprocesada por el pipe, calcula el ruido (original - preprocesada) y, si esta el flag -d, exporta preprocesada.bin y ruido.bin. Mas adelante enviara la preprocesada a tHough
int main(int argc, char *argv[]) {
    int debug = 0;

    // este nodo solo necesita saber si viene el flag -d; los demas se reenviaran al siguiente nodo
    int opt;
    while ((opt = getopt(argc, argv, "i:r:t:v:o:d")) != -1) {
        if (opt == 'd') debug = 1;
    }

    // recibe la original y luego la preprocesada, en el mismo orden en que las mando preprocesamiento
    Image original = recv_image(STDIN_FILENO);
    Image preprocesada = recv_image(STDIN_FILENO);

    // ruido = original - preprocesada (los pixels que la apertura elimino)
    Image ruido = subtract_images(original, preprocesada);
    fprintf(stderr, "aDeRuido: ruido calculado (%d x %d)\n", ruido.width, ruido.height);

    // si el flag -d esta activo, exporta las imagenes intermedias
    if (debug) {
        write_image(preprocesada, "preprocesada.bin");
        write_image(ruido, "ruido.bin");
        fprintf(stderr, "aDeRuido: debug activo -> escritos preprocesada.bin y ruido.bin\n");
    }

    // TODO (Paso 5): enviar la preprocesada a tHough por un pipe

    free_image(original);
    free_image(preprocesada);
    free_image(ruido);
    return 0;
}
