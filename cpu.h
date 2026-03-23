#ifndef CPU_H
#define CPU_H

#include "types.h"
#include <stdint.h>

void incrementar_pc(CPU *cpu, ResultadoUla resultado, uint8_t imediato);
void escrever_no_banco_de_regs(CPU *cpu, InstrucaoDecodificada instrucao,SinaisDeControle sinais, int8_t valor);
        
void resetar_cpu(CPU *cpu);
void avancar_cpu(CPU *cpu);
void executar_cpu(CPU *cpu);
void inicializar_cpu(CPU *cpu);


#endif
