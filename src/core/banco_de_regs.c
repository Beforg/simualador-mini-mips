#include "banco_de_regs.h"
#include <stdio.h>

int8_t ler_registrador(const CPU *cpu, uint8_t indice) {
    return cpu->banco_de_regs[indice];
}

void escrever_registrador(CPU *cpu, uint8_t indice, int8_t valor, SinaisDeControle sinais_de_controle) {
    if (sinais_de_controle.escrever_reg == 0) return;
    cpu->banco_de_regs[indice] = valor;
}

void imprimir_registradores(const CPU *cpu) {
    for (int i = 0; i < 8; i++) {
        printf("R%d: %d\n", i, cpu->banco_de_regs[i]);
    }
}