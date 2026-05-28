# include "controle.h"
# include "types.h"

static void gerar_sinais_add(SinaisDeControle *sinais_de_controle);
static void gerar_sinais_sub(SinaisDeControle *sinais_de_controle);
static void gerar_sinais_and(SinaisDeControle *sinais_de_controle);
static void gerar_sinais_or(SinaisDeControle *sinais_de_controle);
static void gerar_sinais_addi(SinaisDeControle *sinais_de_controle);
static void gerar_sinais_lw(SinaisDeControle *sinais_de_controle);
static void gerar_sinais_sw(SinaisDeControle *sinais_de_controle);
static void gerar_sinais_beq(SinaisDeControle *sinais_de_controle);
static void gerar_sinais_jump(SinaisDeControle *sinais_de_controle);
static void gerar_sinais_default_r(SinaisDeControle *sinais_de_controle);
static void gerar_sinais_default_i(SinaisDeControle *sinais_de_controle);
static void gerar(SinaisDeControle *sinais_de_controle, uint8_t opcode, uint8_t funct);

SinaisDeControle gerar_sinais_de_controle(uint8_t opcode, uint8_t funct) {
    SinaisDeControle sinais_de_controle = {0}; // Zera tudo
    gerar(&sinais_de_controle, opcode, funct);
    return sinais_de_controle;
}

static void gerar(SinaisDeControle *sinais_de_controle, uint8_t opcode, uint8_t funct) {
    switch (opcode) {
        case OPCODE_R:  
            switch (funct) {
                case FUNCT_ADD: // add
                    gerar_sinais_add(sinais_de_controle);
                    break;
                case FUNCT_SUB: // sub
                    gerar_sinais_sub(sinais_de_controle);
                    break;
                case FUNCT_AND: // and
                    gerar_sinais_and(sinais_de_controle);
                    break;
                case FUNCT_OR: // or
                    gerar_sinais_or(sinais_de_controle);
                    break;
                default: // funct invalido (nop)
                    *sinais_de_controle = (SinaisDeControle){0};
                    break;
            }
            break;
        case OPCODE_ADDI: // addi
            gerar_sinais_addi(sinais_de_controle);
            break;
        case OPCODE_LW: // lw
            gerar_sinais_lw(sinais_de_controle);
            break;
        case OPCODE_SW: // sw
            gerar_sinais_sw(sinais_de_controle);
            break;
        case OPCODE_BEQ: // beq
            gerar_sinais_beq(sinais_de_controle);
            break;
        case OPCODE_J: // jump
            gerar_sinais_jump(sinais_de_controle);
            break;
        default: // opcode invalido (nop)
            *sinais_de_controle = (SinaisDeControle){0};
            break;
    }
}

static void gerar_sinais_add(SinaisDeControle *sinais_de_controle) {
    sinais_de_controle->controle_ula = OP_ADD; // Operação de adição
    gerar_sinais_default_r(sinais_de_controle);
}

static void gerar_sinais_sub(SinaisDeControle *sinais_de_controle) {
    sinais_de_controle->controle_ula = OP_SUB; // Operação de subtração
    gerar_sinais_default_r(sinais_de_controle);
}

static void gerar_sinais_and(SinaisDeControle *sinais_de_controle) {
    sinais_de_controle->controle_ula = OP_AND; // Operação de AND
    gerar_sinais_default_r(sinais_de_controle);
}

static void gerar_sinais_or(SinaisDeControle *sinais_de_controle) {
    sinais_de_controle->controle_ula = OP_OR; // Operação de OR
    gerar_sinais_default_r(sinais_de_controle);
}

static void gerar_sinais_addi(SinaisDeControle *sinais_de_controle) {
    sinais_de_controle->controle_ula = OP_ADDI;
    sinais_de_controle->ula_fonte = 1;
    sinais_de_controle->memoria_para_reg = 1;
    sinais_de_controle->escrever_reg = 1;
    sinais_de_controle->escrever_memoria = 0;
    sinais_de_controle->branch = 0;
    gerar_sinais_default_i(sinais_de_controle);
}

static void gerar_sinais_lw(SinaisDeControle *sinais_de_controle) {
    sinais_de_controle->controle_ula = OP_LW;
    sinais_de_controle->ula_fonte = 1;
    sinais_de_controle->memoria_para_reg = 0;
    sinais_de_controle->escrever_reg = 1;
    sinais_de_controle->escrever_memoria = 0;
    sinais_de_controle->branch = 0;
    gerar_sinais_default_i(sinais_de_controle);
}

static void gerar_sinais_sw(SinaisDeControle *sinais_de_controle) {
    sinais_de_controle->controle_ula = OP_SW;
    sinais_de_controle->ula_fonte = 1;
    sinais_de_controle->memoria_para_reg = 0; // Não importa para sw
    sinais_de_controle->escrever_reg = 0;
    sinais_de_controle->escrever_memoria = 1;
    sinais_de_controle->branch = 0;
    gerar_sinais_default_i(sinais_de_controle);
}

static void gerar_sinais_beq(SinaisDeControle *sinais_de_controle) {
    sinais_de_controle->controle_ula = OP_BEQ; // Operação de comparação (subtração)
    sinais_de_controle->ula_fonte = 0;
    sinais_de_controle->memoria_para_reg = 0;
    sinais_de_controle->escrever_reg = 0;
    sinais_de_controle->escrever_memoria = 0;
    sinais_de_controle->branch = 1; // Habilita o controle de desvio
    gerar_sinais_default_i(sinais_de_controle);
}

static void gerar_sinais_default_r(SinaisDeControle *sinais_de_controle) {
    sinais_de_controle->incremento_pc = 1; // Incrementa o PC normalmente
    sinais_de_controle->reg_destino = 1; // O registrador de destino é rd
    sinais_de_controle->ula_fonte = 0; // O segundo operando vem do registrador rt
    sinais_de_controle->memoria_para_reg = 1; // CUIDADO: Na arquitetura, 1 vai da ula(resultado) para reg.
    sinais_de_controle->escrever_reg = 1; // Escreve no registrador de destino
    sinais_de_controle->escrever_memoria = 0; // Não escreve na memória
    sinais_de_controle->jump = 0; // Não é uma instrução de salto
    sinais_de_controle->branch = 0; // Não é uma instrução de desvio
}

static void gerar_sinais_default_i(SinaisDeControle *sinais_de_controle) {
    sinais_de_controle->incremento_pc = 1; // Incrementa o PC normalmente
    sinais_de_controle->reg_destino = 0;
    sinais_de_controle->jump = 0;
}

static void gerar_sinais_jump(SinaisDeControle *sinais_de_controle) {
    sinais_de_controle->controle_ula = JUMP; // Não importa para jump
    sinais_de_controle->incremento_pc = 1; // O PC é atualizado com o endereço de salto, não incrementado normalmente
    sinais_de_controle->reg_destino = 0; // Não importa para jump
    sinais_de_controle->ula_fonte = 0; // O segundo operando vem do registrador rt
    sinais_de_controle->memoria_para_reg = 0; // Não importa para jump
    sinais_de_controle->escrever_reg = 0;
    sinais_de_controle->escrever_memoria = 0;
    sinais_de_controle->jump = 1; // Habilita o controle de salto
    sinais_de_controle->branch = 0; // Não é uma instrução de desvio
}
