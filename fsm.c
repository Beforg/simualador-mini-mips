#include "fsm.h"
# include "types.h"

#include <stdio.h>

// netoe1: Vou deixar comentado pro compilador não ficar enchendo o saco,
// se quiser aproveitar o código, só descomentar
static void gerar_sinais_estado_if(SinaisDeControle *sinais_de_controle);
static void gerar_sinais_estado_id(SinaisDeControle *sinais_de_controle);
static void gerar_sinais_addr_calc(SinaisDeControle *sinais_de_controle);
static void gerar_sinais_acesso_lw_sw_memoria(SinaisDeControle *sinais_de_controle);
static void gerar_sinais_estado_escrita_reg_rt(SinaisDeControle *sinais_de_controle);
static void gerar_sinais_estado_sw_acesso_mem(SinaisDeControle *sinais_de_controle);
static void gerar_sinais_estado_end_addi(SinaisDeControle *sinais_de_controle);
static void gerar_sinais_estado_ex_r(SinaisDeControle *sinais_de_controle, uint8_t funct);
static void gerar_sinais_estado_end_r(SinaisDeControle *sinais_de_controle);
static void gerar_sinais_estado_ex_branch(SinaisDeControle *sinais_de_controle);
static void gerar_sinais_estado_ex_jump(SinaisDeControle *sinais_de_controle);
// Sobre a variável global:
// Sugestão: FSM fsm_g;


void gerar_sinais(SinaisDeControle *sinais_de_controle, uint8_t funct, CPU *cpu){
    // Aqui vamos primeiro gerar os sinais do estado, para depois aplicar as transições:
    // Seguindo a lógica:
    // Todos os estados
    // 0 - Busca da instrução
    // 1 - Decodificação
    // 2 - Cálculo de endereço
    // 3 - Acesso à memória (lw)
    // 4 - Write-back (lw)
    // 5 - Acesso à memória (sw)
    // 6 - Write-back (addi)
    // 7 - Execução tipo R
    // 8 - Write-back tipo R
    // 9 - Branch (beq)
    // 10 - Jump

    switch (cpu->estado_atual) {  
        case S0_IF:
            gerar_sinais_estado_if(sinais_de_controle); 
            break;

        case S1_ID:
            gerar_sinais_estado_id(sinais_de_controle);
            break;

        case S2_ADDR_CALC:
         gerar_sinais_addr_calc(sinais_de_controle);
            break;

        case S3_MEM_READ:
            gerar_sinais_acesso_lw_sw_memoria(sinais_de_controle);
            break;

        case S4_MEM_WRITEBACK:
            gerar_sinais_estado_escrita_reg_rt(sinais_de_controle);
            break;

        case S5_MEM_WRITE:
            gerar_sinais_estado_sw_acesso_mem(sinais_de_controle);
            break;

        case S6_ADDI_WRITEBACK:
            gerar_sinais_estado_end_addi(sinais_de_controle);
            break;
        
        case S7_EXEC_R:
            gerar_sinais_estado_ex_r(sinais_de_controle, funct);
            break;

        case S8_R_WRITEBACK:
            gerar_sinais_estado_end_r(sinais_de_controle);
            break;

        case S9_BRANCH:
            gerar_sinais_estado_ex_branch(sinais_de_controle);
            break;

        case S10_JUMP:
            gerar_sinais_estado_ex_jump(sinais_de_controle);
            break;

        default:
            puts("mini-mips: Estado não reconhecido, erro na função gerarSinais() - fsm.c!");
            *sinais_de_controle = (SinaisDeControle){0};
            break;
    }
}

static void gerar_sinais_estado_if(SinaisDeControle *sinais_de_controle) {
    *sinais_de_controle = (SinaisDeControle){
        .controle_ula = 0, // Não importa para IF
        .escrever_memoria = 0,
        .escrever_reg = 0,
        .memoria_para_reg = 0,
        .ula_fonte_a = 0,
        .ula_fonte_b = 1,
        .reg_destino = 1,
        .incremento_pc = 1, // Incrementa o PC para buscar a próxima instrução
        .pc_fonte = 0, // O próximo PC vem do incremento normal
        .ir_escrever = 1, // Escreve a instrução lida no RI
        .i_ou_d = 0, // Não importa para IF
        .jump = 0, // Não é uma instrução de salto
        .branch = 0 // Não é uma instrução de desvio
    };
}

static void gerar_sinais_estado_id(SinaisDeControle *sinais_de_controle) {
    *sinais_de_controle = (SinaisDeControle){
        .controle_ula = OP_ADD, // Calcula PC + imediato
        .escrever_memoria = 0,
        .escrever_reg = 0,
        .memoria_para_reg = 0,
        .ula_fonte_a = 0,
        .ula_fonte_b = 2,
        .reg_destino = 1,
        .incremento_pc = 0,
        .pc_fonte = 0,
        .ir_escrever = 0,
        .i_ou_d = 0,
        .jump = 0,
        .branch = 0
    };
}
static void gerar_sinais_estado_sw_acesso_mem(SinaisDeControle *sinais_de_controle) {
    *sinais_de_controle = (SinaisDeControle){
        .controle_ula = 0,
        .escrever_memoria = 1,
        .escrever_reg = 0,
        .memoria_para_reg = 0,
        .ula_fonte_a = 1,
        .ula_fonte_b = 2,
        .reg_destino = 0,
        .incremento_pc = 0,
        .pc_fonte = 0,
        .ir_escrever = 0,
        .i_ou_d = 1,
        .jump = 0,
        .branch = 0
    };
}

static void gerar_sinais_estado_end_addi(SinaisDeControle *sinais_de_controle) {
    *sinais_de_controle = (SinaisDeControle){
        .controle_ula = 0,
        .escrever_memoria = 0,
        .escrever_reg = 1,
        .memoria_para_reg = 0,
        .ula_fonte_a = 1,
        .ula_fonte_b = 2,
        .reg_destino = 0,
        .incremento_pc = 0,
        .pc_fonte = 0,
        .ir_escrever = 0,
        .i_ou_d = 0,
        .jump = 0,
        .branch = 0
    };
}

static void gerar_sinais_estado_ex_r(SinaisDeControle *sinais_de_controle, uint8_t funct) {
    *sinais_de_controle = (SinaisDeControle){
        .controle_ula = funct,
        .escrever_memoria = 0,
        .escrever_reg = 0,
        .memoria_para_reg = 0,
        .ula_fonte_a = 1,
        .ula_fonte_b = 0,
        .reg_destino = 1,
        .incremento_pc = 0,
        .pc_fonte = 0,
        .ir_escrever = 0,
        .i_ou_d = 0,
        .jump = 0,
        .branch = 0 
    };
}

static void gerar_sinais_estado_end_r(SinaisDeControle *sinais_de_controle) {
    *sinais_de_controle = (SinaisDeControle){
        .controle_ula = 0,
        .escrever_memoria = 0,
        .escrever_reg = 1,
        .memoria_para_reg = 0,
        .ula_fonte_a = 1,
        .ula_fonte_b = 0,
        .reg_destino = 1,
        .incremento_pc = 0,
        .pc_fonte = 0,
        .ir_escrever = 0,
        .i_ou_d = 0,
        .jump = 0,
        .branch = 0 
    };
}

static void gerar_sinais_estado_ex_branch(SinaisDeControle *sinais_de_controle) {
    *sinais_de_controle = (SinaisDeControle){
        .controle_ula = OP_BEQ,
        .escrever_memoria = 0,
        .escrever_reg = 0,
        .memoria_para_reg = 0,
        .ula_fonte_a = 1,
        .ula_fonte_b = 0,
        .reg_destino = 0,
        .incremento_pc = 0,
        .pc_fonte = 1,
        .ir_escrever = 0,
        .i_ou_d = 0,
        .jump = 0,
        .branch = 1
    };
}

static void gerar_sinais_estado_ex_jump(SinaisDeControle *sinais_de_controle) {
    *sinais_de_controle = (SinaisDeControle){
        .controle_ula = JUMP,
        .escrever_memoria = 0,
        .escrever_reg = 0,
        .memoria_para_reg = 0,
        .ula_fonte_a = 0,
        .ula_fonte_b = 0,
        .reg_destino = 0,
        .incremento_pc = 0,
        .pc_fonte = 2,
        .ir_escrever = 0,
        .i_ou_d = 0,
        .jump = 1,
        .branch = 0
    };
}

// Estado 2
static void gerar_sinais_addr_calc(SinaisDeControle *sinais_de_controle){
    *sinais_de_controle = (SinaisDeControle){
        .controle_ula = OP_ADD,     //0
        .escrever_memoria = 0,      //x
        .escrever_reg = 0,          //x
        .memoria_para_reg = 0,      //x
        .ula_fonte_a = 1,           //1
        .ula_fonte_b = 2,           //2
        .reg_destino = 0,           //x
        .incremento_pc = 0,         //x
        .pc_fonte = 0,              //x
        .ir_escrever = 0,           //x
        .i_ou_d = 0,                //x
        .jump = 0,                  //x
        .branch = 0                 //x
    };
}

// Estado 3
static void gerar_sinais_acesso_lw_sw_memoria(SinaisDeControle *sinais_de_controle){
      *sinais_de_controle = (SinaisDeControle){
        .controle_ula = OP_ADD,     //x
        .escrever_memoria = 0,      //0
        .escrever_reg = 0,          //x
        .memoria_para_reg = 0,      //x
        .ula_fonte_a = 1,           //1
        .ula_fonte_b = 2,           //2
        .reg_destino = 0,           //x
        .incremento_pc = 0,         //x
        .pc_fonte = 0,              //x
        .ir_escrever = 0,           //x
        .i_ou_d = 1,                //1
        .jump = 0,                  //x
        .branch = 0                 //x
    };
}

// Estado 4:
static void gerar_sinais_estado_escrita_reg_rt(SinaisDeControle *sinais_de_controle){
    *sinais_de_controle = (SinaisDeControle){
        .controle_ula = OP_ADD,     //x
        .escrever_memoria = 0,      //x
        .escrever_reg = 1,          //1
        .memoria_para_reg = 1,      //1
        .ula_fonte_a = 1,           //1
        .ula_fonte_b = 2,           //2
        .reg_destino = 0,           //0
        .incremento_pc = 0,         //x
        .pc_fonte = 0,              //x
        .ir_escrever = 0,           //x
        .i_ou_d = 0,                //x
        .jump = 0,                  //x
        .branch = 0                 //x
    };
}