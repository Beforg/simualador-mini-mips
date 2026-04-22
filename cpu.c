#include <stdio.h>
#include "types.h"
#include "decodificador.h"
#include "conversor.h"
#include "controle.h"
#include "ula.h"
#include "memoria.h"
#include "utils.h"
#include "debug.h"

/*Funções auxiliares*/
static void iniciar_cpu(CPU *cpu);
static void executrar_ciclo(CPU *cpu, int opcao_debug);
static void executar_programa_completo(CPU *cpu);
static void incrementar_pc(CPU *cpu, SinaisDeControle sinais_de_controle);
static void resolver_desvio(CPU *cpu, int8_t imediato, uint8_t endereco, SinaisDeControle sinais_de_controle, ResultadoUla resultadoUla);
//static uint16_t buscar_instrucao(const CPU *cpu);
static uint8_t mux_reg_destino(const SinaisDeControle sinais_de_controle, const InstrucaoDecodificada instrucao_decodificada);
static int8_t mux_fonte_ula(const SinaisDeControle sinais_de_controle, const InstrucaoDecodificada instrucao_decodificada, const CPU *cpu);
static int8_t mux_memoria_para_reg(
	const SinaisDeControle sinais_de_controle, 
	const InstrucaoDecodificada instrucao_decodificada,  
	const CPU *cpu, const ResultadoUla resultadoUla);


/* Funções de debug */

/* Ver implementação e qual o comportamento do reset. */
void voltar_cpu(CPU *cpu, PilhaCPU *pilha) {
	if (pop_cpu(pilha, cpu)) {
        printf("mini-mips-info: Retornou para a instrução anterior.\n");
    } else {
        printf("mini-mips-warn: Não há estados anteriores para voltar!\n");
    }
}

/* Executa um ciclo no simulador. */
void avancar_cpu(CPU *cpu, PilhaCPU *pilha, int opcao_debug)
{
	push_cpu(pilha, cpu);
	executrar_ciclo(cpu, opcao_debug);
}

/* Executa todo o programa */
void executar_cpu(CPU *cpu) {
	 // Salva o estado inicial da CPU na pilha
	executar_programa_completo(cpu);
}

/* Inicializar colocando zerando os regs e memorias colocando em um estado inicial */
void inicializar_cpu(CPU *cpu)
{
	iniciar_cpu(cpu);
}


/* executa um ciclo no simulador
* recebe a CPU que vai ser declarada no main e executa um ciclo completo, 
* ou seja, busca a instrução, decodifica, gera os sinais de controle, e
* executa a operação na ULA, acessa a memória 
* e escreve de volta no banco de registradores.
**/
static void executrar_ciclo(CPU *cpu, int opcao_debug)
{
	uint16_t instrucao;
	InstrucaoDecodificada instrucao_decodificada;
	SinaisDeControle sinais_de_controle;

	int8_t operador_a;		 // Read register 1
	int8_t operador_b;		 // Read register 2 ou imediato
	int8_t valor_write_back; // valor a ser escrito no banco de registradores (DMem ou ResultadoUla)
	uint8_t registrador_destino_indice; // índice do registrador a ser escrito no banco de registradores, selecionado pelo mux RegDest
	ResultadoUla resultadoUla; // Resultado das operações da ULA, incluindo o resultado e o sinal de zero (para branch)

	// Buscar instrucao 
	instrucao = ler_end_mem_instrucao(cpu, cpu->pc);
	
	// Decodificar instrucao 
	instrucao_decodificada = decodificar_instrucao(instrucao);
	sinais_de_controle = gerar_sinais_de_controle(instrucao_decodificada.opcode, instrucao_decodificada.funct);

	// PC = PC + 1;
    incrementar_pc(cpu, sinais_de_controle); 

	// mux da arquitetura que vai selecionar o RegDest (indica o registrador a ser escrito no banco de regs)
	// ele escolhe entre o campo rd ou rt da instrução decodificada, dependendo do tipo da instrução (R ou I).
	registrador_destino_indice = mux_reg_destino(sinais_de_controle, instrucao_decodificada);

	// valores que serão operados na ula: _a RS, _b RT ou imediato dependendo do tipo da instrução (R ou I)
	operador_a = ler_registrador(cpu, instrucao_decodificada.rs);
	operador_b = mux_fonte_ula(sinais_de_controle, instrucao_decodificada, cpu);
	
	// Execucao 
	resultadoUla = executar(operador_a, operador_b, sinais_de_controle.controle_ula);

	// acesso a memoria 
	escrever_end_mem_dados(cpu, resultadoUla.resultado, cpu->banco_de_regs[instrucao_decodificada.rt], sinais_de_controle);
	valor_write_back = mux_memoria_para_reg(sinais_de_controle, instrucao_decodificada, cpu, resultadoUla);

	// write back 
	escrever_registrador(cpu, registrador_destino_indice, valor_write_back, sinais_de_controle);

	
	resolver_desvio(cpu, instrucao_decodificada.imediato, instrucao_decodificada.endereco, sinais_de_controle, resultadoUla);
	
	debug_geral(instrucao_decodificada,instrucao, sinais_de_controle, resultadoUla, cpu, opcao_debug);
	
}

static void executar_programa_completo(CPU *cpu) {
	while (cpu->memoria_de_instrucao[cpu->pc] != 0) {
		executrar_ciclo(cpu, 0);
	}
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
		return ler_end_mem_dados(cpu, (uint8_t)(resultadoUla.resultado));
	}
	return resultadoUla.resultado;
}

static void incrementar_pc(CPU *cpu, SinaisDeControle sinais_de_controle) {

	if (cpu->pc >= 256) {
		return; // Evita incrementar o PC além do limite da memória de instruções
	}

	if (sinais_de_controle.incremento_pc == 1) {
		//printf("Incrementando PC. Valor atual: %u\n", cpu->pc);
		cpu->pc += 1; // Incrementa o PC para a próxima instrução
	}
}

static void resolver_desvio(CPU *cpu, int8_t imediato, uint8_t endereco, SinaisDeControle sinais_de_controle, ResultadoUla resultadoUla) {
	if (sinais_de_controle.jump) {
        cpu->pc = endereco;
        return;
    }

	if (sinais_de_controle.branch) {
        if (resultadoUla.zero) {
            cpu->pc = cpu->pc + imediato; //zero = 1 
        } 
        return;
    }
}
