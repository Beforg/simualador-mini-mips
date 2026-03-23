# ifndef IO_H
# define IO_H

# include <stdint.h>
# include "types.h"

#define MAX_INSTRUCOES 256
#define TAMANHO_LINHA 18 // 16 bits + \n + \0

void carregar_instrucoes(const char *nome_arquivo, uint16_t *memoria_de_instrucoes);
void carregar_dat(FILE *file, uint16_t *memoria_de_dados);
void salvar_dat(const uint16_t *memoria_de_dados);
void salvar_asm(const uint16_t *memoria_de_instrucoes);
int exibir_menu();


# endif
