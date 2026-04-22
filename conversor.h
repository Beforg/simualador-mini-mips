#ifndef CONVERSOR_H
#define CONVERSOR_H

#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


//  Recebe a Instrução já decodificada pelo decodificador, realiza a conversão
// e retorna em formato de string para ser adicionado ao arquivo.

void converter_para_asm(InstrucaoDecodificada instr, char *retorno);

#endif
