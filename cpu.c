#include <stdio.h>
#include "types.h"
#include "decodificador.h"

static void iniciar_cpu(CPU *cpu);
static void executrar_ciclo(CPU *cpu);
static uint16_t buscar_instrucao(const CPU *cpu);

/* Funções de debug */
static void debug(InstrucaoDecodificada instrucao_decodificada);



void incrementar_pc(CPU *cpu, ResultadoUla resultado, uint8_t imediato) {}

void escrever_no_banco_de_regs(CPU *cpu, InstrucaoDecodificada instrucao,SinaisDeControle sinais, int8_t valor) {}
        
void resetar_cpu(CPU *cpu) {}

void avancar_cpu(CPU *cpu) {
	executrar_ciclo(cpu);
}

void executar_cpu(CPU *cpu) {}

void inicializar_cpu(CPU *cpu) {
	iniciar_cpu(cpu);
}

static void iniciar_cpu(CPU *cpu) {
	int posicao;
	cpu->pc = 0;
	
	for (posicao = 0; posicao < 256; posicao++) {
		cpu->memoria_de_instrucao[posicao] = 0;
	}
	
	for (posicao = 0; posicao < 256; posicao++) {
		cpu->memoria_de_dados[posicao] = 0;
	}
	
	for (posicao = 0; posicao < 8; posicao++) {
		cpu->banco_de_regs[posicao] = 0;
	}
}

static void executrar_ciclo(CPU *cpu) {
	uint16_t instrucao;
    InstrucaoDecodificada instrucao_decodificada;
   // SinaisDeControle sinais_de_controle;
   // uint16_t operador_a;
   // uint16_t operador_b;
   // int8_t imediato_extendido;
   // ResultadoUla resultado;
    
   
	//buscar instrucao
	instrucao = buscar_instrucao(cpu);
	//
	printf("instrucao: %u\n\n", instrucao);
	//decodificar instrucao
	instrucao_decodificada = decodificar_instrucao(instrucao);
	debug(instrucao_decodificada);
	cpu->pc += 1; // teste
	//operadores
	
	// extensao sinal
	
	// executar ula
}

static  uint16_t buscar_instrucao(const CPU *cpu) {
	return cpu->memoria_de_instrucao[cpu->pc];
}

static void debug(InstrucaoDecodificada instrucao_decodificada) {
	printf("Tipo: %u\n", instrucao_decodificada.tipo);
	printf("Opcode: %u\n", instrucao_decodificada.opcode);
	printf("RS: %u\n", instrucao_decodificada.rs);
	printf("RT: %u\n", instrucao_decodificada.rt);
	printf("RD: %u\n", instrucao_decodificada.rd);
	printf("Funct: %u\n", instrucao_decodificada.funct);
	printf("Imediato: %d\n", instrucao_decodificada.imediato);
	printf("Endereco: %u\n", instrucao_decodificada.endereco);
}

