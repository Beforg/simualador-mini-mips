#ifndef CPU_H
#define CPU_H

#include "core/types.h"
#include <stdint.h>
#include <stdbool.h>
    
void voltar_cpu(CPU *cpu, PilhaCPU *pilha);
void avancar_cpu(CPU *cpu, PilhaCPU *pilha, int opcao_debug);
void executar_cpu(CPU *cpu);
void inicializar_cpu(CPU *cpu);


#endif
