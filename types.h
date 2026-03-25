#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

typedef enum {
  TIPO_R,
  TIPO_I,
  TIPO_J,
  TIPO_INVALIDO
} TipoInstrucao;

typedef enum {
  OPCODE_R = 0b0000,
  OPCODE_J = 0b0010,
  OPCODE_I1 = 0b0100, // addi
  OPCODE_I2 = 0b1011, // lw
  OPCODE_I3 = 0b1111, // sw
  OPCODE_I4 = 0b1000  // beq  
} Opcode;

typedef struct {
  uint8_t registrador_a;
  uint8_t registrador_b;
} RegistradoresLidos;

typedef enum {
  OP_ADD = 0b000,
  OP_SUB = 0b010,
  OP_AND = 0b100,
  OP_OR = 0b101,
  OP_ADDI = 0b001,
  OP_LW = 0b011,
  OP_SW = 0b111,
  OP_BEQ = 0b110,
  JUMP = 0b100
} OperacaoUla;

typedef enum {
  FUNCT_ADD = 0b000,
  FUNCT_SUB = 0b010,
  FUNCT_AND = 0b100,
  FUNCT_OR = 0b101
} Funct;

typedef struct {
 uint16_t pc;
 uint16_t memoria_de_instrucao[256];
 int8_t memoria_de_dados[256];
 int8_t banco_de_regs[8];
} CPU;

typedef struct {
  TipoInstrucao tipo;
  uint8_t opcode;
  uint8_t rs;
  uint8_t rt;
  uint8_t rd;
  uint8_t funct;
  int8_t imediato;
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
  uint8_t branch;
} SinaisDeControle;

typedef struct {
    int8_t resultado;
    uint8_t zero;
} ResultadoUla;

#endif
