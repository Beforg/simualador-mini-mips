#ifndef MEMORIA_H
#define MEMORIA_H
#include "types.h"
#include <stdint.h>
#include <stdio.h>
#include "utils.h"

#pragma region CONSTANTES
// Região para declaração de constantes

typedef enum{
    DADOS,
    INSTRUCAO,
    REGISTRADOR
}TipoMemoria;

typedef enum {
    DECIMAL,
    HEXADECIMAL, 
    BINARIO
}OpcaoBase;

#pragma endregion CONSTANTES

// Modelagem:
// Entidades necessárias: Memória de dados e memória de programa

// ENTIDADE: MEMÓRIA DE DADOS : int8_t
// A memória deve ser capaz de:
// 1) Escrever em um endereço
// 2) Ler um endereço
// 3) Procurar valor e retornar **INTEIRO COM SINAL**




#pragma region MEMORIA

uint16_t ler_end_mem(const CPU *p,uint16_t addr); 
void escrever_end_mem(CPU *p,uint16_t addr,int8_t valor, SinaisDeControle sinais_de_controle); 

#pragma endregion MEMORIA

#pragma region REGISTRADORES

void escrever_registrador(CPU *p, uint8_t id,int8_t valor, SinaisDeControle sinais_de_controle);     // Escrever em registrador
int8_t ler_registrador(const CPU *p, uint8_t id);                 // Ler registrador

#pragma endregion REGISTRADORES

#pragma region FUNCOES_GENERICAS

void imprimirMemoria(const CPU *p, TipoMemoria tipo, OpcaoBase base);     // Mostra todos os dados da memória
void resetarMemoria(CPU *p, TipoMemoria tipo);      // Reseta a memória de acordo com o seu tipo, irá zerar todos os dados.

#pragma endregion FUNCOES_GENERICAS


#endif