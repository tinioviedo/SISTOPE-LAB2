#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>   // getopt
#include "fcargaDatos.h"

// Entradas: argc y argv, usa el flag -i con la ruta de la imagen de entrada
// Salidas: 0 si todo salio bien
// Descripcion: primer nodo del pipeline. Carga la imagen .bin desde el archivo y por ahora solo muestra sus dimensiones. Mas adelante enviara la imagen al siguiente nodo por un pipe
int main(int argc, char *argv[]) {
    char *input_path = NULL;

    // este nodo solo necesita -i, pero acepta los demas flags para no fallar cuando lab2 se los pase
    int opt;
    while ((opt = getopt(argc, argv, "i:r:t:v:o:d")) != -1) {
        if (opt == 'i') input_path = optarg;
        // los demas flags los usaran los nodos siguientes, aca se ignoran
    }

    if (input_path == NULL) {
        fprintf(stderr, "cargaDatos: falta la ruta de entrada (-i)\n");
        exit(EXIT_FAILURE);
    }

    // carga la imagen desde el archivo binario
    Image img = read_image(input_path);
    printf("cargaDatos: imagen cargada desde %s (%d x %d, %d pixels)\n",
           input_path, img.width, img.height, img.width * img.height);

    free_image(img);
    return 0;
}
