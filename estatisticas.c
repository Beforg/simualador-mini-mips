#include "estatisticas.h"
#include <stdio.h>


Estatisticas dados = {0,0,0,0};

void reset_estatisticas(){
    dados.total_instrucoes = 
    dados.total_tipo_i = 
    dados.total_tipo_j = 
    dados.total_tipo_r = 0;

    dados.perc_i =
    dados.perc_j =
    dados.perc_r =
    0.0;
}

void mostrar_estatisticas(){
    if(dados.total_instrucoes != 0){
        dados.perc_i = ((float)dados.total_tipo_i /dados.total_instrucoes) * 100;
        dados.perc_j = ((float)dados.total_tipo_j /dados.total_instrucoes) * 100;
        dados.perc_r = ((float)dados.total_tipo_r /dados.total_instrucoes) * 100;
    }
    

    printf("======================ESTATÍSTICAS======================\n");
    printf("Total de instruções:[%d]\n",dados.total_instrucoes);
    printf("Instruções TIPO R:[%d]\n",dados.total_tipo_r);
    printf("Instruções TIPO I:[%d]\n",dados.total_tipo_i);
    printf("Instruções TIPO J:[%d]\n",dados.total_tipo_j);
    printf("%% TIPO R:[%.1f%%]\n",dados.perc_r);
    printf("%% TIPO I:[%.1f%%]\n",dados.perc_i);
    printf("%% TIPO J:[%.1f%%]\n",dados.perc_j);
    printf("========================================================\n");
}

