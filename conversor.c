#include <stdio.h>
#include "conversor.h"
#include "decodificador.h"
#include "types.h"
#include <string.h>

// netoe1-task:Implementar conversão de binario para Assembly Conversor
// Receber a Instrução decodificada do processador e converter para assembly;
// ==========================================================================
// ======CADEIAS=====================================
// Primeira cadeia: Definir Tipo da instrução;
// Segunda cadeia: Definir opcodes
// Sei que a melhor forma de implementar não é usar o strcpy/sprintf, mas para resolvermos rápido,
// irá servir

void converter_para_asm(InstrucaoDecodificada instr, char *retorno) {

    switch (instr.tipo){
        case TIPO_R:{
            switch (instr.funct){
                case FUNCT_ADD: sprintf(retorno, "add $r%u,$r%u,$r%u", instr.rd, instr.rs, instr.rt); break;
                case FUNCT_SUB: sprintf(retorno, "sub $r%u,$r%u,$r%u", instr.rd, instr.rs, instr.rt); break;
                case FUNCT_AND: sprintf(retorno, "and $r%u,$r%u,$r%u", instr.rd, instr.rs, instr.rt); break;                    
                case FUNCT_OR:  sprintf(retorno, "or $r%u,$r%u,$r%u", instr.rd, instr.rs, instr.rt);  break;
            }
            break;
        }    
        case TIPO_I:{
            switch (instr.opcode) {
                case OPCODE_ADDI:   sprintf(retorno, "addi $r%u,$r%u,%d", instr.rt, instr.rs, instr.imediato);break;
                case OPCODE_LW:     sprintf(retorno, "lw $r%u,%u($r%u)", instr.rt, instr.imediato, instr.rs); break;
                case OPCODE_SW:     sprintf(retorno, "sw $r%u,%u($r%u)", instr.rt, instr.imediato, instr.rs); break;
                case OPCODE_BEQ:    sprintf(retorno, "beq $r%u,$r%u,%u", instr.rt, instr.rs, instr.imediato); break;          
            }
            break;
        }    
        case TIPO_J:{
            if (instr.opcode == OPCODE_J) {
                sprintf(retorno, "j %u", instr.endereco);
            }
            break;
        }    
        case NOP:{
            // Zerando as strings.
            strcpy(retorno, "NOP");
            //puts("mini-mips-err: Tipo inválido para conversão");
            break;
        }    
        default:{
            // Zerando as strings.
            strcpy(retorno, "NOP");
            //puts("mini-mips-err: Erro de tipagem no decodificador.");
            break;
        }    
    }
}