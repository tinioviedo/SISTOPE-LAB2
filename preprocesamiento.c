#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "imagen.h"
#include "fpreprocesamiento.h"

// Entradas: argc y argv (por ahora no usa flags, la imagen le llega por el pipe)
// Salidas: 0 si todo salio bien
// Descripcion: segundo nodo del pipeline. Recibe la imagen por el pipe, le aplica la apertura (erosion seguida de dilatacion) para limpiar el ruido, y por ahora reporta cuanto ruido elimino. Mas adelante enviara la original y la preprocesada a aDeRuido
int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    // recibe la imagen original por stdin (el pipe que dejo conectado cargaDatos)
    Image original = recv_image(STDIN_FILENO);

    // apertura: primero erosion (elimina el ruido aislado), luego dilatacion (restaura el grosor)
    Image eroded = erosion(original);
    Image preprocesada = dilation(eroded);
    free_image(eroded);

    // cuenta los pixels blancos antes y despues, para ver cuanto ruido se limpio
    int blancos_orig = 0, blancos_prep = 0;
    int total = original.width * original.height;
    for (int i = 0; i < total; i++) {
        if (original.pixels[i]) blancos_orig++;
        if (preprocesada.pixels[i]) blancos_prep++;
    }
    fprintf(stderr, "preprocesamiento: apertura lista. Blancos: original %d -> preprocesada %d (ruido eliminado: %d)\n",
            blancos_orig, blancos_prep, blancos_orig - blancos_prep);

    // TODO (Paso 4): enviar la original y la preprocesada a aDeRuido por un pipe

    free_image(original);
    free_image(preprocesada);
    return 0;
}
