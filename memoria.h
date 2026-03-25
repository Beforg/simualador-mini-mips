#ifndef MEMORIA_H
#define MEMORIA_H

#include <stdint.h>
#include "types.h"

uint16_t ler_memoria_instrucao(const CPU *cpu, uint16_t endereco);
int8_t ler_memoria_dados(const CPU *cpu, uint8_t endereco);
void escrever_memoria_dados(CPU *cpu, uint16_t endereco, int8_t valor, SinaisDeControle sinais_de_controle);
void imprimir_memoria_instrucao(const CPU *cpu);
void imprimir_memoria_dados(const CPU *cpu);

#endif