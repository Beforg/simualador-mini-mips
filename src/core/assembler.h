#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

// Macros definidos para labels

#define ADD_LBL "add"
#define SUB_LBL "sub"
#define BEQ_LBL "beq"
#define AND_LBL "and"
#define OR_LBL  "or"
#define ADDI_LBL "addi"
#define LW_LBL  "lw"
#define SW_LBL "sw"
#define J_LBL  "j"

#define R0_LBL "$r0"
#define R1_LBL "$r1"
#define R2_LBL "$r2"
#define R3_LBL "$r3"
#define R4_LBL "$r4"
#define R5_LBL "$r5"
#define R6_LBL "$r6"
#define R7_LBL "$r7"

// Labels definidas para tabelas hash.
// Hash para tabela de instruções.
#define ADD_HASH  0
#define SUB_HASH  1
#define AND_HASH  2
#define OR_HASH   3
#define ADDI_HASH 4
#define LW_HASH   5
#define SW_HASH   6
#define BEQ_HASH  7
#define J_HASH    8

// Hash para tabela de registradores
#define R0_HASH 0
#define R1_HASH 1
#define R2_HASH 2
#define R3_HASH 3
#define R4_HASH 4
#define R5_HASH 5
#define R6_HASH 6
#define R7_HASH 7


typedef struct{
    char nome[10];
    char tipo;
    unsigned int opcode;
    unsigned int funct;
}Instrucoes;

typedef struct{
    char lbl[4];    // Label do registrador
    int index;      // Valor inteiro do registrador.
}Registradores;

// Variáveis globais no assembler.c
extern FILE *input_file;                        // Ponteiro para arquivo de entrada.
extern FILE *output_file;                       // Ponteiro para arquivo de saída
extern Instrucoes tabela_instrucoes[];          // Struct de tabela
extern Registradores tabela_lbl_regs[];   
extern uint16_t instrucao_atual;                // Usando unsigned int com 16 bits, para processar.
extern bool debug_assembler;                    // Variável para controlar os prints.
void reset_controle_assembler();                // Reseta as flags de controle internas. Pode ser usada publicamente para evitar bugs;
void fechar_arquivos();                         // Fecha os arquivos de input e output.
void carregar_arq_asm(char *dirname);           // Carrega a referência para o ponteiro global de input_file
void processar_asm();                           // Realiza todo o processamento da parte do assembly.
void exportar_asm_bin(char *dirname);           // Exporta o arquivo asm

#endif