#include <stdio.h>
#include "utils/utils.h"


void print_int_4bits(int int_generico){
    // Usando uma feature nova do C23 para números binários.
    // Existe o operador no printf() %b, que printa um número em binário
    // O gcc já possui suporte, porém compiladores mais antigos podem não ter.
    printf("%.4b",int_generico);
}

void print_int_3bits(int int_generico){
    printf("%.3b",int_generico);
}

int8_t extender_sinal(uint16_t instrucao) {
    uint8_t imm6 = (uint8_t)(instrucao & 0x3F); // extrai 6 bits

    // verifica se o 6º (5) bit (bit de sinal, máscara 0x20 que é 0010 0000) é 1
    if (imm6 & 0x20) {
        // Se for 1, precisamos preencher os bits 6 e 7 com '1'
        // 0xC0 em binário é 1100 0000. 
        // Usamos o operador OR para "forçar" esses bits para 1.
        return (int8_t)(imm6 | 0xC0); 
    }
    return (int8_t)imm6;
}

void int16_para_binario(int16_t valor) {
    char binario[17];
    for (int posicao_bit = 15; posicao_bit >= 0; posicao_bit--) {
        binario[15 - posicao_bit] = (valor & (1 << posicao_bit)) ? '1' : '0';
    }
    binario[16] = '\0';
    printf("%s", binario);
}

void int8_para_binario(int8_t valor) {
	char binario[9];
	for (int posicao_bit = 7; posicao_bit >= 0; posicao_bit--) {
		binario[7 - posicao_bit] = (valor & (1 << posicao_bit)) ? '1' : '0';
	}
	
	binario[8] = '\0';
	printf("%s", binario);
}

void int8_hexa(int8_t valor) {
    printf("0x%02X", (uint8_t)valor);
}

void int16_hexa(int16_t valor) {
    printf("0x%04X", (uint16_t)valor);
}

int8_t binario_para_int8(const char *binario) {
    return (int8_t)strtoul(binario, NULL, 2);
}

uint8_t binario_para_int8_sem_sinal(const char *binario) {
    return (uint8_t)strtoul(binario, NULL, 2);
}

int16_t binario_para_int16(const char *binario) {
    return (int16_t)strtoul(binario, NULL, 2);
}

uint16_t binario_para_int16_sem_sinal(const char *binario) {
    return (uint16_t)strtoul(binario, NULL, 2);
}

char *strtrim(char *str) { // Função pega com o gemini.
    char *end;

    // Remove espaços do início
    while (isspace((unsigned char)*str)) str++;

    // Se a string for apenas espaços, retorna o ponteiro atual (vazio)
    if (*str == 0) return str;

    // Remove espaços do final
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    *(end + 1) = '\0'; // Termina a string

    return str;
}


int arquivo_vazio(const char *filename) {
    FILE *fp = fopen(filename, "r");
    
    if (fp == NULL) return -1; // Erro ao abrir (arquivo não existe)

    fseek(fp, 0, SEEK_END);    // Move o cursor para o final
    long tamanho = ftell(fp);  // Pega a posição atual (em bytes)
    fclose(fp);

    return (tamanho == 0);     // Retorna 1 se estiver vazio, 0 se tiver conteúdo
}

// Salva o estado atual na pilha (antes de executar a instrução)
void push_cpu(PilhaCPU *p, const CPU *cpu) {
    No *novo = (No*) malloc(sizeof(No));
    if (novo == NULL) return; // Erro de memória

    novo->estado = *cpu; // Cópia completa da struct CPU
    novo->proximo = p->topo;
    p->topo = novo;
}

// Retorna para o estado anterior
int pop_cpu(PilhaCPU *p, CPU *cpu) {
    if (p->topo == NULL) return 0; // Pilha vazia, pra evitar underflow

    No *aux = p->topo;
    *cpu = aux->estado; // Restaura o estado salvo
    p->topo = aux->proximo;
    
    free(aux);
    return 1;
}

void inicializar_pilha(PilhaCPU *p) {
    p->topo = NULL;
}