#include <stdio.h>
#include "conversor.h"

void int16_para_binario(int16_t valor) {
    char binario[17];
    for (int posicao_bit = 15; posicao_bit >= 0; posicao_bit--) {
        binario[15 - posicao_bit] = (valor & (1 << posicao_bit)) ? '1' : '0';
    }
    binario[16] = '\0';
    printf("%s BINARIO | %i DECIMAL\n", binario, valor);
}

void int8_para_binario(int8_t valor) {
	char binario[9];
	for (int posicao_bit = 7; posicao_bit >= 0; posicao_bit--) {
		binario[7 - posicao_bit] = (valor & (1 << posicao_bit)) ? '1' : '0';
	}
	
	binario[9] = '\0';
	printf("%s BINARIO\n %i DECIMAL", binario, valor);
}

int8_t binario_para_int8(const char *binario) {
    return (int8_t)strtoul(binario, NULL, 2);
}

uint8_t binario_para_int8_sem_sinal(const char *binario) {
    return (uint8_t)strtoul(binario, NULL, 2);
}

int16_t binario_para_int16(const char *binario) {
    return (int16_t)strtoul(binario, NULL, 2);
}


uint16_t binario_para_int16_sem_sinal(const char *binario) {
    return (uint16_t)strtoul(binario, NULL, 2);
}
