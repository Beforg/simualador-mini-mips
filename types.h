#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

typedef struct {
 uint16_t pc;
 uint16_t memoria_de_instrucao[256];
 int8_t memoria_de_dados[256];
 int8_t banco_de_regs[8];
} CPU;

typedef struct {
  uint8_t opcode;
  uint8_t rs;
  uint8_t rt;
  uint8_t rd;
  uint8_t funct;
  uint8_t imediato;
  uint8_t endereco;
} InstrucaoDecodificada;

typedef struct {
  uint8_t controle_ula;
  uint8_t escrever_memoria;
  uint8_t escrever_reg;
  uint8_t memoria_para_reg;
  uint8_t ula_fonte;
  uint8_t reg_destino;
  uint8_t incremento_pc;
  uint8_t jump;
} SinaisDeControle;

typedef struct {
    int8_t resultado;
    uint8_t zero;
} ResultadoUla;

#endif
