# include "controle.h"
# include "types.h"
# include "fsm.h"

SinaisDeControle gerar_sinais_de_controle(uint8_t opcode, uint8_t funct, CPU *cpu) {
    SinaisDeControle sinais_de_controle = {0}; // Zera tudo
    gerar_sinais(&sinais_de_controle, funct, cpu);
    return sinais_de_controle;
}

EstadosControle proximo_estado(EstadosControle estado_atual, uint8_t opcode) {

    switch (estado_atual){
        case IF:
            return ID;

        case ID:
            switch (opcode){
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
                    return IF; // Para instruções inválidas, voltando pra IF
            }

        case EX_MEM_IMM:
            switch (opcode){
                case OPCODE_LW:
                    return LW_ACESSO_MEM;
                case OPCODE_SW:
                    return SW_ACESSO_MEM;
                case OPCODE_ADDI:
                    return END_ADDI;
                default:
                    return IF; // Para instruções inválidas, voltando pra IF
            }

        case LW_ACESSO_MEM:
            return MEM_WB;

        case MEM_WB:
            return IF;

        case SW_ACESSO_MEM:
            return IF;

        case END_ADDI:
            return IF;

        case EX_TIPO_R:
            return END_TIPO_R; 

        case END_TIPO_R:
            return IF;

        case EX_BRANCH:
            return IF;

        case EX_JUMP:
            return IF;

        default:
            return IF;
    }
}
