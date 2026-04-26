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
	const CPU *cpu);

static int8_t mux_operador_a(CPU *cpu, SinaisDeControle sinais_de_controle, int8_t valor_registrador);
static int8_t mux_operador_b(CPU *cpu, SinaisDeControle sinais_de_controle, int8_t valor_registrador, InstrucaoDecodificada instrucao_decodificada);
static uint8_t mux_i_ou_r(SinaisDeControle sinais_de_controle, CPU *cpu);
static uint8_t mux_pc_fonte(SinaisDeControle sinais_de_controle, CPU *cpu, ResultadoUla resultadoUla, uint8_t endereco);

static void estado_if(CPU *cpu);
static void estado_id(CPU *cpu);
static void estado_ex_mem_imm(CPU *cpu);
static void estado_mem_read(CPU *cpu);
static void estado_wb_lw(CPU *cpu);
static void estado_mem_write(CPU *cpu);
static void estado_wb_addi(CPU *cpu);
static void estado_ex_r(CPU *cpu);
static void estado_wb_r(CPU *cpu);
static void estado_ex_beq(CPU *cpu);
static void estado_ex_j(CPU *cpu);
static void escrever_ri(CPU *cpu, uint16_t instrucao, SinaisDeControle sinais_de_controle);
static void escrever_rdm(CPU *cpu, uint16_t dado);

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
	uint8_t registrador_destino_indice;
	uint8_t endereco; // índice do registrador a ser escrito no banco de registradores, selecionado pelo mux RegDest
	ResultadoUla resultadoUla; // Resultado das operações da ULA, incluindo o resultado e o sinal de zero (para branch)
	int estado_executa_ula;


	//Buscar instrucao 
	instrucao_decodificada = decodificar_instrucao(cpu->ri);
	sinais_de_controle = gerar_sinais_de_controle(instrucao_decodificada.opcode, instrucao_decodificada.funct, cpu);
	endereco = mux_i_ou_r(sinais_de_controle, cpu);

	instrucao = ler_end_mem(cpu, endereco);

	escrever_ri(cpu, instrucao, sinais_de_controle);
	if (cpu->estado_atual == IF || cpu->estado_atual == LW_ACESSO_MEM) {
		escrever_rdm(cpu, instrucao);
	}


	// Decodificar instrucao 
	
	

	// PC = PC + 1;
    //incrementar_pc(cpu, sinais_de_controle); 

	// mux da arquitetura que vai selecionar o RegDest (indica o registrador a ser escrito no banco de regs)
	// ele escolhe entre o campo rd ou rt da instrução decodificada, dependendo do tipo da instrução (R ou I).
	registrador_destino_indice = mux_reg_destino(sinais_de_controle, instrucao_decodificada);
	// valores que serão operados na ula: _a RS, _b RT ou imediato dependendo do tipo da instrução (R ou I)
	cpu->a = ler_registrador(cpu, instrucao_decodificada.rs);
	cpu->b = ler_registrador(cpu, instrucao_decodificada.rt);
	
	operador_a = mux_operador_a(cpu, sinais_de_controle, cpu->a);
	operador_b = mux_operador_b(cpu, sinais_de_controle, cpu->b, instrucao_decodificada);
	
	// Execucao 
	estado_executa_ula = (cpu->estado_atual == IF ||
		cpu->estado_atual == ID ||
		cpu->estado_atual == EX_MEM_IMM ||
		cpu->estado_atual == EX_TIPO_R ||
		cpu->estado_atual == EX_BRANCH);

	if (estado_executa_ula) {
		resultadoUla = executar(operador_a, operador_b, sinais_de_controle.controle_ula);
		cpu->saida_ula = resultadoUla.resultado;
	} else {
		resultadoUla.resultado = cpu->saida_ula;
		resultadoUla.zero = (cpu->saida_ula == 0);
		resultadoUla.overflow = 0;
	}

	cpu->pc = mux_pc_fonte(sinais_de_controle, cpu, resultadoUla, instrucao_decodificada.endereco);
	// acesso a memoria 
	escrever_end_mem(cpu, endereco, cpu->b, sinais_de_controle);
	valor_write_back = mux_memoria_para_reg(sinais_de_controle, instrucao_decodificada, cpu);

	// write back 
	escrever_registrador(cpu, registrador_destino_indice, valor_write_back, sinais_de_controle);

	//resolver_desvio(cpu, instrucao_decodificada.imediato, instrucao_decodificada.endereco, sinais_de_controle, resultadoUla);
	debug_geral(instrucao_decodificada,instrucao, sinais_de_controle, resultadoUla, cpu, opcao_debug);
	cpu->estado_atual = proximo_estado(cpu->estado_atual, instrucao_decodificada.opcode);
	
}

static void executar_programa_completo(CPU *cpu) {
	while (cpu->memoria[cpu->pc] != 0) {
		executrar_ciclo(cpu, 0);
	}
}

static void iniciar_cpu(CPU *cpu)
{
	int posicao;
	cpu->pc = 0;
	cpu->estado_atual = IF;
	cpu->ri = 0;
	cpu->rdm = 0;
	cpu->a = 0;
	cpu->b = 0;
	cpu->saida_ula = 0;

	for (posicao = 0; posicao < 256; posicao++)
	{
		cpu->memoria[posicao] = 0;
	}

	for (posicao = 0; posicao < 256; posicao++)
	{
		cpu->memoria[posicao] = 0;
	}

	for (posicao = 0; posicao < 8; posicao++)
	{
		cpu->banco_de_regs[posicao] = 0;
	}
}

static void escrever_ri(CPU *cpu, uint16_t instrucao, SinaisDeControle sinais_de_controle) {
	if (sinais_de_controle.ir_escrever) {
		cpu->ri = instrucao;
	}
}

static void escrever_rdm(CPU *cpu, uint16_t dado) {
	cpu->rdm = dado;
}

static uint8_t mux_pc_fonte(SinaisDeControle sinais_de_controle, CPU *cpu, ResultadoUla resultadoUla, uint8_t endereco) {


	if (sinais_de_controle.pc_fonte == 2) {
		return endereco; // Endereço de jump
	} else if (sinais_de_controle.pc_fonte == 1 && (sinais_de_controle.branch == 1 && resultadoUla.zero == 1)) {
		return cpu->saida_ula; // Endereço de branch
	} else {
		if (sinais_de_controle.incremento_pc == 0) {
		return cpu->pc; // Mantém o PC atual (não incrementa)
	}
		return resultadoUla.resultado; // Próxima instrução sequencial
	}
}

static int8_t mux_operador_b(CPU *cpu, SinaisDeControle sinais_de_controle, int8_t valor_registrador, InstrucaoDecodificada instrucao_decodificada) {
	if (sinais_de_controle.ula_fonte_b == 1) {
		return 1; // Vem do campo imediato da instrução
	} else if (sinais_de_controle.ula_fonte_b == 2) {
		return instrucao_decodificada.imediato; // Vem do imediato da instrucao
	} else {
		return valor_registrador; // Valor fixo 0 para operações que não usam o registrador rt ou imediato
	}
}

static int8_t mux_operador_a(CPU *cpu, SinaisDeControle sinais_de_controle, int8_t valor_registrador) {
	if (sinais_de_controle.ula_fonte_a == 1) {
		return valor_registrador; // Vem do registrador rs
	} else {
		return cpu->pc; // Valor fixo 0 para operações que não usam o registrador rs
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
	if (sinais_de_controle.ula_fonte_a == 1)
	{
		return instrucao_decodificada.imediato;
	}
	return ler_registrador(cpu, instrucao_decodificada.rt);
}
static int8_t mux_memoria_para_reg(
	const SinaisDeControle sinais_de_controle,
	const InstrucaoDecodificada instrucao_decodificada,
	const CPU *cpu)
{
	if (sinais_de_controle.memoria_para_reg == 1)
	{
		return (int8_t)cpu->rdm;
	}
	return cpu->saida_ula;
}

static uint8_t mux_i_ou_r(SinaisDeControle sinais_de_controle, CPU *cpu) {
	if(sinais_de_controle.i_ou_d == 1) {
		return cpu->saida_ula; // Funct para instruções R
	} else {
		return cpu->pc; // Opcode para instruções I
	}
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


static void estado_if(CPU *cpu) {

}
static void estado_id(CPU *cpu) {

}
static void estado_ex_mem_imm(CPU *cpu) {

}
static void estado_mem_read(CPU *cpu) {

}
static void estado_wb_lw(CPU *cpu) {

}
static void estado_mem_write(CPU *cpu) {

}
static void estado_wb_addi(CPU *cpu) {

}
static void estado_ex_r(CPU *cpu) {

}
static void estado_wb_r(CPU *cpu) {

}
static void estado_ex_beq(CPU *cpu) {

}
static void estado_ex_j(CPU *cpu) {
	
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
