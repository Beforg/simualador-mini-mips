# ifndef CONTROLE_H
# define CONTROLE_H

# include <stdint.h>
# include "types.h"

SinaisDeControle gerar_sinais_de_controle(uint8_t opcode, uint8_t funct, CPU *cpu);
EstadosControle proximo_estado(EstadosControle estado_atual, uint8_t opcode);

# endif
