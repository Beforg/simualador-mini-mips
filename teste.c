/*
 * Simulador MiniMips8Bits | Bruno Forgiarini | Ely Neto | Pablo Henrique |
 * 
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "io.h"
#include "types.h"
#include "cpu.h"

static void exibir_programa(int *opcao);
static void executar_opcao_escolhida(int opcao);

CPU cpu;

int main(int argc, char **argv) {
	int opcao = 1;	
	inicializar_cpu(&cpu);
	exibir_programa(&opcao);
	return 0;
}


static void exibir_programa(int *opcao) {
	while (*opcao != 0) {
		*opcao = exibir_menu();
		executar_opcao_escolhida(*opcao);
	}
	
}

static void executar_opcao_escolhida(int opcao) {
	if (opcao == -1) return; // resposta de erro da opcao.
	
	switch(opcao) {
		case 1:	carregar_instrucoes("memoria1.mem", cpu.memoria_de_instrucao);
				break;
		case 2:	printf("\nFuncionalidade a ser Implementada.\n");
				break;
		case 3:	printf("\nFuncionalidade a ser Implementada.\n");
				break;
		case 4:	printf("\nFuncionalidade a ser Implementada.\n");
				break;
		case 5:	printf("\nFuncionalidade a ser Implementada.\n");
				break;
		case 6:	printf("\nFuncionalidade a ser Implementada.\n");
				break;
		case 7:	printf("\nFuncionalidade a ser Implementada.\n");
				break;
		case 8:	printf("\nFuncionalidade a ser Implementada.\n");
				break;
		case 9:	avancar_cpu(&cpu);
				break;
		case 10:printf("\nFuncionalidade a ser Implementada.\n");
				break;																																					
	}
}
