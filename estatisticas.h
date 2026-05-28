#ifndef ESTATISTICAS_H
#define ESTATISTICAS_H

// Tipo R
#define ADD_CICLOS 4
#define SUB_CICLOS 4
#define OR_CICLOS 4
#define AND_CICLOS 4
// Tipo I
#define ADDI_CICLOS 4
#define LW_CICLOS 5
#define SW_CICLOS 4
#define BEQ_CICLOS 3
// Tipo J
#define J_CICLOS 3
#include <stdint.h>
#include "types.h"

void reset_estatisticas(CPU *cpu);
void mostrar_estatisticas(CPU *cpu);
void contabilizar_todas_instrucoes(CPU *cpu, uint8_t opcode, uint8_t funct);
void contabilizar_estatisticas(CPU *cpu, EstadosControle proximo_estado,EstadosControle estado_atual, InstrucaoDecodificada instrucao_decodificada);

#endif