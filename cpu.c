#include <stdio.h>
#include "types.h"
#include "decodificador.h"
#include "conversor.h"
#include "controle.h"
#include "ula.h"
#include "memoria.h"
#include "utils.h"
#include "debug.h"
#include <stdbool.h>
/*Funções auxiliares*/
static void iniciar_cpu(CPU *cpu);
static void executrar_ciclo(CPU *cpu, int opcao_debug);
static void executar_programa_completo(CPU *cpu);
static void incrementar_pc(CPU *cpu, SinaisDeControle sinais_de_controle);
static void resolver_desvio(CPU *cpu, int8_t imediato, uint8_t endereco, SinaisDeControle sinais_de_controle, ResultadoUla resultadoUla);
static int8_t mux_operador_forward_di_ex(CPU* cpu,int8_t operador);
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
static int8_t mux_operador_ou_imediato_forward_di_ex(CPU* cpu,int8_t operador);
static int8_t mux_forward_di_ex(CPU *cpu, uint8_t src, int8_t valor);
static int8_t mux_forward_di_ex_store(CPU *cpu, uint8_t src, int8_t valor);
// STALL
static int verificar_stall_lw(CPU* cpu, InstrucaoDecodificada instrucao_decodificada);
static bool flush_branch_tomado(CPU* cpu, SinaisDeControle sinais_de_controle,ResultadoUla resultadoUla);
static bool flush_jump_tomado(CPU* cpu, SinaisDeControle sinais_de_controle);

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
	int8_t operador_a;
	int8_t operador_b;
	int8_t valor_write_back; // valor a ser escrito no banco de registradores (DMem ou ResultadoUla)
	uint8_t registrador_destino_indice; // índice do registrador a ser escrito no banco de registradores, selecionado pelo mux RegDest
	ResultadoUla resultadoUla; // Resultado das operações da ULA, incluindo o resultado e o sinal de zero (para branch)
	int8_t valor_memoria; // Valor lido da memória de dados (para instruções de load)	
	char instrucao_asm[INSTRUCAO_ASM_TAM]; // String para representação em ASM da instrução atual (para debug)
	char instrucao_asm_bi_di[INSTRUCAO_ASM_TAM]; // String para instrução buscada (BI/DI)
	bool flush_cpu;
	bool stall_lw;

	instrucao_decodificada = decodificar_instrucao(cpu->bi_di.ri);	
	

	instrucao_asm[0] = '\0';
	converter_para_asm(instrucao_decodificada, instrucao_asm);
	sinais_de_controle = gerar_sinais_de_controle(instrucao_decodificada.opcode, instrucao_decodificada.funct);
	valor_write_back = mux_write_back(cpu);
	escrever_registrador(cpu, cpu->mem_wb.reg_destino, valor_write_back, sinais_de_controle);
	valor_reg_a = ler_registrador(cpu, instrucao_decodificada.rs);
	valor_reg_b = ler_registrador(cpu, instrucao_decodificada.rt);
	if (sinais_de_controle.branch) {
		valor_reg_a = mux_forward_di_ex(cpu, instrucao_decodificada.rs, valor_reg_a);
		valor_reg_b = mux_forward_di_ex(cpu, instrucao_decodificada.rt, valor_reg_b);
	}

	// lidar com o forward das operacoes da ula.
	operador_a = mux_operador_forward_di_ex(cpu, cpu->di_ex.a);
	operador_b = mux_operador_ou_imediato_forward_di_ex(cpu, cpu->di_ex.b);
	resultadoUla = executar(operador_a, operador_b, cpu->di_ex.ex_sinais.controle_ula);

	// forward caso store word precise do valor atualizado do registrador (estagio EX)
	int8_t valor_store_ex = cpu->di_ex.b;
	if (cpu->di_ex.mem_sinais.escrever_memoria) {
		valor_store_ex = mux_forward_di_ex_store(cpu, cpu->di_ex.rt, valor_store_ex);
	}

	// acesso a memoria | TODO: (RETIRAR SINBAIS DE CONTROLE DPS)
	escrever_end_mem_dados(cpu, cpu->ex_mem.ula_saida, cpu->ex_mem.b, sinais_de_controle);
	//valor_write_back = mux_write_back(cpu);
	valor_memoria = ler_end_mem_dados(cpu, (uint8_t)(cpu->ex_mem.ula_saida));

	
	 // Atualiza o RI para a próxima instrução a ser decodificada no próximo ciclo
	instrucao = cpu->bi_di.ri;
	CPU cpu_antes = *cpu; // Snapshot para mostrar o estado antes e depois do update do pipeline

	//resolver_desvio(cpu, instrucao_decodificada.imediato, instrucao_decodificada.endereco, sinais_de_controle, resultadoUla);
	// Atualizar os registradores de pipeline (DI/EX, EX/MEM, MEM/WB) e o PC
	cpu->mem_wb.er = cpu->ex_mem.er;
	cpu->mem_wb.memoria_saida = valor_memoria;
	cpu->mem_wb.opcode = cpu->ex_mem.opcode;
	cpu->mem_wb.ula_saida = cpu->ex_mem.ula_saida;
	cpu->mem_wb.reg_destino = cpu->ex_mem.reg_destino;
	cpu->mem_wb.mem_sinais = cpu->ex_mem.mem_sinais;
	snprintf(cpu->mem_wb.instrucao_asm, INSTRUCAO_ASM_TAM, "%s", cpu->ex_mem.instrucao_asm);

	cpu->ex_mem.er = cpu->di_ex.er;
	cpu->ex_mem.mem_sinais = cpu->di_ex.mem_sinais;
	cpu->ex_mem.opcode = cpu->di_ex.opcode;
	cpu->ex_mem.ula_saida = resultadoUla.resultado;
	cpu->ex_mem.b = valor_store_ex;
	cpu->ex_mem.reg_destino = mux_reg_destino(cpu);
	snprintf(cpu->ex_mem.instrucao_asm, INSTRUCAO_ASM_TAM, "%s", cpu->di_ex.instrucao_asm);

	// Guardar os proximos valores para o BEQ funcionar corretamente antes de substituir o PC
	uint8_t pc_atual = cpu->pc;
	uint8_t proximo_pc = atualizar_pc(cpu, resultadoUla, instrucao_decodificada, sinais_de_controle);
	stall_lw = verificar_stall_lw(cpu, instrucao_decodificada);
	flush_cpu = flush_branch_tomado(cpu, sinais_de_controle, resultadoUla);
	if (!flush_cpu && !stall_lw) {
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
	cpu->di_ex.rs = instrucao_decodificada.rs;
	snprintf(cpu->di_ex.instrucao_asm, INSTRUCAO_ASM_TAM, "%s", instrucao_asm);

	if (!flush_jump_tomado(cpu, sinais_de_controle)) {
	cpu->bi_di.pc_mais_um = (uint8_t)(pc_atual + 1); // PC+1 do endereco que foi buscado
	cpu->bi_di.ri = ler_end_mem_instrucao(cpu, pc_atual);
	instrucao_asm_bi_di[0] = '\0';
	converter_para_asm(decodificar_instrucao(cpu->bi_di.ri), instrucao_asm_bi_di);
	snprintf(cpu->bi_di.instrucao_asm, INSTRUCAO_ASM_TAM, "%s", instrucao_asm_bi_di);
	}
	}

	if (!stall_lw) {
		cpu->pc = proximo_pc;
	}

	



	debug_pipeline(&cpu_antes, cpu, opcao_debug);
	debug_geral(instrucao_decodificada,instrucao, sinais_de_controle, resultadoUla, cpu, opcao_debug);

	
}

// Funçoes PIPELINE =================== :
static bool flush_jump_tomado(CPU* cpu, SinaisDeControle sinais_de_controle) {
	if (sinais_de_controle.jump) {
		printf("mini-mips-info: Jump tomado! Fazendo flush.\n");

		// Flush: zerar BI/DI e DI/EX
		cpu->bi_di.ri = 4096;
		snprintf(cpu->bi_di.instrucao_asm, INSTRUCAO_ASM_TAM, "NOP");
		cpu->bi_di.pc_mais_um = 0;
		//sprintf(cpu->bi_di.instrucao_asm, INSTRUCAO_ASM_TAM, "NOP");

		return true;
	}
	return false;
}

static bool flush_branch_tomado(CPU* cpu, SinaisDeControle sinais_de_controle,ResultadoUla resultadoUla) {
	if (desvio_condicional_tomado(cpu, resultadoUla)) {
    printf("mini-mips-info: Branch tomado! Fazendo flush.\n");

    // Flush: zerar BI/DI e DI/EX
	cpu->bi_di.ri = 4096;
	snprintf(cpu->bi_di.instrucao_asm, INSTRUCAO_ASM_TAM, "NOP");
	cpu->bi_di.pc_mais_um = 0;
    cpu->di_ex = (DI_EX){0};
	snprintf(cpu->di_ex.instrucao_asm, INSTRUCAO_ASM_TAM, "NOP");
    return true;
    // EX/MEM e MEM/WB continuam normalmente
	}
return false;
}
int verificar_stall_lw(CPU* cpu, InstrucaoDecodificada instrucao_decodificada) {
	bool lw_use = (cpu->di_ex.opcode == OPCODE_LW) &&
              ((instrucao_decodificada.rs == cpu->di_ex.rt) ||
               (instrucao_decodificada.rt == cpu->di_ex.rt));

	if (lw_use) {
		printf("mini-mips-hazard: Load-use detectado! Inserindo stall.\n");
		// Não atualizar PC
		// Não atualizar BI/DI
		// Injetar bubble em DI/EX
		snprintf(cpu->di_ex.instrucao_asm, INSTRUCAO_ASM_TAM, "NOP");
		cpu->di_ex.er = (ErSinais){0};
		cpu->di_ex.mem_sinais = (MemSinais){0};
		cpu->di_ex.ex_sinais = (ExSinais){0};
		cpu->di_ex.a = 0;
		cpu->di_ex.b = 0;
		cpu->di_ex.imediato = 0;
		cpu->di_ex.opcode = 0;
		cpu->di_ex.rd = 0;
		cpu->di_ex.rt = 0;
		cpu->di_ex.rs = 0;
		cpu->di_ex.pc_mais_um = 0;
		return 1; // Retornar 1 indicando que houve stall
	}
	return 0; // Retornar 0 se não houve stallcpu->di_ex.ex_sinais = (ExSinais){0};
		return; // Sair sem atualizar o resto do pipeline
	}

static uint8_t mux_reg_destino(CPU* cpu) {
	if (cpu->di_ex.ex_sinais.reg_destino == 0) {
		return cpu->di_ex.rt; // Para instruções do tipo I, o destino é rt
	} else {
		return cpu->di_ex.rd; // Para instruções do tipo R, o destino é rd
	}
}

static int8_t mux_forward_di_ex(CPU *cpu, uint8_t src, int8_t valor) {
	if (cpu->ex_mem.er.escrever_reg &&
		cpu->ex_mem.reg_destino == src) {
		printf("Forward: Registrador %d com valor %d de EX/MEM para ID/EX\n", src, cpu->ex_mem.ula_saida);
		return cpu->ex_mem.ula_saida;
	}
	if (cpu->mem_wb.er.escrever_reg &&
		cpu->mem_wb.reg_destino == src) {
		printf("Forward: Registrador %d com valor %d de MEM/WB para ID/EX\n", src, (cpu->mem_wb.er.memoria_para_reg == 0) ? cpu->mem_wb.memoria_saida : cpu->mem_wb.ula_saida);
		return (cpu->mem_wb.er.memoria_para_reg == 0)
			? cpu->mem_wb.memoria_saida
			: cpu->mem_wb.ula_saida;
	}
	return valor;
}

static int8_t mux_operador_forward_di_ex(CPU* cpu, int8_t operador) {
	if (cpu->ex_mem.er.escrever_reg &&
        cpu->ex_mem.reg_destino == cpu->di_ex.rs) {
        printf("Forward: Registrador %d com valor %d de EX/MEM para DI/EX (OPERADOR A)\n", cpu->di_ex.rs, cpu->ex_mem.ula_saida);
        return cpu->ex_mem.ula_saida;
    }

	if (cpu->mem_wb.er.escrever_reg &&
        cpu->mem_wb.reg_destino == cpu->di_ex.rs) {
        printf("Forward: Registrador %d com valor %d de MEM/WB para DI/EX (OPERADOR A)\n", cpu->di_ex.rs, (cpu->mem_wb.er.memoria_para_reg == 0) ? cpu->mem_wb.memoria_saida : cpu->mem_wb.ula_saida);
        return (cpu->mem_wb.er.memoria_para_reg == 0)
            ? cpu->mem_wb.memoria_saida
            : cpu->mem_wb.ula_saida;
    }

    return operador;
}

static int8_t mux_operador_ou_imediato_forward_di_ex(CPU* cpu, int8_t operador) {
    // Se for imediato, nao faz forwarding em B
    if (cpu->di_ex.ex_sinais.ula_fonte == 1) {
        return cpu->di_ex.imediato;
    }

	if (cpu->ex_mem.er.escrever_reg &&
        cpu->ex_mem.reg_destino == cpu->di_ex.rt) {
        printf("Forward: Registrador %d com valor %d de EX/MEM para DI/EX (OPERADOR B)\n", cpu->di_ex.rt, cpu->ex_mem.ula_saida);
        return cpu->ex_mem.ula_saida;
    }

	if (cpu->mem_wb.er.escrever_reg &&
        cpu->mem_wb.reg_destino == cpu->di_ex.rt) {
		printf("Forward: Registrador %d com valor ", cpu->di_ex.rt);
		printf("%d de MEM/WB para DI/EX (OPERADOR B)\n", (cpu->mem_wb.er.memoria_para_reg == 0) ? cpu->mem_wb.memoria_saida : cpu->mem_wb.ula_saida);
        return (cpu->mem_wb.er.memoria_para_reg == 0)
            ? cpu->mem_wb.memoria_saida
            : cpu->mem_wb.ula_saida;
    }

    return operador;
}

static int8_t mux_forward_di_ex_store(CPU *cpu, uint8_t src, int8_t valor) {
	if (cpu->ex_mem.er.escrever_reg &&
		cpu->ex_mem.reg_destino == src &&
		cpu->ex_mem.er.memoria_para_reg == 1) {
		printf("Forward: Registrador %d com valor %d de EX/MEM para DI/EX (STORE)\n", src, cpu->ex_mem.ula_saida);
		return cpu->ex_mem.ula_saida;
	}
	if (cpu->mem_wb.er.escrever_reg &&
		cpu->mem_wb.reg_destino == src) {
		printf("Forward: Registrador %d com valor %d de MEM/WB para DI/EX (STORE)\n", src, (cpu->mem_wb.er.memoria_para_reg == 0) ? cpu->mem_wb.memoria_saida : cpu->mem_wb.ula_saida);
		return (cpu->mem_wb.er.memoria_para_reg == 0)
			? cpu->mem_wb.memoria_saida
			: cpu->mem_wb.ula_saida;
	}
	return valor;
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
	
	if (desvio_condicional_tomado(cpu, resultadoUla) == 1) { 
		// Verifica se é um branch e se o resultado da ULA é zero
		return somador_pc_branch(cpu); // Atualiza o PC para o endereço do branch
	} else {
		return somador_pc_mais_um(cpu); // Incrementa o PC normalmente
	}
}

static int desvio_condicional_tomado(CPU *cpu, ResultadoUla resultadoUla) {
	return cpu->di_ex.mem_sinais.branch && resultadoUla.zero; // Retorna 1 se o branch for tomado, caso contrário retorna 0
}

static uint8_t somador_pc_branch(CPU* cpu) {
	int8_t pc_mais_um = (int8_t)cpu->di_ex.pc_mais_um; // PC + 1 calculado no ciclo anterior
	uint8_t endereco_branch = (uint8_t)(pc_mais_um + cpu->di_ex.imediato); 
	printf("Endereço do branch: %u\n", endereco_branch);
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
	cpu->estatistica = (Estatisticas){0};
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

