#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "io.h"
#include "utils.h"
#include "conversor.h"
#include "decodificador.h"

/* Auxiliares de Leitura */
// static void verificar_erro_ao_abrir_arquivo(FILE *file);
static void ler_arquivo_de_instrucoes(FILE *file, uint16_t *memoria_de_instrucoes);
static void inserir_na_memoria_de_instrucoes(char linha[TAMANHO_LINHA], uint16_t* memoria_de_instrucoes, int *posicao, int *map_orig_to_new, int indice_original);
static void salvar_txt_logisim(FILE *file);
/* Auxiliares menu */
static void exibir_opcoes_do_menu();
static int validar_opcao_escolhida(int opcao);
static void inserir_nop(uint16_t *memoria_de_instrucoes, int *posicao);
static void ajustar_enderecos_desvios(uint16_t *memoria_de_instrucoes, const uint16_t *originais, const int *map_orig_to_new, int total_originais);

/* *
 * 
 * Carregamento das Instruções a Partir de um arquivo .mem
 * @params: Nome do arquivo e Memória de Instruções da struct CPU.
 *
 * */
void carregar_instrucoes(const char* nome_arquivo, uint16_t *memoria_de_instrucoes) {
	FILE *file;
	file = fopen(nome_arquivo, "r");

    if(file == NULL){
        puts("mini-mips-err: O arquivo não existe ou não é possível abrir!");
        return;
    }

    printf("mini-mips-info: Arquivo aberto, iniciando leitura das instruções...\n");
	ler_arquivo_de_instrucoes(file, memoria_de_instrucoes);
	
}

// static void verificar_erro_ao_abrir_arquivo(FILE *file) {
// 	if (file == NULL) {
// 		perror("Erro ao abrir o arquivo");
//         return;
// 	}
// 	printf("Arquivo aberto, iniciando leitura das instruções...\n");
// }


static void ler_arquivo_de_instrucoes(FILE *file, uint16_t *memoria_de_instrucoes) {
	char linha[TAMANHO_LINHA];
    uint16_t originais[MAX_INSTRUCOES] = {0};
    int map_orig_to_new[MAX_INSTRUCOES] = {0};
    int posicao_atual_memoria_de_instrucoes = 0; // índice da memória
    int total_originais = 0;
    
    while (fgets(linha, sizeof(linha), file) != NULL && posicao_atual_memoria_de_instrucoes < MAX_INSTRUCOES) {
        
        linha[strcspn(linha, "\n")] = '\0';
		if (strlen(linha) == 0) {
			continue;
		}
		if (total_originais >= MAX_INSTRUCOES) {
			puts("mini-mips-warn: Limite maximo de instrucoes originais atingido.");
			break;
		}
		originais[total_originais] = binario_para_int16_sem_sinal(linha);
        inserir_na_memoria_de_instrucoes(linha, memoria_de_instrucoes, &posicao_atual_memoria_de_instrucoes, map_orig_to_new, total_originais);
		total_originais++;
	}

    ajustar_enderecos_desvios(memoria_de_instrucoes, originais, map_orig_to_new, total_originais);

    salvar_txt_logisim(file);
    fclose(file);
    printf("Carga finalizada. %d instrucoes carregadas.\n", posicao_atual_memoria_de_instrucoes);
}

static void salvar_txt_logisim(FILE *file) {
    if (file == NULL) {
        puts("mini-mips-err: Arquivo de entrada inválido para gerar logisim_output.txt.");
        return;
    }

    char opcao_usuario = '\0';
    int opcao_valida = 0;

    while (!opcao_valida) {
        printf("mini-mips: Deseja gerar o arquivo de memória para o Logisim? (s/n): ");
        if (scanf(" %c", &opcao_usuario) != 1) {
            puts("mini-mips-err: Não foi possível ler a opção do usuário.");
            return;
        }

        if (opcao_usuario == 's' || opcao_usuario == 'S' || opcao_usuario == 'n' || opcao_usuario == 'N') {
            opcao_valida = 1;
        } else {
            puts("mini-mips-err: Opção inválida. Digite 's' para sim ou 'n' para não.");
        }
    }

    if (opcao_usuario == 'n' || opcao_usuario == 'N') {
        puts("mini-mips-info: Geração do arquivo do Logisim cancelada pelo usuário.");
        return;
    }

    

    FILE *file_logisim = fopen("logisim_output.txt", "w");
    char linha[TAMANHO_LINHA];
    if (file_logisim == NULL) {
        perror("Erro ao criar o arquivo de Logisim");
        return;
    }

    // Reposiciona o ponteiro no início, pois o arquivo já foi lido para carregar memória.
    rewind(file);

    fprintf(file_logisim, "v2.0 raw\n");
    while (fgets(linha, sizeof(linha), file) != NULL) {
        linha[strcspn(linha, "\n")] = '\0';
        if (strlen(linha) == 0) {
            continue;
        }
        int valor = binario_para_int16_sem_sinal(linha);
        fprintf(file_logisim, "%04X ", valor);
    }

    fclose(file_logisim);
    puts("mini-mips-info: Arquivo logisim_output.txt gerado com sucesso.");
}

static void inserir_na_memoria_de_instrucoes(char linha[TAMANHO_LINHA], uint16_t* memoria_de_instrucoes, int *posicao, int *map_orig_to_new, int indice_original) {
     if (strlen(linha) > 0) {
         uint16_t instrucao_lida = binario_para_int16_sem_sinal(linha);
            map_orig_to_new[indice_original] = *posicao;
         memoria_de_instrucoes[*posicao] = instrucao_lida;
         uint8_t opcode = (instrucao_lida >> 12) & 0xF; // Extrai os 4 bits mais significativos
         printf("Carregado na posicao [%d]: %s (0x%04X)\n", *posicao, linha, memoria_de_instrucoes[*posicao]);
         (*posicao)++;
        // if (opcode == OPCODE_J) {
         //   inserir_nop(memoria_de_instrucoes, posicao);
        // } else if (opcode == OPCODE_BEQ) {
          //  inserir_nop(memoria_de_instrucoes, posicao);
           // inserir_nop(memoria_de_instrucoes, posicao);
           // } else if (opcode == OPCODE_LW) {
           //     inserir_nop(memoria_de_instrucoes, posicao);
        // }
     }
}

static void inserir_nop(uint16_t *memoria_de_instrucoes, int *posicao) {
    if (*posicao < MAX_INSTRUCOES) {
        memoria_de_instrucoes[*posicao] = 0b0001000000000000; // NOP é representado por 0
        printf("Inserido NOP na posicao [%d]\n", *posicao);
        (*posicao)++;
    } else {
        puts("mini-mips-err: Memória de instruções cheia, não é possível inserir NOP.");
    }
}

static void ajustar_enderecos_desvios(uint16_t *memoria_de_instrucoes, const uint16_t *originais, const int *map_orig_to_new, int total_originais) {
    for (int i = 0; i < total_originais; i++) {
        uint16_t instrucao = originais[i];
        uint8_t opcode = (uint8_t)((instrucao >> 12) & 0x0F);
        int posicao_nova = map_orig_to_new[i];

        if (opcode == OPCODE_J) {
            uint8_t destino_original = (uint8_t)(instrucao & 0xFF);
            if (destino_original >= (uint8_t)total_originais) {
                continue;
            }
            uint8_t destino_novo = (uint8_t)map_orig_to_new[destino_original];
            memoria_de_instrucoes[posicao_nova] = (uint16_t)((instrucao & 0xFF00) | destino_novo);
            continue;
        }

        if (opcode == OPCODE_BEQ) {
            uint8_t destino_original = (uint8_t)(instrucao & 0x3F); // destino absoluto original
            if (destino_original >= (uint8_t)total_originais) {
                continue;
            }
            int pc_mais_um_novo = map_orig_to_new[i] + 1;
            int alvo_novo = map_orig_to_new[destino_original];
            int offset_novo = alvo_novo - pc_mais_um_novo;
            if (offset_novo < -32 || offset_novo > 31) {
                printf("mini-mips-warn: Offset de branch fora do intervalo em %d.\n", i);
                continue;
            }
            memoria_de_instrucoes[posicao_nova] = (uint16_t)((instrucao & 0xFFC0) | ((uint16_t)offset_novo & 0x3F));
        }
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

void salvar_asm(const char *filename,const uint16_t *memoria_de_instrucao){
    if (!filename || filename[0] == '\0'){
        puts("mini-mips-err->salvar_asm(): O filename não foi especificado.");
        return;
    }

    if (!memoria_de_instrucao) {
        puts("mini-mips-err->salvar_asm(): Variável memória de instrução não especificada.");
        return;
    }

    FILE *fp = fopen(filename, "w");

    if (!fp) {
        puts("mini-mips-err->salvar_asm(): Não foi possível abrir o arquivo de output.asm.");
        return;
    }

    for (int i = 0; i < 256; i++) {
        if (memoria_de_instrucao[i] == 0)
            continue;

		InstrucaoDecodificada instr = decodificar_instrucao(memoria_de_instrucao[i]);
        char linha[128] = {0};
        converter_para_asm(instr, linha);
        fprintf(fp, "%s\n", linha);
    }

    fclose(fp);
    printf("mini-mips: seu %s foi gerado!\n", filename);
}

void salvar_dat(const char *filename,const int8_t *memoria_de_dados){
	// Verifica se o arquivo recebido e válido.
	if(!filename || filename[0] == '\0'){
		puts("mini-mips-err->salvar_dat(): O filename não foi especificado.");
		return;
	}

	// Verifica se o ponteiro é válido para os dados.
	if(!memoria_de_dados){
		puts("mini-mips-err->salvar_dat(): Variável memória de dados não especificada.");
		return;
	}

	// Tenta abrir o arquivo para escrita.
	FILE *fp = fopen(filename,"w");

	// Caso o arquivo não abra, retorna zero.
	if(!fp){
		puts("mini-mips-err->salvar_dat(): Não foi possível abrir o arquivo de output.dat");
		return;
	}

	// Faz o dump da memória de dados, para o arquivo.
	// Usando o macro PRId8 para printar o int8_t sem warning do compilador
	for(int i = 0; i < 256; i++){
		fprintf(fp,"%"PRId8"\n",memoria_de_dados[i]);
	}

	// Fecha o buffer após uso.
	fclose(fp);

	printf("mini-mips: seu %s foi gerado!\n",filename);
	
	return;
}

void carregar_dat(const char *filename, int8_t *memoria_de_dados) {
    char line[16]; 
    int linha = 0;

    if (!filename || filename[0] == '\0') {
        printf("mini-mips-err: Filename não especificado.\n");
        return;
    }

    if (!memoria_de_dados) {
        printf("mini-mips-err: Ponteiro de memória inválido.\n");
        return;
    }

	if(arquivo_vazio(filename) == 1){
		puts("mini-mips-err: O arquivo está vazio!");
		return;
	}
	
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        puts("mini-mips-err: Erro ao abrir o arquivo.");
        return;
    }

    while (linha < 256 && fscanf(fp, "%15s", line) == 1) {
        char *endptr;
        long val = strtol(line, &endptr, 10);

        // Verifica se a conversão ocorreu
        if (endptr == line) {
            printf("mini-mips-err: Valor inválido na linha %d\n", linha + 1);
            linha++; 
            continue;
        }

        // Verifica o range de 8 bits
        if (val < -128 || val > 127) {
            printf("Erro: %ld fora do intervalo int8_t na linha %d.\n", val, linha + 1);
        } else {
            memoria_de_dados[linha] = (int8_t)val;
        }

        linha++;
    }

	puts("mini-mips-info: Memória de dados carregada com sucesso!");
	// Fecha o arquivo
    fclose(fp);
}

