
#include "types.h"
#include <stdint.h>
#include "decodificador.h"
#include "utils.h"
#include <stdio.h>

static uint8_t obter_opcode(uint16_t instr);
static uint8_t extrair_rs(uint16_t instr);
static uint8_t extrair_rt(uint16_t instr);
static uint8_t extrair_rd(uint16_t instr);
static uint8_t extrair_funct(uint16_t instr);
static int8_t extrair_imediato(uint16_t instr);
static uint8_t extrair_endereco(uint16_t instr);
static TipoInstrucao determinar_tipo_instrucao(uint8_t opcode);

static void decodificar_tipo_r(
    InstrucaoDecodificada *instr_decodificada,
    const uint16_t instrucao,
    const TipoInstrucao tipo_instrucao);

static void decodificar_tipo_i(
    InstrucaoDecodificada *instr_decodificada,
    const uint16_t instrucao,
    const TipoInstrucao tipo_instrucao);

static void decodificar_tipo_j(
    InstrucaoDecodificada *instr_decodificada,
    const uint16_t instrucao,
    const TipoInstrucao tipo_instrucao);

static void decodificar(
    InstrucaoDecodificada *instr_decodificada,
    TipoInstrucao tipo_instrucao,
    const uint16_t instrucao);

InstrucaoDecodificada decodificar_instrucao(uint16_t instrucao)
{
    InstrucaoDecodificada instrucao_decodificada = {0};
    TipoInstrucao tipo_instrucao = determinar_tipo_instrucao(obter_opcode(instrucao));
    decodificar(&instrucao_decodificada, tipo_instrucao, instrucao);

    return instrucao_decodificada;
}

static void decodificar(InstrucaoDecodificada *instr_decodificada, TipoInstrucao tipo_instrucao, const uint16_t instrucao)
{

    switch (tipo_instrucao)
    {
    case TIPO_R:
        decodificar_tipo_r(instr_decodificada, instrucao, tipo_instrucao);
        break;
    case TIPO_I:
        decodificar_tipo_i(instr_decodificada, instrucao, tipo_instrucao);
        break;
    case TIPO_J:
        decodificar_tipo_j(instr_decodificada, instrucao, tipo_instrucao);
        break;
    default:
        instr_decodificada->tipo = TIPO_INVALIDO;
        break;
    }
}

static TipoInstrucao determinar_tipo_instrucao(uint8_t opcode)
{
    if (opcode == OPCODE_R)
        return TIPO_R;

    else if (opcode == OPCODE_J)
        return TIPO_J;

    else if (opcode == OPCODE_I1 || opcode == OPCODE_I2 || opcode == OPCODE_I3 || opcode == OPCODE_I4)
        return TIPO_I;

    else
        return TIPO_INVALIDO;
}

/**
 * Funções auxiliares para extrair campos da instrução e determinar o tipo de instrução
 * Vão decodificar os campos usando uma mascara de bits e deslocamento para obter os valores corretos
 * O tipo de instrução é determinado com base no opcode, usando uma série de condições para
 * classificar a instrução como R, I, J ou inválida
 * As funções de decodificação para cada tipo de instrução vão preencher os campos daquela classe de instrução.
 */

static uint8_t obter_opcode(uint16_t instrucao)
{
    return (uint8_t)(instrucao >> 12) & 0x0F; // 0100000000000000 >> 12 -> 0000000000001000 & 000000000000001111 -> 0001000 = 8
}

static uint8_t extrair_rs(uint16_t instrucao)
{
    return (uint8_t)(instrucao >> 9) & 0x07; // 0x07 é 111 em binário, para extrair os 3 bits de rs
}

static uint8_t extrair_rt(uint16_t instrucao)
{
    return (uint8_t)(instrucao >> 6) & 0x07; // Mesma coisa aqui
}

static uint8_t extrair_rd(uint16_t instrucao)
{
    return (uint8_t)(instrucao >> 3) & 0x07;
}

static uint8_t extrair_funct(uint16_t instrucao)
{
    return (uint8_t)(instrucao & 0x07); // sem shift por que já esta nos 3 bits menos significativos
}

static int8_t extrair_imediato(uint16_t instrucao)
{
    return extender_sinal(instrucao); // 6 bits finais que vão ser extendidos.
}

static uint8_t extrair_endereco(uint16_t instrucao)
{
    return (uint8_t)(instrucao & 0xFF); // 8 bits finais para o endereço
}

static void decodificar_tipo_r(
    InstrucaoDecodificada *instr_decodificada,
    const uint16_t instrucao,
    const TipoInstrucao TipoInstrucao)
{

    instr_decodificada->tipo = TipoInstrucao;
    instr_decodificada->opcode = obter_opcode(instrucao);
    instr_decodificada->rs = extrair_rs(instrucao);
    instr_decodificada->rt = extrair_rt(instrucao);
    instr_decodificada->rd = extrair_rd(instrucao);
    instr_decodificada->funct = extrair_funct(instrucao);
}

static void decodificar_tipo_i(
    InstrucaoDecodificada *instr_decodificada,
    const uint16_t instrucao,
    const TipoInstrucao TipoInstrucao)
{
    instr_decodificada->tipo = TipoInstrucao;
    instr_decodificada->opcode = obter_opcode(instrucao);
    instr_decodificada->rs = extrair_rs(instrucao);
    instr_decodificada->rt = extrair_rt(instrucao);
    instr_decodificada->imediato = extrair_imediato(instrucao);
    instr_decodificada->funct = 0;
    instr_decodificada->rd = 0;
    instr_decodificada->endereco = 0;

}
static void decodificar_tipo_j(
    InstrucaoDecodificada *instr_decodificada,
    const uint16_t instrucao,
    const TipoInstrucao TipoInstrucao)
{
    instr_decodificada->tipo = TipoInstrucao;
    instr_decodificada->opcode = obter_opcode(instrucao);
    instr_decodificada->endereco = extrair_endereco(instrucao);
}