#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>   // getopt, pipe, fork, dup2, execv
#include <sys/wait.h> // wait
#include "imagen.h"
#include "ftHough.h"

// Entradas: argc y argv, usa el flag -r (radio) para votar; los demas flags se reenviaran al siguiente nodo
// Salidas: 0 si todo salio bien
// Descripcion: cuarto nodo del pipeline. Recibe la preprocesada por el pipe, calcula el acumulador de votos de Hough (720 pasos) para el radio pedido, y se lo manda a resultados
int main(int argc, char *argv[]) {
    int radius = 0;

    int opt;
    while ((opt = getopt(argc, argv, "i:r:t:v:o:d")) != -1) {
        if (opt == 'r') radius = atoi(optarg);
    }

    // recibe la preprocesada por stdin (el pipe que dejo conectado aDeRuido)
    Image preprocesada = recv_image(STDIN_FILENO);

    int *accumulator = hough_accumulate(preprocesada, radius);

    // diagnostico para verificar el acumulador mientras no existe resultados (Paso 6) para recibirlo
    int max_votos = 0;
    for (int i = 0; i < preprocesada.width * preprocesada.height; i++) {
        if (accumulator[i] > max_votos) max_votos = accumulator[i];
    }
    fprintf(stderr, "tHough: acumulador listo (%d x %d), max votos = %d\n", preprocesada.width, preprocesada.height, max_votos);

    // crea la tuberia hacia resultados
    int fd[2];
    if (pipe(fd) == -1) {
        fprintf(stderr, "tHough: no se pudo crear el pipe\n");
        exit(EXIT_FAILURE);
    }

    // arma el argv para resultados reenviando los mismos flags que recibio este nodo
    char *args[argc + 1];
    args[0] = "./resultados";
    for (int k = 1; k < argc; k++) args[k] = argv[k];
    args[argc] = NULL;

    pid_t pid = fork();
    if (pid < 0) {
        fprintf(stderr, "tHough: no se pudo crear el proceso con fork\n");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // proceso hijo -> resultados: su stdin sera el extremo de lectura del pipe
        dup2(fd[0], STDIN_FILENO);
        close(fd[0]);
        close(fd[1]);
        execv("./resultados", args);
        fprintf(stderr, "tHough: no se pudo ejecutar resultados con execv\n");
        exit(EXIT_FAILURE);
    }

    // proceso padre -> tHough: manda el acumulador de votos
    close(fd[0]);
    send_int_array(accumulator, preprocesada.width, preprocesada.height, fd[1]);
    close(fd[1]);

    wait(NULL); // espera a que resultados termine

    free_image(preprocesada);
    free(accumulator);
    return 0;
}
