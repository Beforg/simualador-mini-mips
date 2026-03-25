#include "memoria.h"
#include <stdio.h>

uint16_t ler_memoria_instrucao(const CPU *cpu, uint16_t endereco_apontado) {
    return cpu->memoria_de_instrucao[endereco_apontado];
}

int8_t ler_memoria_dados(const CPU *cpu, uint8_t endereco) {
    if (endereco < 0) return cpu->memoria_de_dados[0];
    return cpu->memoria_de_dados[endereco];
}

void escrever_memoria_dados(CPU *cpu, uint16_t endereco, int8_t valor, SinaisDeControle sinais_de_controle) {
    if (sinais_de_controle.escrever_memoria == 0) return;
    cpu->memoria_de_dados[endereco] = valor;
}

void imprimir_memoria_instrucao(const CPU *cpu) {
    for (int i = 0; i < 256; i++) {
        printf("Instrucao %d: %u\n", i, cpu->memoria_de_instrucao[i]);
    }
}

void imprimir_memoria_dados(const CPU *cpu) {
    for (int i = 0; i < 256; i++) {
        printf("Dado %d: %d\n", i, cpu->memoria_de_dados[i]);
    }
}

