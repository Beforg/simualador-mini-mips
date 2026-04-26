#include <stdio.h>
#include <stdlib.h>

#include "memoria.h"
#include "debug.h"
#include "utils.h"
#include "conversor.h"
// Variável de controle do debug, por padrão, é false.
static unsigned int debugAtivado = false;

#pragma region FN_PUBLICAS

// Função para iniciar o log. Caso seja falsa, nenhum debug é escrito.

void set_debug(bool estado)
{
    if (estado)
    {
        debugAtivado = estado;
        return;
    }
    debugAtivado = false;
}

void debug_legacy(const InstrucaoDecodificada instrucao_decodificada, const SinaisDeControle sinais_de_controle, const ResultadoUla resultadoUla, const CPU *cpu)
{
    if (!debugAtivado)
    {
        return;
    }

    printf("PC: %u\n", cpu->pc - 1);
    printf("Instrução executada: ");
    int16_para_binario(cpu->memoria[cpu->pc - 1]);
    printf("\n======= Instrução Decodificada ======= \n");
    printf("Tipo: ");
    int8_para_binario(instrucao_decodificada.tipo);
    printf(" Opcode: ");
    int8_para_binario(instrucao_decodificada.opcode);
    printf("\n RS: ");
    int8_para_binario(instrucao_decodificada.rs);

    printf(" RT: ");
    int8_para_binario(instrucao_decodificada.rt);
    printf("\n RD: ");
    int8_para_binario(instrucao_decodificada.rd);
    printf(" Funct: ");
    int8_para_binario(instrucao_decodificada.funct);
    printf("\nImediato: %d\n", instrucao_decodificada.imediato);
    printf("Endereco: %u\n", instrucao_decodificada.endereco);

    printf("======= Sinais de Controle Gerados ======= \n");
    printf("Controle ULA: %d\n", sinais_de_controle.controle_ula);
    printf("Escrever Memoria: %d\n", sinais_de_controle.escrever_memoria);
    printf("Escrever Reg: %d\n", sinais_de_controle.escrever_reg);
    printf("Memoria para Reg: %d\n", sinais_de_controle.memoria_para_reg);
    printf("ULA Fonte B: %d\n", sinais_de_controle.ula_fonte_b);
    printf("Reg Destino: %d\n", sinais_de_controle.reg_destino);
    printf("Incremento PC: %d\n", sinais_de_controle.incremento_pc);
    printf("Jump: %d\n", sinais_de_controle.jump);
    printf("Branch: %d\n", sinais_de_controle.branch);

    printf("\n======= Resultado da Operação na Ula =======\n");
    printf("Resultado: %d\n", resultadoUla.resultado);
    printf("Zero: %u\n", resultadoUla.zero);

    printf("PC novo: %u | ", cpu->pc);
    printf("Instrução atual: %u\n", cpu->memoria[cpu->pc]);
}

void estado_atual_cpu(const CPU *cpu, int opcao)
{
    const char *modo = (opcao == 1) ? "HEXADECIMAL" : (opcao == 2) ? "BINÁRIO"
                                                                   : "DECIMAL";
    uint16_t instrucao_atual = cpu->memoria[cpu->pc];

    printf("\n╔══════════════════════════════════════════════════════════════════════╗");
    printf("\n║  ESTADO DA CPU - MODO: %-11s | Debug: %s                                      ", modo, debugAtivado ? "ATIVADO" : "DESATIVADO"  );
    printf("\n╠══════════════════════════════════════════════════════════════════════╣");

    // Seção do PC e Instrução
    if (opcao == 1)
    {
        printf("\n║  PC: %u (0x%02X)  │  Instrução (RI): ", cpu->pc, cpu->pc);
    }
    else if (opcao == 2)
    {
        printf("\n║  PC: %u (", cpu->pc);
        int8_para_binario(cpu->pc);
        printf(")  │  Instrução (RI): ");
    }
    else
    {
        printf("\n║  PC: %u  │  Instrução (RI): ", cpu->pc);
    }
    if (opcao == 1)
    {
        printf("0x%04X                      ", cpu->ri);
    }
    else if (opcao == 2)
    {
        int16_para_binario(cpu->ri);
        printf("      ");
    }
    else
    {
        printf("%-5u (signed: %-5d)          ", cpu->ri, (int16_t)cpu->ri);
    }

    printf("\n╠══════════════════════════════════════════════════════════════════════╣");
    printf("\n║  BANCO DE REGISTRADORES:                                             ║\n║  ");

    // Exibição dos Registradores em duas linhas (4 por linha) para melhor leitura
    for (int i = 0; i < 8; i++)
    {
        int8_t reg_val = cpu->banco_de_regs[i];

        printf("R%d: ", i);
        if (opcao == 1)
        {
            printf("0x%02X  ", (uint8_t)reg_val);
        }
        else if (opcao == 2)
        {
            int8_para_binario(reg_val);
            printf(" ");
        }
        else
        {
            printf("%4d  ", reg_val);
        }

        // Formatação de colunas
        if (i == 3)
        {
            printf(" ║\n║  "); // Quebra para a segunda linha de registradores
        }
        else if (i == 7)
        {
            printf(" ║");
        }
        else
        {
            printf("║  "); // Divisor entre colunas
        }
    }

    printf("\n╚══════════════════════════════════════════════════════════════════════╝\n");
}

void debug_geral(const InstrucaoDecodificada inst,
                 const uint16_t instrucao,
                 const SinaisDeControle sinais,
                 const ResultadoUla ula,
                 const CPU *cpu,
                 int opcao) // opcao: 0 = Decimal, 1 = Hexa, 2 = Binario
{   
    char pseudo_instrucao[64] = {0};
    converter_para_asm(inst, pseudo_instrucao);

    if (!debugAtivado)
    {
        return;
    }


    printf("\n============ DEBUG CPU =====================\n");

    // 1. PC e Instrução (16 bits)

    printf("%s", pseudo_instrucao[0] ? pseudo_instrucao : "Instrucao Pseudo: N/A");
    printf("\nPC Atualizado: ");
    if (opcao == 1)
        int8_hexa(cpu->pc);
    else if (opcao == 2)
        int8_para_binario(cpu->pc);
    else
        printf("%d", cpu->pc);

    printf(" | Instrucao sendo executada: ");
    if (opcao == 1)
        int16_hexa(cpu->ri);
    else if (opcao == 2)
        int16_para_binario(cpu->ri);
    else
        printf("%d", instrucao);

    // 2. Campos Decodificados (8 bits)
    printf("\n\n====== Instrução Decodificada ======");
    switch (inst.tipo){
        case TIPO_R:
            printf("\nTipo: R \n");

            printf("\nOpcode: ");
            opcao == 1 ? int8_hexa(inst.opcode) : (opcao == 2 ? print_int_4bits(inst.opcode) : printf("%d", inst.opcode));
            
            printf(" | RS: ");
            opcao == 1 ? int8_hexa(inst.rs) : (opcao == 2 ? print_int_3bits(inst.rs) : printf("%d", inst.rs));
            
            printf(" | RT: ");
            opcao == 1 ? int8_hexa(inst.rt) : (opcao == 2 ? print_int_3bits(inst.rt) : printf("%d", inst.rt));
            
            printf(" | RD: ");
            opcao == 1 ? int8_hexa(inst.rd) : (opcao == 2 ? print_int_3bits(inst.rd) : printf("%d", inst.rd));
            
            printf(" | Funct: ");
            opcao == 1 ? int8_hexa(inst.funct) : (opcao == 2 ? print_int_4bits(inst.funct) : printf("%d", inst.funct));
    
            break;
        case TIPO_I:
            printf("\nTipo: I \n");

            printf("\nOpcode: ");
            opcao == 1 ? int8_hexa(inst.opcode) : (opcao == 2 ? print_int_4bits(inst.opcode) : printf("%d", inst.opcode));
            
            printf(" | RS: ");
            opcao == 1 ? int8_hexa(inst.rs) : (opcao == 2 ? print_int_3bits(inst.rs) : printf("%d", inst.rs));
            
            printf(" | RT: ");
            opcao == 1 ? int8_hexa(inst.rt) : (opcao == 2 ? print_int_3bits(inst.rt) : printf("%d", inst.rt));
            
            printf(" | Imediato: ");
            opcao == 1 ? int8_hexa(inst.imediato) : (opcao == 2 ? int8_para_binario(inst.imediato) : printf("%d", inst.imediato));
    
            break;
        case TIPO_J:
            printf("\nTipo: J \n");

            printf("\nOpcode: ");
            opcao == 1 ? int8_hexa(inst.opcode) : (opcao == 2 ? print_int_4bits(inst.opcode) : printf("%d", inst.opcode));

            printf(" | Endereco: ");
            opcao == 1 ? int8_hexa(inst.endereco) : (opcao == 2 ? int8_para_binario(inst.endereco) : printf("%u", inst.endereco));

            break;
        default:
            printf("\nTipo: INVALIDO\n");
            break;
    }

    // 3. Sinais de Controle (Sempre Binário como no Logisim)
    printf("\n\n============ Sinais de Controle ===============");
    
    printf("\nIR Atualizado: %04x | ", cpu->ri);
    printf("RDM Atualizado: %04x | ", cpu->rdm);
    printf("A: %d | ", cpu->a);
    printf("B: %d | ", cpu->b);
    printf("Saida ULA: %d\n", cpu->saida_ula);
    printf("Estado Executado: %d\n", cpu->estado_atual);
    printf("UlaFonteA: %d | ", sinais.ula_fonte_a);
    printf("I ou D: %d | ", sinais.i_ou_d);
    printf("ULA Ctrl: ");
    if (opcao == 1)
    {
        int8_hexa(sinais.controle_ula);
         printf(" | EscMem: 0x%02X | EscReg: 0x%02X | Mem2Reg: 0x%02X | UlaFonteB: 0x%02X",
             sinais.escrever_memoria, sinais.escrever_reg, sinais.memoria_para_reg, sinais.ula_fonte_b);
        printf("\n | RegDest: 0x%02X | IncPC: 0x%02X | Jump: 0x%02X | Branch: 0x%02X",
               sinais.reg_destino, sinais.incremento_pc, sinais.jump, sinais.branch);
    }
    else if (opcao == 2)
    {
        print_int_3bits(sinais.controle_ula);
        printf(" | EscMem: ");
        printf("%d", sinais.escrever_memoria);
        printf(" | EscReg: ");
        printf("%d", sinais.escrever_reg);
        printf(" | Mem2Reg: ");
        printf("%d", sinais.memoria_para_reg);
        printf(" | UlaFonte: ");
        printf("%d", sinais.ula_fonte_b);
        printf(" | RegDest: ");
        printf("%d", sinais.reg_destino);
        printf(" | IncPC: ");
        printf("%d", sinais.incremento_pc);
        printf(" | Jump: ");
        printf("%d", sinais.jump);
        printf(" | Branch: ");
        printf("%d", sinais.branch);
    }
    else
    {
        printf("%d", sinais.controle_ula);
         printf(" | EscMem: %d | EscReg: %d | Mem2Reg: %d | UlaFonteB: %d",
             sinais.escrever_memoria, sinais.escrever_reg, sinais.memoria_para_reg, sinais.ula_fonte_b);
        printf(" | RegDest: %d | IncPC: %d | Jump: %d | Branch: %d",
               sinais.reg_destino, sinais.incremento_pc, sinais.jump, sinais.branch);
    }

    // 4. ULA
    printf("\n\n============ ULA ===============================\n");
    printf("Resultado: ");
    if (opcao == 1)
        int8_hexa(ula.resultado);
    else if (opcao == 2)
        int8_para_binario(ula.resultado);
    else
        printf("%d", ula.resultado);
    printf(" | Zero: %u", ula.zero);

    // 5. Memórias e Registradores (Reutilizando sua função)
    // imprimirMemoria((CPU*)cpu, REGISTRADOR, opcao);
    // imprimirMemoria((CPU*)cpu, DADOS, opcao);

    printf("\n================================================\n");
}

#pragma endregion FN_PUBLICAS

#pragma region FN_PRIVADAS

// Função de log interna para o macro, ela é privada. Você pode ignorar
static void _log_internal(const char *prefix, const char *str)
{
    if (debugAtivado)
    {
        if (prefix && str)
        {
            printf("%s:%s\n", prefix, str);
        }
    }
}
void debug_decimal(const InstrucaoDecodificada inst,
                   const SinaisDeControle sinais,
                   const ResultadoUla ula,
                   const CPU *cpu)
{

    printf("\n--- DEBUG COMPLETO (DECIMAL) ---\n");
    printf("PC: %d | Instrução: %d\n", cpu->pc - 1, cpu->memoria[cpu->pc - 1]);

    printf("--- Campos da Instrução ---\n");
    printf("Tipo: %d | Opcode: %d | RS: %d | RT: %d | RD: %d | Funct: %d\n",
           inst.tipo, inst.opcode, inst.rs, inst.rt, inst.rd, inst.funct);
    printf("Imediato: %d | Endereço: %u\n", inst.imediato, inst.endereco);

    printf("--- Sinais de Controle ---\n");
        printf("ULA_Ctrl: %d | MemWrite: %d | RegWrite: %d | MemToReg: %d | ALUSrcB: %d\n",
            sinais.controle_ula, sinais.escrever_memoria, sinais.escrever_reg, sinais.memoria_para_reg, sinais.ula_fonte_b);
    printf("RegDest: %d | Jump: %d | Branch: %d\n", sinais.reg_destino, sinais.jump, sinais.branch);

    printf("--- ULA & Memória ---\n");
    printf("Resultado ULA: %d | Flag Zero: %u\n", ula.resultado, ula.zero);
    printf("PC Novo: %d\n", cpu->pc);
}

void debug_binario(const InstrucaoDecodificada inst,
                   const SinaisDeControle sinais,
                   const ResultadoUla ula,
                   const CPU *cpu)
{

    printf("\n--- DEBUG COMPLETO (BINÁRIO) ---\n");
    printf("PC: ");
    int16_para_binario(cpu->pc - 1);
    printf(" | Instrução: ");
    int16_para_binario(cpu->memoria[cpu->pc - 1]);

    printf("\n--- Campos da Instrução ---\n");
    printf("Opcode: ");
    int8_para_binario(inst.opcode);
    printf(" | RS: ");
    int8_para_binario(inst.rs);
    printf(" | RT: ");
    int8_para_binario(inst.rt);
    printf(" | RD: ");
    int8_para_binario(inst.rd);
    printf("\nFunct:  ");
    int8_para_binario(inst.funct);
    printf(" | Immed: ");
    int16_para_binario(inst.imediato);

    printf("\n--- Sinais de Controle ---\n");
    printf("ULA_Ctrl: ");
    int8_para_binario(sinais.controle_ula);
    printf(" | MemW: %d | RegW: %d | M2R: %d | Src: %d | J: %d | B: %d\n",
           sinais.escrever_memoria, sinais.escrever_reg, sinais.memoria_para_reg,
           sinais.ula_fonte_b, sinais.jump, sinais.branch);

    printf("--- ULA & Memória ---\n");
    printf("Resultado: ");
    int16_para_binario(ula.resultado);
    printf(" | Zero: %u\n", ula.zero);
}

static void debug_hexa(const InstrucaoDecodificada inst,
                       const SinaisDeControle sinais,
                       const ResultadoUla ula,
                       const CPU *cpu)
{
    printf("\n--- DEBUG COMPLETO (HEXADECIMAL) ---\n");
    printf("PC: ");
    int16_hexa(cpu->pc - 1);
    printf(" | Instrução: ");
    int16_hexa(cpu->memoria[cpu->pc - 1]);

    printf("\n--- Campos da Instrução ---\n");
    printf("Opcode: ");
    int8_hexa(inst.opcode);
    printf(" | RS: ");
    int8_hexa(inst.rs);
    printf(" | RT: ");
    int8_hexa(inst.rt);
    printf(" | RD: ");
    int8_hexa(inst.rd);
    printf("\nImediato: ");
    int16_hexa(inst.imediato);
    printf(" | Endereço: ");
    int16_hexa(inst.endereco);

    printf("\n--- Sinais de Controle ---\n");
    printf("ULA_Ctrl: ");
    int8_hexa(sinais.controle_ula);
    printf(" | Sinais (W/R/J/B): %X%X%X%X",
           sinais.escrever_memoria, sinais.escrever_reg, sinais.jump, sinais.branch);

    printf("\n--- ULA & Memória ---\n");
    printf("Resultado ULA: ");
    int16_hexa(ula.resultado);
    printf(" | Zero: %u\n", ula.zero);

    printf("--- Memórias ---\n");
    imprimirMemoria(cpu, DADOS, HEXADECIMAL);
    imprimirMemoria(cpu, INSTRUCAO, HEXADECIMAL);
}

#pragma endregion FN_PRIVADAS
