#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>   // getopt, pipe, fork, dup2, execv
#include <sys/wait.h> // wait
#include "imagen.h"
#include "faDeRuido.h"

// Entradas: argc y argv, usa el flag -d para saber si debe exportar los archivos de debug
// Salidas: 0 si todo salio bien
// Descripcion: tercer nodo del pipeline. Recibe la original y la preprocesada por el pipe, calcula el ruido (original - preprocesada) y, si esta el flag -d, exporta preprocesada.bin y ruido.bin. Luego le manda la preprocesada a tHough (es la que tiene los bordes limpios sobre los que se vota, el ruido ya cumplio su funcion en el debug dump)
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

    // crea la tuberia hacia tHough
    int fd[2];
    if (pipe(fd) == -1) {
        fprintf(stderr, "aDeRuido: no se pudo crear el pipe\n");
        exit(EXIT_FAILURE);
    }

    // arma el argv para tHough reenviando los mismos flags que recibio este nodo
    char *args[argc + 1];
    args[0] = "./tHough";
    for (int k = 1; k < argc; k++) args[k] = argv[k];
    args[argc] = NULL;

    pid_t pid = fork();
    if (pid < 0) {
        fprintf(stderr, "aDeRuido: no se pudo crear el proceso con fork\n");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // proceso hijo -> tHough: su stdin sera el extremo de lectura del pipe
        dup2(fd[0], STDIN_FILENO);
        close(fd[0]);
        close(fd[1]);
        execv("./tHough", args);
        fprintf(stderr, "aDeRuido: no se pudo ejecutar tHough con execv\n");
        exit(EXIT_FAILURE);
    }

    // proceso padre -> aDeRuido: manda solo la preprocesada (la que tHough necesita para votar)
    close(fd[0]);
    send_image(preprocesada, fd[1]);
    close(fd[1]);

    wait(NULL); // espera a que tHough termine

    free_image(original);
    free_image(preprocesada);
    free_image(ruido);
    return 0;
}
