#include <stdio.h>
#include "types.h"
#include "decodificador.h"
#include "conversor.h"
#include "controle.h"
#include "ula.h"
#include "banco_de_regs.h"
#include "memoria.h"

static void iniciar_cpu(CPU *cpu);
static void executrar_ciclo(CPU *cpu);
static uint16_t buscar_instrucao(const CPU *cpu);
static uint8_t mux_reg_destino(const SinaisDeControle sinais_de_controle, const InstrucaoDecodificada instrucao_decodificada);
static int8_t mux_fonte_ula(const SinaisDeControle sinais_de_controle, const InstrucaoDecodificada instrucao_decodificada, const CPU *cpu);
static int8_t mux_memoria_para_reg(
	const SinaisDeControle sinais_de_controle, 
	const InstrucaoDecodificada instrucao_decodificada,  
	const CPU *cpu, const ResultadoUla resultadoUla);

/* Funções de debug */
static void debug(const InstrucaoDecodificada instrucao_decodificada, const SinaisDeControle sinais_de_controle, const ResultadoUla resultadoUla, const CPU *cpu);

static void incrementar_pc(CPU *cpu, int8_t imediato, uint8_t endereco, SinaisDeControle sinais_de_controle, ResultadoUla resultadoUla);

void resetar_cpu(CPU *cpu) {}

void avancar_cpu(CPU *cpu)
{
	executrar_ciclo(cpu);
}

void executar_cpu(CPU *cpu) {}

void inicializar_cpu(CPU *cpu)
{
	iniciar_cpu(cpu);
}

static void executrar_ciclo(CPU *cpu)
{
	uint16_t instrucao;
	InstrucaoDecodificada instrucao_decodificada;
	SinaisDeControle sinais_de_controle;

	int8_t operador_a;		 // read register 1
	int8_t operador_b;		 // read register 2 ou imediato
	int8_t valor_write_back; // valor a ser escrito no banco de registradores
	int8_t valor_lido_memoria; // valor lido da memória (para lw)
	uint8_t registrador_destino;
	ResultadoUla resultadoUla;

	// buscar instrucao
	printf("PC valor: %u\n", cpu->pc);
	instrucao = ler_memoria_instrucao(cpu, cpu->pc);
	//
	printf("instrucao: %u\n\n", instrucao);
	// decodificar instrucao
	instrucao_decodificada = decodificar_instrucao(instrucao);
	sinais_de_controle = gerar_sinais_de_controle(instrucao_decodificada.opcode, instrucao_decodificada.funct);

	registrador_destino = mux_reg_destino(sinais_de_controle, instrucao_decodificada);

	operador_a = ler_registrador(cpu, instrucao_decodificada.rs);
	operador_b = mux_fonte_ula(sinais_de_controle, instrucao_decodificada, cpu);

	// execucao
	resultadoUla = executar(operador_a, operador_b, sinais_de_controle.controle_ula);

	// memoria
	valor_lido_memoria = ler_memoria_dados(cpu, (uint8_t)(resultadoUla.resultado));
	escrever_memoria_dados(cpu, resultadoUla.resultado, cpu->banco_de_regs[instrucao_decodificada.rt], sinais_de_controle);

	valor_write_back = mux_memoria_para_reg(sinais_de_controle, instrucao_decodificada, cpu, resultadoUla);

	// write back
	escrever_registrador(cpu, registrador_destino, valor_write_back, sinais_de_controle);

	debug(instrucao_decodificada, sinais_de_controle, resultadoUla, cpu);
	incrementar_pc(cpu, instrucao_decodificada.imediato, instrucao_decodificada.endereco, sinais_de_controle, resultadoUla); // teste
	printf("PC atualizado: %u\n", cpu->pc);
}

static void iniciar_cpu(CPU *cpu)
{
	int posicao;
	cpu->pc = 0;

	for (posicao = 0; posicao < 256; posicao++)
	{
		cpu->memoria_de_instrucao[posicao] = 0;
	}

	for (posicao = 0; posicao < 256; posicao++)
	{
		cpu->memoria_de_dados[posicao] = 0;
	}

	for (posicao = 0; posicao < 8; posicao++)
	{
		cpu->banco_de_regs[posicao] = 0;
	}
}

static void x() {}

static void debug(const InstrucaoDecodificada instrucao_decodificada, const SinaisDeControle sinais_de_controle, const ResultadoUla resultadoUla, const CPU *cpu)
{
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
	imprimir_registradores(cpu);
	if (cpu->pc == 10 ) {


	printf("------------------- Memória de Instruções\n");
	imprimir_memoria_instrucao(cpu);
	printf("------------------- Memória de Dados\n");
	imprimir_memoria_dados(cpu);

	}

	printf("PC: %u\n", cpu->pc);

	printf("Instrucao atual: %u\n", cpu->memoria_de_instrucao[cpu->pc]);
}

static uint8_t mux_reg_destino(const SinaisDeControle sinais_de_controle, const InstrucaoDecodificada instrucao_decodificada)
{
	if (sinais_de_controle.reg_destino == 1)
	{
		return instrucao_decodificada.rd;
	}
	return instrucao_decodificada.rt;
}
static int8_t mux_fonte_ula(const SinaisDeControle sinais_de_controle, const InstrucaoDecodificada instrucao_decodificada, const CPU *cpu)
{
	if (sinais_de_controle.ula_fonte == 1)
	{
		return instrucao_decodificada.imediato;
	}
	return ler_registrador(cpu, instrucao_decodificada.rt);
}
static int8_t mux_memoria_para_reg(
	const SinaisDeControle sinais_de_controle,
	const InstrucaoDecodificada instrucao_decodificada,
	const CPU *cpu, ResultadoUla resultadoUla)
{
	if (sinais_de_controle.memoria_para_reg == 0)
	{
		return ler_memoria_dados(cpu, resultadoUla.resultado);
	}
	return resultadoUla.resultado;
}

void incrementar_pc(CPU *cpu, int8_t imediato, uint8_t endereco, SinaisDeControle sinais_de_controle, ResultadoUla resultadoUla) {
	if (sinais_de_controle.incremento_pc == 1) {
		printf("Incrementando PC. Valor atual: %u\n", cpu->pc);
		printf("Imediato: %d, Endereco: %u\n", imediato, endereco);

		cpu->pc += 1; // Incrementa o PC normalmente
		if (sinais_de_controle.branch == 1 && resultadoUla.zero == 1) {
			printf("Executando branch. PC atual: %u\n", cpu->pc);
			cpu->pc += imediato; // Desvio para frente ou para trás dependendo do valor do imediato

		} else if (sinais_de_controle.jump == 1) {
			printf("Executando jump. PC atual: %u\n", cpu->pc);
			cpu->pc = endereco; // Salto para o endereço especificado pelo imediato
		} 
	}
}