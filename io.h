# ifndef IO_H
# define IO_H

# include <stdint.h>
# include "types.h"

#define MAX_INSTRUCOES 256
#define TAMANHO_LINHA 18 // 16 bits + \n + \0
#define MODO_INSTRUCAO 0
#define MODO_DADOS 1

void carregar_instrucoes_e_dados(const char *nome_arquivo, CPU *cpu);
void carregar_dat(const char *filename, int8_t *memoria_de_dados);
void salvar_dat(const char *filename,const int8_t *memoria_de_dados);
void salvar_asm(const char *filename,const uint16_t *memoria_de_instrucao);
int exibir_menu();


# endif
