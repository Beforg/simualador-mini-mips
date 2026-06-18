#ifndef BANCO_DE_REGS_H
#define BANCO_DE_REGS_H

#include <stdint.h>
#include "types.h"

int8_t ler_registrador(const CPU *cpu, uint8_t indice);
void escrever_registrador(CPU *cpu, uint8_t indice, int8_t valor, SinaisDeControle sinais_de_controle);
void imprimir_registradores(const CPU *cpu);

#endif