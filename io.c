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
static void processar_dados_e_instrucoes(FILE *file, CPU *cpu);
static void inserir_na_memoria_de_instrucoes(char linha[TAMANHO_LINHA], uint16_t* memoria_de_instrucoes, int *posicao);
static void salvar_txt_logisim(FILE *file);
/* Auxiliares menu */
static void exibir_opcoes_do_menu();
static int validar_opcao_escolhida(int opcao);

/* *
 * 
 * Carregamento das Instruções a Partir de um arquivo .mem
 * @params: Nome do arquivo e Memória de Instruções da struct CPU.
 *
 * */
void carregar_instrucoes_e_dados_e_instrucoes(const char *nome_arquivo, CPU *cpu) {
	FILE *file;
	file = fopen(nome_arquivo, "r");

    if(file == NULL){
        puts("mini-mips-err: O arquivo não existe ou não é possível abrir!");
        return;
    }

    printf("mini-mips-info: Arquivo aberto, iniciando leitura das instruções...\n");
	processar_dados_e_instrucoes(file, cpu);
	
}

// static void verificar_erro_ao_abrir_arquivo(FILE *file) {
// 	if (file == NULL) {
// 		perror("Erro ao abrir o arquivo");
//         return;
// 	}
// 	printf("Arquivo aberto, iniciando leitura das instruções...\n");
// }


static void processar_dados_e_instrucoes(FILE *file, CPU *cpu) {
	char linha[64];
    int modo = MODO_INSTRUCAO;
    int endereco_atual = 0;
    int addr_dados_sequencial = 128; 

    while (fgets(linha, sizeof(linha), file) != NULL) {
        linha[strcspn(linha, "\r\n")] = '\0';

        if (strlen(linha) == 0 || linha[0] == '#') {
            continue;
        }

        if (strcmp(linha, ".data") == 0) {
            modo = MODO_DADOS;
            printf("  -> Seção .data encontrada. Carregando dados a partir de 128.\n");
            continue;
        }

        if (modo == MODO_INSTRUCAO) {
            if (endereco_atual <= 127) {
                cpu->memoria[endereco_atual] = binario_para_int16_sem_sinal(linha);
                endereco_atual++;
            } else {
                puts("mini-mips-warn: Limite de instruções (127) atingido. Verifique o ficheiro .mem");
            }
        } 
        else if (modo == MODO_DADOS) {
            int addr_lido;
            char bin_lido[20];

            // Tenta formato "endereco:binario"
            if (sscanf(linha, "%d:%s", &addr_lido, bin_lido) == 2) {
                if (addr_lido >= 128 && addr_lido <= 255) {
                    cpu->memoria[addr_lido] = (int8_t)binario_para_int16_sem_sinal(bin_lido);
                    printf("    [Data] End: %d carregado.\n", addr_lido);
                } else {
                    printf("mini-mips-err: Endereço de dado %d fora da faixa permitida [128-255].\n", addr_lido);
                }
            } 
            else {
                // Formato sequencial (apenas o binário)
                if (addr_dados_sequencial <= 255) {
                    cpu->memoria[addr_dados_sequencial] = (int8_t)binario_para_int16_sem_sinal(linha);
                    addr_dados_sequencial++;
                } else {
                    puts("mini-mips-err: Memória de dados cheia (255).");
                }
            }
        }
    }
    salvar_txt_logisim(file);
    fclose(file);
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

static void inserir_na_memoria_de_instrucoes(char linha[TAMANHO_LINHA], uint16_t* memoria_de_instrucoes, int *posicao) {
     if (strlen(linha) > 0) {
         memoria_de_instrucoes[*posicao] = binario_para_int16_sem_sinal(linha);
         printf("Carregado na posicao [%d]: %s (0x%04X)\n", *posicao, linha, memoria_de_instrucoes[*posicao]);
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

