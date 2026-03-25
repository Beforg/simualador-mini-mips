
#include "utils.h"
#include <stdio.h>

int8_t extender_sinal(uint16_t instrucao) {
    uint8_t imm6 = (uint8_t)(instrucao & 0x3F); // extrai 6 bits

    // verifica se o 6º (5) bit (bit de sinal, máscara 0x20 que é 0010 0000) é 1
    if (imm6 & 0x20) {
        // Se for 1, precisamos preencher os bits 6 e 7 com '1'
        // 0xC0 em binário é 1100 0000. 
        // Usamos o operador OR (|) para "forçar" esses bits para 1.
        return (int8_t)(imm6 | 0xC0); 
    }
    return (int8_t)imm6;
}