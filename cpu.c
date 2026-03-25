#include <stdio.h>
#include "types.h"
#include "decodificador.h"
#include "conversor.h"
#include "controle.h"
#include "ula.h"

static void iniciar_cpu(CPU *cpu);
static void executrar_ciclo(CPU *cpu);
static uint16_t buscar_instrucao(const CPU *cpu);
static uint8_t mux_reg_destino(const SinaisDeControle sinais_de_controle, const InstrucaoDecodificada instrucao_decodificada);
static int8_t mux_fonte_ula(const SinaisDeControle sinais_de_controle, const InstrucaoDecodificada instrucao_decodificada, const CPU *cpu);
static void mux_memoria_para_reg();;

/* Funções de debug */
static void debug(const InstrucaoDecodificada instrucao_decodificada, const SinaisDeControle sinais_de_controle, const ResultadoUla resultadoUla, const CPU *cpu);



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



static void executrar_ciclo(CPU *cpu) {
	uint16_t instrucao;
    InstrucaoDecodificada instrucao_decodificada;
    SinaisDeControle sinais_de_controle;

    int8_t operador_a; // read register 1
    int8_t operador_b; // read register 2 ou imediato
	uint8_t registrador_destino;
    ResultadoUla resultadoUla;
    
   
	//buscar instrucao
	instrucao = buscar_instrucao(cpu);
	//
	printf("instrucao: %u\n\n", instrucao);
	//decodificar instrucao
	instrucao_decodificada = decodificar_instrucao(instrucao);
	sinais_de_controle = gerar_sinais_de_controle(instrucao_decodificada.opcode, instrucao_decodificada.funct);

	registrador_destino = mux_reg_destino(sinais_de_controle, instrucao_decodificada);

	operador_a = cpu->banco_de_regs[instrucao_decodificada.rs];
	operador_b = mux_fonte_ula(sinais_de_controle, instrucao_decodificada, cpu);

	resultadoUla = executar(operador_a, operador_b, sinais_de_controle.controle_ula);

	


	if (sinais_de_controle.escrever_reg) {
		//escrever_no_banco_de_regs(cpu, instrucao_decodificada, sinais_de_controle, resultadoUla.resultado);
		cpu->banco_de_regs[registrador_destino] = resultadoUla.resultado;
	}
	debug(instrucao_decodificada, sinais_de_controle, resultadoUla, cpu);
	cpu->pc += 1; // teste
	//operadores
	
	// extensao sinal
	
	// executar ula
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

static uint16_t buscar_instrucao(const CPU *cpu) {
	return cpu->memoria_de_instrucao[cpu->pc];
}

static void x() {}

static void debug(const InstrucaoDecodificada instrucao_decodificada, const SinaisDeControle sinais_de_controle, const ResultadoUla resultadoUla, const CPU *cpu) {
	printf("Tipo: %u\n", instrucao_decodificada.tipo);
	printf("Opcode: %u\n", instrucao_decodificada.opcode);
	printf("RS: %u\n", instrucao_decodificada.rs);
	printf("RT: %u\n", instrucao_decodificada.rt);
	printf("RD: %u\n", instrucao_decodificada.rd);
	printf("Funct: %u\n", instrucao_decodificada.funct);
	printf("Imediato: %d\n", instrucao_decodificada.imediato);
	printf("Endereco: %u\n", instrucao_decodificada.endereco);
	
	printf("------------------- Sinais de Controle Gerados: \n");
	printf("\nControle ULA: ");
	int8_para_binario(sinais_de_controle.controle_ula);
	printf("\nEscrever Memoria:");
	int8_para_binario(sinais_de_controle.escrever_memoria);
	printf("\nEscrever Reg:");
	int8_para_binario(sinais_de_controle.escrever_reg);
	printf("\nMemoria para Reg:");
	int8_para_binario(sinais_de_controle.memoria_para_reg);
	printf("\nULA Fonte:");
	int8_para_binario(sinais_de_controle.ula_fonte);
	printf("\nReg Destino:");
	int8_para_binario(sinais_de_controle.reg_destino);
	printf("\nIncremento PC:");
	int8_para_binario(sinais_de_controle.incremento_pc);
	printf("\nJump:");
	int8_para_binario(sinais_de_controle.jump);
	printf("\nBranch:");
	int8_para_binario(sinais_de_controle.branch);

	printf("------------------- Resultado da Operação na Ula\n");
	printf("Resultado: %d\n", resultadoUla.resultado);
	printf("Zero: %u\n", resultadoUla.zero);

	printf("------------------- Banco de Registradores\n");
	for (int i = 0; i < 8; i++) {
		printf("R%d: %d\n", i, cpu->banco_de_regs[i]);
	}

	printf("Instrucao atual: %u\n", cpu->memoria_de_instrucao[cpu->pc]);
}

static uint8_t mux_reg_destino(const SinaisDeControle sinais_de_controle, const InstrucaoDecodificada instrucao_decodificada) { 
	if (sinais_de_controle.reg_destino == 1) {
		return instrucao_decodificada.rd;
	}
	return instrucao_decodificada.rt;
 }
static int8_t mux_fonte_ula(const SinaisDeControle sinais_de_controle, const InstrucaoDecodificada instrucao_decodificada, const CPU *cpu) {
	if (sinais_de_controle.ula_fonte == 1) {
		return instrucao_decodificada.imediato;
	} 
	return  cpu->banco_de_regs[instrucao_decodificada.rt];
}
static void mux_memoria_para_reg() {}