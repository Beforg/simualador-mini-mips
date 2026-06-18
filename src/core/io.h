# ifndef IO_H
# define IO_H

# include <stdint.h>
# include "core/types.h"

#define MAX_INSTRUCOES 256
#define TAMANHO_LINHA 18 // 16 bits + \n + \0

void carregar_instrucoes(const char *nome_arquivo, uint16_t *memoria_de_instrucoes);
void carregar_dat(const char *filename, int8_t *memoria_de_dados);
void salvar_dat(const char *filename,const int8_t *memoria_de_dados);
void salvar_asm(const char *filename,const uint16_t *memoria_de_instrucao);
int exibir_menu();


# endif
