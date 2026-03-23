#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "io.h"
#include "conversor.h"

/* Auxiliares de Leitura */
static void verificar_erro_ao_abrir_arquivo(FILE *file);
static void ler_arquivo_de_instrucoes(FILE *file, uint16_t *memoria_de_instrucoes);
static void inserir_na_memoria_de_instrucoes(char linha[TAMANHO_LINHA], uint16_t* memoria_de_instrucoes, int *posicao);

/* Auxiliares menu */
static void exibir_opcoes_do_menu();
static int validar_opcao_escolhida(int opcao);

/* *
 * 
 * Carregamento das Instruções a Partir de um arquivo .mem
 * @params: Nome do arquivo e Memória de Instruções da struct CPU.
 *
 * */
void carregar_instrucoes(const char* nome_arquivo, uint16_t *memoria_de_instrucoes) {
	FILE *file;
	file = fopen(nome_arquivo, "r");
	verificar_erro_ao_abrir_arquivo(file);
	ler_arquivo_de_instrucoes(file, memoria_de_instrucoes);
	
}

static void verificar_erro_ao_abrir_arquivo(FILE *file) {
	if (file == NULL) {
		perror("Erro ao abrir o arquivo");
        exit(1);	
	}
	printf("Arquivo aberto, iniciando leitura das instruções...\n");
}


static void ler_arquivo_de_instrucoes(FILE *file, uint16_t *memoria_de_instrucoes) {
	char linha[TAMANHO_LINHA];
    int posicao_atual_memoria_de_instrucoes = 0; // índice da memória

    while (fgets(linha, sizeof(linha), file) != NULL && posicao_atual_memoria_de_instrucoes < MAX_INSTRUCOES) {
        
        linha[strcspn(linha, "\n")] = '\0';
        inserir_na_memoria_de_instrucoes(linha, memoria_de_instrucoes, &posicao_atual_memoria_de_instrucoes);
	}

    fclose(file);
    printf("Carga finalizada. %d instrucoes carregadas.\n", posicao_atual_memoria_de_instrucoes);
}

static void inserir_na_memoria_de_instrucoes(char linha[TAMANHO_LINHA], uint16_t* memoria_de_instrucoes, int *posicao) {
     if (strlen(linha) > 0) {
         memoria_de_instrucoes[*posicao] = binario_para_int16_sem_sinal(linha);
         printf("Carregado na posicao [%d]: %s (%u)\n", *posicao, linha, memoria_de_instrucoes[*posicao]);
         (*posicao)++;
     }
}

/* *
 * 
 * Menu com as opções do simulador.
 * @return: a opção escolhida pelo usuário (para switch na main)
 *
 * */

int exibir_menu() {
	int opcao;
	
	exibir_opcoes_do_menu();
	scanf("%d", &opcao);
	return validar_opcao_escolhida(opcao);
	
}

static void exibir_opcoes_do_menu() {
	printf("1.  Carregar memória de instruções (.mem) \n");
	printf("2.  Carregar memória de Dados (.dat) \n");
	printf("3.  Imprimir memórias (instruções e dados) \n");
	printf("4.  Imprimir banco de registradores \n");
	printf("5.  Imprimir todo o simulador (registradores e memórias) \n");
	printf("6.  Salvar .asm \n");
	printf("7.  Salvar .dat \n");
	printf("8.  Executar Programa (run) \n");
	printf("9.  Executar uma instrução (Step) \n");
	printf("10. Voltar uma instrução (Back) \n");
	printf("0.  Encerrar \n\n\n");
	printf("Digite a opção: ");
}

static int validar_opcao_escolhida(int opcao) {
	if (opcao > 10 || opcao < 0) {
		printf("Opção inválida.\n");
		return -1; //erro
	}
	return opcao; // sucesso
}
