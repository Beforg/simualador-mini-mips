#include "controle.h"
#include "types.h"

static void gerar_sinais_add(SinaisDeControle *sinais_de_controle);
static void gerar_sinais_sub(SinaisDeControle *sinais_de_controle);
static void gerar_sinais_and(SinaisDeControle *sinais_de_controle);
static void gerar_sinais_or(SinaisDeControle *sinais_de_controle);
static void gerar_sinais_addi(SinaisDeControle *sinais_de_controle);
static void gerar_sinais_lw(SinaisDeControle *sinais_de_controle);
static void gerar_sinais_sw(SinaisDeControle *sinais_de_controle);
static void gerar_sinais_beq(SinaisDeControle *sinais_de_controle);
static void gerar_sinais_jump(SinaisDeControle *sinais_de_controle);

SinaisDeControle gerar_sinais_de_controle(uint8_t opcode, uint8_t funct) {
    SinaisDeControle sinais_de_controle;
    gerar(&sinais_de_controle, opcode, funct);
    return sinais_de_controle;
}

static void gerar(SinaisDeControle *sinais_de_controle, uint8_t opcode, uint8_t funct) {
    switch (opcode) {
        case OPCODE_R:
            switch (funct) {
                case FUNCT_ADD: // add
                    gerar_sinais_add(&sinais_de_controle);
                    break;
                case FUNCT_SUB: // sub
                    gerar_sinais_sub(&sinais_de_controle);
                    break;
                case FUNCT_AND: // and
                    gerar_sinais_and(&sinais_de_controle);
                    break;
                case FUNCT_OR: // or
                    gerar_sinais_or(&sinais_de_controle);
                    break;
                default:
                    // Função desconhecida para opcode R
                    break;
            }
            break;
        case OPCODE_I1: // addi
            gerar_sinais_addi(&sinais_de_controle);
            break;
        case OPCODE_I2: // lw
            gerar_sinais_lw(&sinais_de_controle);
            break;
        case OPCODE_I3: // sw
            gerar_sinais_sw(&sinais_de_controle);
            break;
        case OPCODE_I4: // beq
            gerar_sinais_beq(&sinais_de_controle);
            break;
        case OPCODE_J: // jump
            gerar_sinais_jump(&sinais_de_controle);
            break;
        default:
            // Opcode desconhecido
            break;
    }
}

static void gerar_sinais_add(SinaisDeControle *sinais_de_controle) {
    sinais_de_controle->controle_ula = 0b000; // Operação de adição
    sinais_de_controle->escrever_memoria = 0; // Não escreve na memória
    sinais_de_controle->escrever_reg = 1; // Escreve no registrador de destino
    sinais_de_controle->memoria_para_reg = 1; // O valor vem da ULA, não da memória, CUIDADO: Na arquitetura 1 vai da ula para o reg e 0 da memoria para o reg
    sinais_de_controle->ula_fonte = 0; // O segundo operando vem do registrador rt
    sinais_de_controle->reg_destino = 1; // O registrador de destino é rd
    sinais_de_controle->incremento_pc = 1; // Incrementa o PC normalmente
    sinais_de_controle->jump = 0; // Não é uma instrução de salto
}
static void gerar_sinais_sub(SinaisDeControle *sinais_de_controle) {}
static void gerar_sinais_and(SinaisDeControle *sinais_de_controle) {}
static void gerar_sinais_or(SinaisDeControle *sinais_de_controle) {}
static void gerar_sinais_addi(SinaisDeControle *sinais_de_controle) {}
static void gerar_sinais_lw(SinaisDeControle *sinais_de_controle) {}
static void gerar_sinais_sw(SinaisDeControle *sinais_de_controle) {}
static void gerar_sinais_beq(SinaisDeControle *sinais_de_controle) {}
static void gerar_sinais_jump(SinaisDeControle *sinais_de_controle) {}