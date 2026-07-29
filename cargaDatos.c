#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>   // getopt, pipe, fork, dup2, execv
#include <sys/wait.h> // wait
#include "fcargaDatos.h"

// Entradas: argc y argv, usa el flag -i con la ruta de la imagen de entrada
// Salidas: 0 si todo salio bien
// Descripcion: primer nodo del pipeline. Carga la imagen .bin, crea un pipe hacia preprocesamiento, lo lanza con fork+execv y le envia la imagen por el pipe. El padre espera a que el hijo termine
int main(int argc, char *argv[]) {
    char *input_path = NULL;

    // este nodo usa -i para saber que archivo cargar
    int opt;
    while ((opt = getopt(argc, argv, "i:r:t:v:o:d")) != -1) {
        if (opt == 'i') input_path = optarg;
        // los demas flags no los usa este nodo, se reenvian tal cual mas abajo
    }

    if (input_path == NULL) {
        fprintf(stderr, "cargaDatos: falta la ruta de entrada (-i)\n");
        exit(EXIT_FAILURE);
    }

    // carga la imagen desde el archivo binario
    Image img = read_image(input_path);
    // el mensaje va a stderr porque stdout se usara para mandar la imagen por el pipe
    fprintf(stderr, "cargaDatos: imagen cargada (%d x %d)\n", img.width, img.height);

    // crea la tuberia: fd[0] es el extremo de lectura, fd[1] el de escritura
    int fd[2];
    if (pipe(fd) == -1) {
        fprintf(stderr, "cargaDatos: no se pudo crear el pipe\n");
        exit(EXIT_FAILURE);
    }

    // arma el argv para preprocesamiento reenviando los mismos flags que recibio este nodo
    char *args[argc + 1];
    args[0] = "./preprocesamiento";
    for (int k = 1; k < argc; k++) args[k] = argv[k];
    args[argc] = NULL; // execv necesita que el arreglo termine en NULL

    pid_t pid = fork();
    if (pid < 0) {
        fprintf(stderr, "cargaDatos: no se pudo crear el proceso con fork\n");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // proceso hijo -> preprocesamiento: su stdin sera el extremo de lectura del pipe
        dup2(fd[0], STDIN_FILENO);
        close(fd[0]);
        close(fd[1]);
        execv("./preprocesamiento", args);
        fprintf(stderr, "cargaDatos: no se pudo ejecutar preprocesamiento con execv\n");
        exit(EXIT_FAILURE);
    }

    // proceso padre -> cargaDatos: escribe la imagen por el extremo de escritura del pipe
    close(fd[0]);           // el padre no lee del pipe
    send_image(img, fd[1]); // manda width, height y pixels
    close(fd[1]);           // al cerrar, preprocesamiento recibe la senal de fin
    free_image(img);

    wait(NULL); // espera a que preprocesamiento termine
    return 0;
}
