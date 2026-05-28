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
//static uint8_t mux_reg_destino(const SinaisDeControle sinais_de_controle, const InstrucaoDecodificada instrucao_decodificada);
static int8_t mux_fonte_ula(const SinaisDeControle sinais_de_controle, const InstrucaoDecodificada instrucao_decodificada, const CPU *cpu);
static int8_t mux_memoria_para_reg(
	const SinaisDeControle sinais_de_controle, 
	const InstrucaoDecodificada instrucao_decodificada,  
	const CPU *cpu, const ResultadoUla resultadoUla);
/* Funções pipeline */
static uint8_t mux_reg_destino(CPU* cpu);
static uint8_t atualizar_pc(
    CPU *cpu, ResultadoUla resultadoUla, 
    InstrucaoDecodificada instrucao_decodificada,
    SinaisDeControle sinais_de_controle);
static int8_t mux_write_back(CPU* cpu);
static uint8_t mux_pc_mais_um_ou_branch_ou_jump(
        CPU *cpu, SinaisDeControle sinais_de_controle, 
        InstrucaoDecodificada instrucao_decodificada, 
        uint8_t pc_mais_um_ou_branch);
static uint8_t mux_pc_mais_um_ou_branch(CPU *cpu, ResultadoUla resultadoUla);
static int desvio_condicional_tomado(CPU *cpu, ResultadoUla resultadoUla);
static uint8_t somador_pc_branch(CPU* cpu);
static uint8_t somador_pc_mais_um(CPU* cpu);

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

	int8_t valor_reg_a;		 // Read register 1
	int8_t valor_reg_b;		 // Read register 2 ou imediato
	int8_t valor_write_back; // valor a ser escrito no banco de registradores (DMem ou ResultadoUla)
	uint8_t registrador_destino_indice; // índice do registrador a ser escrito no banco de registradores, selecionado pelo mux RegDest
	ResultadoUla resultadoUla; // Resultado das operações da ULA, incluindo o resultado e o sinal de zero (para branch)
	int8_t valor_memoria; // Valor lido da memória de dados (para instruções de load)	
	

	

	
	// Buscar instrucao 
	//instrucao = ler_end_mem_instrucao(cpu, cpu->bi_di.ri);
	// BI / DI
	instrucao_decodificada = decodificar_instrucao(cpu->bi_di.ri);
	sinais_de_controle = gerar_sinais_de_controle(instrucao_decodificada.opcode, instrucao_decodificada.funct);
	valor_reg_a = ler_registrador(cpu, instrucao_decodificada.rs);
	valor_reg_b = ler_registrador(cpu, instrucao_decodificada.rt);

	resultadoUla = executar(cpu->di_ex.a, cpu->di_ex.b, cpu->di_ex.ex_sinais.controle_ula);


	// PC = PC + 1;
   // incrementar_pc(cpu, sinais_de_controle); 

	// mux da arquitetura que vai selecionar o RegDest (indica o registrador a ser escrito no banco de regs)
	// ele escolhe entre o campo rd ou rt da instrução decodificada, dependendo do tipo da instrução (R ou I).
	//registrador_destino_indice = mux_reg_destino(sinais_de_controle, instrucao_decodificada);

	// acesso a memoria (RETIRAR SINBAIS DE CONTROLE DPS)
	escrever_end_mem_dados(cpu, cpu->ex_mem.ula_saida, cpu->ex_mem.b, sinais_de_controle);
	valor_write_back = mux_write_back(cpu);
	valor_memoria = ler_end_mem_dados(cpu, cpu->ex_mem.ula_saida);

	// write back 
	escrever_registrador(cpu, cpu->mem_wb.reg_destino, valor_write_back, sinais_de_controle);
	
	 // Atualiza o RI para a próxima instrução a ser decodificada no próximo ciclo
	cpu->bi_di.pc_mais_um = somador_pc_mais_um(cpu); // Calcula o PC + 1 para ser usado no próximo ciclo, especialmente para instruções de branch
	cpu->pc = atualizar_pc(cpu, resultadoUla, instrucao_decodificada, sinais_de_controle);
	cpu->bi_di.ri = ler_end_mem_instrucao(cpu, cpu->pc);
	//resolver_desvio(cpu, instrucao_decodificada.imediato, instrucao_decodificada.endereco, sinais_de_controle, resultadoUla);
	
	cpu->di_ex.er.memoria_para_reg = sinais_de_controle.memoria_para_reg;
	cpu->di_ex.er.escrever_reg = sinais_de_controle.escrever_reg;
	cpu->di_ex.mem_sinais.branch = sinais_de_controle.branch;
	cpu->di_ex.mem_sinais.jump = sinais_de_controle.jump;
	cpu->di_ex.mem_sinais.escrever_memoria = sinais_de_controle.escrever_memoria;
	cpu->di_ex.ex_sinais.reg_destino = sinais_de_controle.reg_destino;
	cpu->di_ex.ex_sinais.ula_fonte = sinais_de_controle.ula_fonte;
	cpu->di_ex.ex_sinais.controle_ula = sinais_de_controle.controle_ula;
	cpu->di_ex.opcode = instrucao_decodificada.opcode;
	cpu->di_ex.a = valor_reg_a;
	cpu->di_ex.b = valor_reg_b;
	cpu->di_ex.imediato = instrucao_decodificada.imediato;
	cpu->di_ex.pc_mais_um = cpu->bi_di.pc_mais_um;
	cpu->di_ex.rd = instrucao_decodificada.rd;
	cpu->di_ex.rt = instrucao_decodificada.rt;

	cpu->ex_mem.er = cpu->di_ex.er;
	cpu->ex_mem.mem_sinais = cpu->di_ex.mem_sinais;
	cpu->ex_mem.opcode = cpu->di_ex.opcode;
	cpu->ex_mem.ula_saida = resultadoUla.resultado;
	cpu->ex_mem.b = cpu->di_ex.b;
	cpu->ex_mem.reg_destino = mux_reg_destino(cpu);

	cpu->mem_wb.er = cpu->ex_mem.er;
	cpu->mem_wb.memoria_saida = valor_memoria;
	cpu->mem_wb.opcode = cpu->ex_mem.opcode;
	cpu->mem_wb.ula_saida = cpu->ex_mem.ula_saida;
	cpu->mem_wb.reg_destino = cpu->ex_mem.reg_destino;

	debug_geral(instrucao_decodificada,instrucao, sinais_de_controle, resultadoUla, cpu, opcao_debug);
	
}

// Funçoes PIPELINE =================== :

static uint8_t mux_reg_destino(CPU* cpu) {
	if (cpu->di_ex.ex_sinais.reg_destino == 0) {
		return cpu->di_ex.rt; // Para instruções do tipo I, o destino é rt
	} else {
		return cpu->di_ex.rd; // Para instruções do tipo R, o destino é rd
	}
}
static uint8_t atualizar_pc(
	CPU *cpu, ResultadoUla resultadoUla, 
	InstrucaoDecodificada instrucao_decodificada,
	SinaisDeControle sinais_de_controle) {
	uint8_t pc_mais_um_ou_branch = mux_pc_mais_um_ou_branch(cpu, resultadoUla);

	return mux_pc_mais_um_ou_branch_ou_jump(cpu, sinais_de_controle, instrucao_decodificada, pc_mais_um_ou_branch);
}

static int8_t mux_write_back(CPU* cpu) {
	if (cpu->mem_wb.er.memoria_para_reg == 0) {
		return cpu->mem_wb.memoria_saida;
	} else {
		return cpu->mem_wb.ula_saida;
	}
}

static uint8_t mux_pc_mais_um_ou_branch_ou_jump(
	CPU *cpu, SinaisDeControle sinais_de_controle, 
	InstrucaoDecodificada instrucao_decodificada, 
	uint8_t pc_mais_um_ou_branch) {
	if (sinais_de_controle.jump) {
		return instrucao_decodificada.endereco; // Atualiza o PC para o endereço do jump
	} else {
		return pc_mais_um_ou_branch; // Incrementa o PC ou desvia condicionalmente
	}
}

static uint8_t mux_pc_mais_um_ou_branch(CPU *cpu, ResultadoUla resultadoUla) {
	if (desvio_condicional_tomado(cpu, resultadoUla) == 1) { // Verifica se é um branch e se o resultado da ULA é zero
		return somador_pc_branch(cpu); // Atualiza o PC para o endereço do branch
	} else {
		return somador_pc_mais_um(cpu); // Incrementa o PC normalmente
	}
}

static int desvio_condicional_tomado(CPU *cpu, ResultadoUla resultadoUla) {
	return cpu->di_ex.mem_sinais.branch && resultadoUla.zero; // Retorna 1 se o branch for tomado, caso contrário retorna 0
}

static uint8_t somador_pc_branch(CPU* cpu) {
	uint8_t endereco_branch = cpu->di_ex.pc_mais_um + cpu->di_ex.imediato; // endereço do branch
	return endereco_branch;
}

// Valor usado no MUX pc_mais_um ou branch e no reg Pc_mais_um do BI/DI
static uint8_t somador_pc_mais_um(CPU* cpu) {
	return cpu->pc + 1; // PC + 1
}
// ------------------------------

static void executar_programa_completo(CPU *cpu) {
	while (cpu->memoria_de_instrucao[cpu->pc] != 0) {
		executrar_ciclo(cpu, 0);
	}
}

static void iniciar_cpu(CPU *cpu)
{
	// Novoas variáveis para o pipeline:
	int posicao;
	cpu->pc = 0;
	cpu->bi_di = (BI_DI){0, 0};
	cpu->di_ex = (DI_EX){0};
	cpu->ex_mem = (EX_MEM){0};
	cpu->mem_wb = (MEM_WB){0};

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

