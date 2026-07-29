#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>   // pipe, fork, dup2, execv
#include <sys/wait.h> // wait
#include "imagen.h"
#include "fpreprocesamiento.h"

// Entradas: argc y argv (reenvia los flags al siguiente nodo, la imagen le llega por el pipe)
// Salidas: 0 si todo salio bien
// Descripcion: segundo nodo del pipeline. Recibe la imagen original por el pipe, le aplica la apertura (erosion seguida de dilatacion), y le manda al nodo aDeRuido la original Y la preprocesada (aDeRuido necesita las dos para calcular el ruido)
int main(int argc, char *argv[]) {
    // recibe la imagen original por stdin (el pipe que dejo conectado cargaDatos)
    Image original = recv_image(STDIN_FILENO);

    // apertura: primero erosion (elimina el ruido aislado), luego dilatacion (restaura el grosor)
    Image eroded = erosion(original);
    Image preprocesada = dilation(eroded);
    free_image(eroded);
    fprintf(stderr, "preprocesamiento: apertura lista (%d x %d)\n", original.width, original.height);

    // crea la tuberia hacia aDeRuido
    int fd[2];
    if (pipe(fd) == -1) {
        fprintf(stderr, "preprocesamiento: no se pudo crear el pipe\n");
        exit(EXIT_FAILURE);
    }

    // arma el argv para aDeRuido reenviando los mismos flags que recibio este nodo
    char *args[argc + 1];
    args[0] = "./aDeRuido";
    for (int k = 1; k < argc; k++) args[k] = argv[k];
    args[argc] = NULL;

    pid_t pid = fork();
    if (pid < 0) {
        fprintf(stderr, "preprocesamiento: no se pudo crear el proceso con fork\n");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // proceso hijo -> aDeRuido: su stdin sera el extremo de lectura del pipe
        dup2(fd[0], STDIN_FILENO);
        close(fd[0]);
        close(fd[1]);
        execv("./aDeRuido", args);
        fprintf(stderr, "preprocesamiento: no se pudo ejecutar aDeRuido con execv\n");
        exit(EXIT_FAILURE);
    }

    // proceso padre -> preprocesamiento: manda la original y luego la preprocesada por el pipe
    close(fd[0]);
    send_image(original, fd[1]);
    send_image(preprocesada, fd[1]);
    close(fd[1]);
    free_image(original);
    free_image(preprocesada);

    wait(NULL); // espera a que aDeRuido termine
    return 0;
}
