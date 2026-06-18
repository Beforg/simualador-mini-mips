#ifndef DEBUG_H
#define DEBUG_H

#include "core/cpu.h"
#include <stdbool.h>

#define PREFIX "mini-mips"
#define PREFIX_ERRO "mini-mips-err"
#define PREFIX_INFO "mini-mips-info"
#define PREFIX_WARNING "mini-mips-warn"

#define print_info(str)  _log_internal(PREFIX_INFO, str)        // Usar para dar print em uma informação
#define print_err(str)   _log_internal(PREFIX_ERRO, str)        // usar para dar print em algum erro.
#define print_warn(str)  _log_internal(PREFIX_WARNING, str)     // Usar para dar print em warning.

// Função que verifica se o debug irá funcionar ou não.
void set_debug(bool ativar);

// Debug antigo para mostrar para teste
void debug_legacy(
    const InstrucaoDecodificada instrucao_decodificada,
    const SinaisDeControle sinais_de_controle,
    const ResultadoUla resultadoUla, const CPU *cpu);
    
// Esse só vai funcionar se ativar o set_debug para true
void debug_geral(
    const InstrucaoDecodificada inst,
    const uint16_t instrucao,
    const SinaisDeControle sinais,
    const ResultadoUla ula,
    const CPU *cpu,
    int opcao);
    // opcao: 0 = Decimal, 1 = Hexa, 2 = Binario

// Debug do pipeline por estagio (BI/DI, DI/EX, EX/MEM, MEM/WB)
void debug_pipeline(const CPU *cpu_antes, const CPU *cpu_depois, int opcao);

// Funcões de print de inteiros <stdint.h> para binário e hexadecimal

void debug_binario();
void estado_atual_cpu(const CPU *cpu, int opcao);

#endif