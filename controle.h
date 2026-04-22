# ifndef CONTROLE_H
# define CONTROLE_H

# include <stdint.h>
# include "types.h"

SinaisDeControle gerar_sinais_de_controle(uint8_t opcode, uint8_t funct);

# endif
