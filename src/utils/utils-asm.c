#include "utils/utils.h"

void to_lower_str(char *str) {
    if (str == NULL) return;

    // Força retornar um caractere e unsigned char para evitar erros de conversão.
    for (int i = 0; str[i] != '\0'; i++) {
        str[i] = (char)tolower((unsigned char)str[i]);
    }
}

void trim_str(char *str) {
    if (str == NULL) return;

    int len = strlen(str);
    if (len == 0) return;

    // 1. Trim no final (Right Trim)
    // Começa do último caractere e volta enquanto encontrar espaços
    char *end = str + len - 1;
    while (end >= str && isspace((unsigned char)*end)) {
        end--;
    }
    // Coloca o terminador nulo logo após o último caractere válido
    *(end + 1) = '\0';

    // 2. Trim no início (Left Trim)
    char *start = str;
    while (*start && isspace((unsigned char)*start)) {
        start++;
    }

    // 3. Desloca a string para o início se houver espaços iniciais
    if (start != str) {
        memmove(str, start, strlen(start) + 1);
    }
}

