#ifndef FSM_H
#define FSM_H
#include "types.h"

// netoe1: Aqui vamos usar o esquema das variáveis globais.
// Vou esperar o bruno fazer o struct.

// netoe1: Estou usando o prefixo E_ para sinalizar que é um Enum, para nos facilitar
// Depois quando o bruno fizer o struct, vou aplicar o esquema do extern da variável global, 
// para nos poupar tempo com ponteiros

typedef enum {
    S0_IF = 0,
    S1_ID,
    S2_ADDR_CALC,
    S3_MEM_READ,
    S4_MEM_WRITEBACK,
    S5_MEM_WRITE,
    S6_ADDI_WRITEBACK,
    S7_EXEC_R,
    S8_R_WRITEBACK,
    S9_BRANCH,
    S10_JUMP
}E_EstadosFSM;


// Protótipo da funçao para implementar:
// Vou deixar meio pré-pronto para o bruno depois mudar o tipo
void gerar_sinais(SinaisDeControle *sinais_de_controle, uint8_t funct, CPU *cpu);


// Para usar o extern dps, é só criar variável global no .c, declarar extern <tipo> <nome> que foi colocado no .c, aqui no .h.
// Aí em todos os arquivos que incluírem o .h, dá pra ter acesso a ela
// Sugestão: extern FSM fsm_g;
// Recomendo ter um método para resetar ela no .c, aí chamamos no main, na primeira rodada.
// Como é variável global, podemos fazer isso pra evitar conflitos!


#endif