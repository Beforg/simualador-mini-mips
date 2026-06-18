# ifndef UTILS_H
# define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>
# include "core/types.h"

void print_int_4bits(int int_generico);
void print_int_3bits(int int_generico);
int8_t extender_sinal(uint16_t instrucao);
void int8_para_binario(int8_t valor);
void int16_para_binario(int16_t valor);

int8_t binario_para_int8(const char *binario);
uint8_t binario_para_int8_sem_sinal(const char *binario);

int16_t binario_para_int16(const char *binario);
uint16_t binario_para_int16_sem_sinal(const char *binario);

void int8_hexa(int8_t valor);
void int16_hexa(int16_t valor);
char *strtrim(char *str); // Realiza o trim de strings no mesmo buffer.
int arquivo_vazio(const char *filename);

void push_cpu(PilhaCPU *p, const CPU *cpu);
int pop_cpu(PilhaCPU *p, CPU *cpu);
void inicializar_pilha(PilhaCPU *p);
# endif
