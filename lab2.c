#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>   // getopt
#include <errno.h>
#include <limits.h>

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
// Salidas: 0 si los parametros son validos, termina con EXIT_FAILURE si hay algun error
// Descripcion: nodo raiz del pipeline. Por ahora solo lee y valida los flags con getopt. En el paso 2 este main lanzara el primer proceso del pipeline (cargaDatos)
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

    // parametros se leyeron y validaron bien
    printf("Configuracion leida correctamente:\n");
    printf("  -i (entrada)  : %s\n", input_path);
    printf("  -r (radio)    : %d\n", radius);
    printf("  -t (umbral)   : %d\n", threshold);
    printf("  -v (vecindad) : %d\n", vecindad);
    printf("  -o (salida)   : %s\n", output_path);
    printf("  -d (debug)    : %s\n", debug ? "activado" : "desactivado");

    return 0;
}
