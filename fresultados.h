#ifndef FRESULTADOS_H
#define FRESULTADOS_H

// Entradas: acumulador de votos de Hough (arreglo de enteros), su width y height, y el tamaño v de la vecindad (impar)
// Salidas: nuevo arreglo de enteros reservado en el heap, del mismo tamaño, con la supresion de no-maximos aplicada
// Descripcion: por cada posicion, si no es el maximo dentro de su vecindad v x v (centrada en esa posicion), la deja en 0 en el resultado. Se calcula sobre una copia para no alterar el acumulador original mientras se recorre
int *non_max_suppression(int *accumulator, int width, int height, int v);

// Entradas: arreglo con la supresion de no-maximos ya aplicada, su width y height, el umbral de votos, y la ruta del csv de salida
// Salidas: ninguna
// Descripcion: recorre el arreglo y escribe en el csv (con cabecera X,Y) las coordenadas de las posiciones cuyo valor de votos sea mayor o igual al umbral
void write_csv(int *suppressed, int width, int height, int threshold, const char *csv_path);

#endif
