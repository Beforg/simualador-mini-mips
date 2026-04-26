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
static void gerar(SinaisDeControle *sinais_de_controle, uint8_t funct, CPU *cpu);

static void gerar_sinais_estado_if(SinaisDeControle *sinais_de_controle);
static void gerar_sinais_estado_id(SinaisDeControle *sinais_de_controle);
static void gerar_sinais_estado_ex_mem_imm(SinaisDeControle *sinais_de_controle);
static void gerar_sinais_estado_lw_acesso_mem(SinaisDeControle *sinais_de_controle);
static void gerar_sinais_estado_mem_wb(SinaisDeControle *sinais_de_controle);
static void gerar_sinais_estado_sw_acesso_mem(SinaisDeControle *sinais_de_controle);
static void gerar_sinais_estado_end_addi(SinaisDeControle *sinais_de_controle);
static void gerar_sinais_estado_ex_tipo_r(SinaisDeControle *sinais_de_controle, uint8_t funct);
static void gerar_sinais_estado_end_tipo_r(SinaisDeControle *sinais_de_controle);
static void gerar_sinais_estado_ex_branch(SinaisDeControle *sinais_de_controle);
static void gerar_sinais_estado_ex_jump(SinaisDeControle *sinais_de_controle);

SinaisDeControle gerar_sinais_de_controle(uint8_t opcode, uint8_t funct, CPU *cpu) {
    SinaisDeControle sinais_de_controle = {0}; // Zera tudo
    gerar(&sinais_de_controle, funct, cpu);
    return sinais_de_controle;
}

EstadosControle proximo_estado(EstadosControle estado_atual, uint8_t opcode) {
  if(estado_atual == IF) return ID;
  if (estado_atual == ID) {
    switch (opcode) {
        case OPCODE_LW:
        case OPCODE_SW:
        case OPCODE_ADDI:
            return EX_MEM_IMM;
        case OPCODE_R:
            return EX_TIPO_R;
        case OPCODE_BEQ:
            return EX_BRANCH;
        case OPCODE_J:
            return EX_JUMP;
        default:
            return IF; // Para instruções inválidas, volta para IF
    }
  }
  if (estado_atual == EX_MEM_IMM) {
    switch (opcode) {
        case OPCODE_LW:
            return LW_ACESSO_MEM;
        case OPCODE_SW:
            return SW_ACESSO_MEM;
        case OPCODE_ADDI:
            return END_ADDI;
        default:
            return IF; // Para instruções inválidas, volta para IF
    }
  } 
  
  if (estado_atual == SW_ACESSO_MEM || estado_atual == END_ADDI || estado_atual == END_TIPO_R) return IF;
  if (estado_atual == EX_BRANCH || estado_atual == EX_JUMP) return IF;
  if (estado_atual == LW_ACESSO_MEM) return MEM_WB;
  if (estado_atual == EX_TIPO_R) return END_TIPO_R;
    return IF;
  
}

static void gerar(SinaisDeControle *sinais_de_controle, uint8_t funct, CPU *cpu) {
    switch (cpu->estado_atual) {
        case IF:
            gerar_sinais_estado_if(sinais_de_controle);
            break;
        case ID:
            gerar_sinais_estado_id(sinais_de_controle);
            break;
        case EX_MEM_IMM:
            gerar_sinais_estado_ex_mem_imm(sinais_de_controle);
            break;
        case LW_ACESSO_MEM:
            gerar_sinais_estado_lw_acesso_mem(sinais_de_controle);
            break;
        case MEM_WB:
            gerar_sinais_estado_mem_wb(sinais_de_controle);
            break;
        case SW_ACESSO_MEM:
            gerar_sinais_estado_sw_acesso_mem(sinais_de_controle);
            break;
        case END_ADDI:
            gerar_sinais_estado_end_addi(sinais_de_controle);
            break;
        case EX_TIPO_R:
            gerar_sinais_estado_ex_tipo_r(sinais_de_controle, funct);
            break;
        case END_TIPO_R:
            gerar_sinais_estado_end_tipo_r(sinais_de_controle);
            break;
        case EX_BRANCH:
            gerar_sinais_estado_ex_branch(sinais_de_controle);
            break;
        case EX_JUMP:
            gerar_sinais_estado_ex_jump(sinais_de_controle);
            break;
        default:
            // Para estados desconhecidos, zera os sinais de controle
            *sinais_de_controle = (SinaisDeControle){0};
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

static void gerar_sinais_estado_ex_mem_imm(SinaisDeControle *sinais_de_controle) {
    *sinais_de_controle = (SinaisDeControle){
        .controle_ula = OP_ADD, // Calcula endereço base + imediato
        .escrever_memoria = 0,
        .escrever_reg = 0,
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

static void gerar_sinais_estado_lw_acesso_mem(SinaisDeControle *sinais_de_controle) {
    *sinais_de_controle = (SinaisDeControle){
        .controle_ula = 0,
        .escrever_memoria = 0,
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

static void gerar_sinais_estado_mem_wb(SinaisDeControle *sinais_de_controle) {
    *sinais_de_controle = (SinaisDeControle){
        .controle_ula = 0,
        .escrever_memoria = 0,
        .escrever_reg = 1,
        .memoria_para_reg = 1,
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

static void gerar_sinais_estado_ex_tipo_r(SinaisDeControle *sinais_de_controle, uint8_t funct) {
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

static void gerar_sinais_estado_end_tipo_r(SinaisDeControle *sinais_de_controle) {
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
        .controle_ula = 0b110,
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
        .controle_ula = 0,
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
// ANTIGAS //


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
    sinais_de_controle->ula_fonte_a = 1;
    sinais_de_controle->memoria_para_reg = 1;
    sinais_de_controle->escrever_reg = 1;
    sinais_de_controle->escrever_memoria = 0;
    sinais_de_controle->branch = 0;
    gerar_sinais_default_i(sinais_de_controle);
}

static void gerar_sinais_lw(SinaisDeControle *sinais_de_controle) {
    sinais_de_controle->controle_ula = OP_LW;
    sinais_de_controle->ula_fonte_a = 1;
    sinais_de_controle->memoria_para_reg = 0;
    sinais_de_controle->escrever_reg = 1;
    sinais_de_controle->escrever_memoria = 0;
    sinais_de_controle->branch = 0;
    gerar_sinais_default_i(sinais_de_controle);
}

static void gerar_sinais_sw(SinaisDeControle *sinais_de_controle) {
    sinais_de_controle->controle_ula = OP_SW;
    sinais_de_controle->ula_fonte_a = 1;
    sinais_de_controle->memoria_para_reg = 0; // Não importa para sw
    sinais_de_controle->escrever_reg = 0;
    sinais_de_controle->escrever_memoria = 1;
    sinais_de_controle->branch = 0;
    gerar_sinais_default_i(sinais_de_controle);
}

static void gerar_sinais_beq(SinaisDeControle *sinais_de_controle) {
    sinais_de_controle->controle_ula = OP_BEQ; // Operação de comparação (subtração)
    sinais_de_controle->ula_fonte_a = 0;
    sinais_de_controle->memoria_para_reg = 0;
    sinais_de_controle->escrever_reg = 0;
    sinais_de_controle->escrever_memoria = 0;
    sinais_de_controle->branch = 1; // Habilita o controle de desvio
    gerar_sinais_default_i(sinais_de_controle);
}

static void gerar_sinais_default_r(SinaisDeControle *sinais_de_controle) {
    sinais_de_controle->incremento_pc = 1; // Incrementa o PC normalmente
    sinais_de_controle->reg_destino = 1; // O registrador de destino é rd
    sinais_de_controle->ula_fonte_a = 0; // O segundo operando vem do registrador rt
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
    sinais_de_controle->ula_fonte_a = 0; // O segundo operando vem do registrador rt
    sinais_de_controle->memoria_para_reg = 0; // Não importa para jump
    sinais_de_controle->escrever_reg = 0;
    sinais_de_controle->escrever_memoria = 0;
    sinais_de_controle->jump = 1; // Habilita o controle de salto
    sinais_de_controle->branch = 0; // Não é uma instrução de desvio
}
