#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>   // getopt, fork, execv
#include <errno.h>
#include <limits.h>
#include <sys/wait.h> // wait, para esperar al proceso hijo

// Entradas: cadena s (el argumento de un flag) y puntero out donde se deja el resultado
// Salidas: 1 si s es un entero positivo (lo guarda en out), 0 si no lo es
// Descripcion: convierte s a entero con strtol y rechaza texto sobrante (ej: 4.5), cadenas vacias y valores menores o iguales a 0, asi no se acepta algo que atoi truncaria en silencio
static int parse_entero_positivo(const char *s, int *out) {
    if (s == NULL || *s == '\0') return 0;
    errno = 0;
    char *fin;
    long v = strtol(s, &fin, 10);
    // si fin no quedo apuntando a '\0' es porque sobraron caracteres sin convertir (ej: en 4.5 apunta a .5)
    if (errno != 0 || *fin != '\0') return 0;
    if (v <= 0 || v > INT_MAX) return 0;
    *out = (int)v;
    return 1;
}

// Entradas: nombre del programa (argv[0])
// Salidas: ninguna
// Descripcion: imprime por stderr el modo de uso del programa con la descripcion de cada flag
static void uso(const char *prog) {
    fprintf(stderr,
        "Uso: %s -i entrada.bin -r radio -t umbral [-v vecindad] [-o salida.csv] [-d]\n"
        "  -i  archivo de imagen de entrada (.bin)             (obligatorio)\n"
        "  -r  radio de los circulos, entero positivo          (obligatorio)\n"
        "  -t  umbral de votos, entero positivo                (obligatorio)\n"
        "  -v  tamano de vecindad para no-maximos, impar >= 1  (opcional, default 7)\n"
        "  -o  archivo de salida .csv                          (opcional, default reporte.csv)\n"
        "  -d  activa el volcado de debug (preprocesada.bin y ruido.bin)\n",
        prog);
}

// Entradas: argc y argv de la linea de comandos
// Salidas: 0 si todo salio bien, termina con EXIT_FAILURE si hay algun error
// Descripcion: nodo raiz del pipeline. Lee y valida los flags con getopt y luego lanza el primer nodo (cargaDatos) con fork y execv, esperando a que termine
int main(int argc, char *argv[]) {
    char *input_path = NULL;            // -i (obligatorio)
    char *output_path = "reporte.csv";  // -o (opcional, este es su default)
    int radius = 0;                     // -r (obligatorio)
    int threshold = 0;                  // -t (obligatorio)
    int vecindad = 7;                   // -v (opcional, default 7)
    int debug = 0;                      // -d (opcional, por defecto desactivado)

    // como 0 no es un valor valido para -r ni -t, usamos estas banderas para saber si el usuario los entrego
    int tiene_r = 0, tiene_t = 0;

    // getopt parsea los flags de la linea de comandos, los ':' marcan los que llevan argumento
    int opt;
    while ((opt = getopt(argc, argv, "i:r:t:v:o:d")) != -1) {
        switch (opt) {
            case 'i': input_path = optarg; break;
            case 'r':
                if (!parse_entero_positivo(optarg, &radius)) {
                    fprintf(stderr, "Error: -r (radio) debe ser un entero positivo. Recibido: %s\n", optarg);
                    exit(EXIT_FAILURE);
                }
                tiene_r = 1;
                break;
            case 't':
                if (!parse_entero_positivo(optarg, &threshold)) {
                    fprintf(stderr, "Error: -t (umbral) debe ser un entero positivo. Recibido: %s\n", optarg);
                    exit(EXIT_FAILURE);
                }
                tiene_t = 1;
                break;
            case 'v':
                // debe ser entero positivo y ademas impar para que el pixel tenga un centro exacto en la vecindad
                if (!parse_entero_positivo(optarg, &vecindad) || (vecindad % 2) == 0) {
                    fprintf(stderr, "Error: -v (vecindad) debe ser un entero impar >= 1. Recibido: %s\n", optarg);
                    exit(EXIT_FAILURE);
                }
                break;
            case 'o': output_path = optarg; break;
            case 'd': debug = 1; break;
            default: // flag desconocido o le falto el argumento
                uso(argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    // verificar que los flags obligatorios esten presentes
    if (input_path == NULL || !tiene_r || !tiene_t) {
        fprintf(stderr, "Error: Faltan argumentos obligatorios (-i, -r y -t)\n");
        uso(argv[0]);
        exit(EXIT_FAILURE);
    }

    // verificar que el archivo de entrada exista y se pueda leer, antes de lanzar todo el pipeline
    if (access(input_path, R_OK) != 0) {
        fprintf(stderr, "Error: -i no se pudo abrir el archivo de entrada: %s\n", input_path);
        exit(EXIT_FAILURE);
    }

    // se arma el arreglo de argumentos para cargaDatos. se pasan todos los flags porque los nodos siguientes tambien los necesitan
    char radio_str[16], umbral_str[16], vecindad_str[16];
    snprintf(radio_str, sizeof(radio_str), "%d", radius);
    snprintf(umbral_str, sizeof(umbral_str), "%d", threshold);
    snprintf(vecindad_str, sizeof(vecindad_str), "%d", vecindad);

    char *args[16];
    int n = 0;
    args[n++] = "./cargaDatos";
    args[n++] = "-i"; args[n++] = input_path;
    args[n++] = "-r"; args[n++] = radio_str;
    args[n++] = "-t"; args[n++] = umbral_str;
    args[n++] = "-v"; args[n++] = vecindad_str;
    args[n++] = "-o"; args[n++] = output_path;
    if (debug) args[n++] = "-d"; // el flag -d no lleva valor
    args[n] = NULL;              // execv necesita que el arreglo termine en NULL

    // fork clona el proceso: el hijo se convertira en cargaDatos y el padre (lab2) lo espera
    pid_t pid = fork();
    if (pid < 0) {
        fprintf(stderr, "Error: no se pudo crear el proceso con fork\n");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // proceso hijo: reemplaza su programa por cargaDatos
        execv("./cargaDatos", args);
        // si execv retorna es porque fallo (por ejemplo no encontro el ejecutable)
        fprintf(stderr, "Error: no se pudo ejecutar cargaDatos con execv\n");
        exit(EXIT_FAILURE);
    }

    // proceso padre (lab2): espera a que el hijo termine
    wait(NULL);

    return 0;
}
