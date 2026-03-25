# include "ula.h"

static int8_t operacao_add(int8_t a, int8_t b);
static int8_t operacao_sub(int8_t a, int8_t b);
static int8_t operacao_and(int8_t a, int8_t b);
static int8_t operacao_or(int8_t a, int8_t b);
static int8_t operacao_cmp(int8_t a, int8_t b);
static void executar_operacoes(int8_t operador, int8_t operador2, uint8_t controleUla, ResultadoUla *resultadoUla);

ResultadoUla executar(int8_t operador, int8_t operador2, uint8_t controleUla) {
    ResultadoUla resultadoUla;
    executar_operacoes(operador, operador2, controleUla, &resultadoUla);
    return resultadoUla;
}

static void executar_operacoes(int8_t operador, int8_t operador2, uint8_t controleUla, ResultadoUla *resultadoUla) {
    switch (controleUla) {
        case OP_ADD:
        case OP_ADDI:
        case OP_LW:
        case OP_SW:
            resultadoUla->resultado = operacao_add(operador, operador2);
            break;
        case OP_SUB:
            resultadoUla->resultado = operacao_sub(operador, operador2);
            break;
        case OP_AND:
            resultadoUla->resultado = operacao_and(operador, operador2);
            break;
        case OP_OR:
            resultadoUla->resultado = operacao_or(operador, operador2);
            break;
        case OP_BEQ:
            resultadoUla->resultado = operacao_cmp(operador, operador2);
            break;
        default:
            // Operação desconhecida
            resultadoUla->resultado = 0; 
            break;
    }
    resultadoUla->zero = (resultadoUla->resultado == 1) ? 1 : 0; // Sinaliza se o resultado é zero
}

static int8_t operacao_add(int8_t a, int8_t b) {
    return (a + b);
}

static int8_t operacao_sub(int8_t a, int8_t b) {
    return (a - b);
}

static int8_t operacao_and(int8_t a, int8_t b) {
    return ((uint8_t)a & (uint8_t)b);
}

static int8_t operacao_or(int8_t a, int8_t b) {
    return ((uint8_t)a | (uint8_t)b);
}

static int8_t operacao_cmp(int8_t a, int8_t b) {
    return (a == b) ? 1 : 0;
}
