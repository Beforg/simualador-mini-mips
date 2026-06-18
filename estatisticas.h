#ifndef ESTATISTICAS_H
#define ESTATISTICAS_H
#include "types.h"


typedef struct{
    int total_instrucoes;
    int total_tipo_j;
    int total_tipo_i;
    int total_tipo_r;
    float perc_j;
    float perc_i;
    float perc_r;
}Estatisticas;

// Varíavel global declarada no estatisticas.c
extern Estatisticas dados;

void reset_estatisticas();
void mostrar_estatisticas();

#endif