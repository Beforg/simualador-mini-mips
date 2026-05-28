
#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

typedef struct Estatisticas {
  int total_instrucoes;
  int total_instrucoes_executadas;
  int total_tipo_j;
  int total_tipo_i;
  int total_tipo_r;

  int total_tipo_j_executadas;
  int total_tipo_i_executadas;
  int total_tipo_r_executadas;

  int total_ciclos;
  // Vamos guardar o total de ciclos para cada instrucao separadamente.
  int total_ciclos_add;
  int total_ciclos_sub;
  int total_ciclos_or;
  int total_ciclos_and;
  int total_ciclos_addi;
  int total_ciclos_lw;
  int total_ciclos_sw;
  int total_ciclos_beq;
  int total_ciclos_j;

  int total_add_executadas;
  int total_sub_executadas;
  int total_or_executadas;
  int total_and_executadas;
  int total_addi_executadas;
  int total_lw_executadas;
  int total_sw_executadas;
  int total_beq_executadas;
  int total_j_executadas;

  // Aqui, sao quantas vezes aparecem a instrucao.
  int total_add;
  int total_sub;
  int total_or;
  int total_and;
  int total_addi;
  int total_lw;
  int total_sw;
  int total_beq;
  int total_j;

  // Porcentagens
  float perc_j;
  float perc_i;
  float perc_r;
  float cpi;
} Estatisticas;
typedef enum {
  TIPO_R,
  TIPO_I,
  TIPO_J,
  TIPO_INVALIDO
} TipoInstrucao;

typedef enum EstadosControle {
  IF = 0,
  ID = 1,
  EX_MEM_IMM = 2,
  LW_ACESSO_MEM = 3,
  MEM_WB = 4,
  SW_ACESSO_MEM = 5,
  END_ADDI = 6,
  EX_TIPO_R = 7,
  END_TIPO_R = 8,
  EX_BRANCH = 9,
  EX_JUMP = 10,
} EstadosControle;

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
 uint16_t pc;
 uint16_t ri;
 uint16_t rdm;
 int8_t a;
 int8_t b;
 int8_t saida_ula;
 uint16_t memoria_principal[256]; // netoe1: Uso de apenas uma memória principal.
 int8_t banco_de_regs[8];
 EstadosControle estado_atual;
 Estatisticas estatistica; // Ponteiro para módulo de estatística.
} CPU;

typedef struct InstrucaoDecodificada {
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
  uint8_t ula_fonte_a;
  uint8_t ula_fonte_b;
  uint8_t reg_destino;
  uint8_t incremento_pc;
  uint8_t pc_fonte;
  uint8_t ir_escrever;
  uint8_t i_ou_d;
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
