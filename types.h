#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

#define INSTRUCAO_ASM_TAM 64

//novas estruturas:

typedef struct {
  uint8_t memoria_para_reg;
  uint8_t escrever_reg;
} ErSinais;

typedef struct {
  uint8_t escrever_memoria;
  uint8_t jump;
  uint8_t branch;
} MemSinais;

typedef struct {
  uint8_t controle_ula;
  uint8_t ula_fonte;
  uint8_t reg_destino;
} ExSinais;

typedef struct {
  uint8_t pc_mais_um;
  uint16_t ri;
  char instrucao_asm[INSTRUCAO_ASM_TAM];
} BI_DI;

typedef struct {
  //registrador ER:
  ErSinais er;
  //registrador MEM:
  MemSinais mem_sinais;
  //registrador EX:
  ExSinais ex_sinais;

  uint8_t opcode; 
  int8_t a;
  int8_t b;
  int8_t imediato;
  uint8_t pc_mais_um;
  uint8_t rd;
  uint8_t rt;
  uint8_t rs;
  char instrucao_asm[INSTRUCAO_ASM_TAM];
} DI_EX;

typedef struct {
  ErSinais er;
  MemSinais mem_sinais;
  uint8_t opcode; 
  int8_t ula_saida;
  int8_t b;
  uint8_t reg_destino; // rt ou rd
  char instrucao_asm[INSTRUCAO_ASM_TAM];
} EX_MEM;

typedef struct {
  ErSinais er;
  MemSinais mem_sinais;
  uint8_t opcode;
  int8_t memoria_saida;
  int8_t ula_saida;
  uint8_t reg_destino; // rt ou rd
  char instrucao_asm[INSTRUCAO_ASM_TAM];
} MEM_WB;

typedef enum {
  TIPO_R,
  TIPO_I,
  TIPO_J,
  NOP
} TipoInstrucao;

typedef enum {
  OPCODE_R = 0b0000,
  OPCODE_J = 0b0010,
  OPCODE_ADDI = 0b0100, // addi
  OPCODE_LW = 0b1011, // lw
  OPCODE_SW = 0b1111, // sw
  OPCODE_BEQ = 0b1000  // beq  
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
  JUMP = 0b000
} OperacaoUla;

typedef enum {
  FUNCT_ADD = 0b000,
  FUNCT_SUB = 0b010,
  FUNCT_AND = 0b100,
  FUNCT_OR = 0b101
} Funct;

typedef struct {
 uint8_t pc; // corrigido de 16 para 8
 uint16_t memoria_de_instrucao[256];
 int8_t memoria_de_dados[256];
 int8_t banco_de_regs[8];
 BI_DI bi_di;
 DI_EX di_ex;
 EX_MEM ex_mem;
 MEM_WB mem_wb;
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
    uint8_t overflow;
} ResultadoUla;

typedef struct No {
    CPU estado;       
    struct No *proximo;
} No;

typedef struct {
    No *topo;
} PilhaCPU;

#endif
