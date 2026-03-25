#ifndef CPU_H
#define CPU_H

#include "types.h"
#include <stdint.h>

        
void resetar_cpu(CPU *cpu);
void avancar_cpu(CPU *cpu);
void executar_cpu(CPU *cpu);
void inicializar_cpu(CPU *cpu);


#endif
