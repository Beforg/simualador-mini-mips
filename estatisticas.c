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

static void incrementar_ciclos(CPU *cpu, uint8_t opcode, uint8_t funct, EstadosControle estado_atual) {
 // Não contabiliza ciclos durante a fase de IF
    if (estado_atual != IF) {
    switch (opcode) {
        case OPCODE_R:
            switch (funct) {
                case FUNCT_ADD:
                    cpu->estatistica.total_ciclos_add += 1;
                    break;
                case FUNCT_SUB:
                    cpu->estatistica.total_ciclos_sub += 1;
                    break;
                case FUNCT_OR:
                    cpu->estatistica.total_ciclos_or += 1;
                    break;
                case FUNCT_AND:
                    cpu->estatistica.total_ciclos_and += 1;
                    break;
                default:
                    break;
            }
            break;
        case OPCODE_ADDI:
            cpu->estatistica.total_ciclos_addi += 1;
            break;
        case OPCODE_LW:
            cpu->estatistica.total_ciclos_lw += 1;
            break;
        case OPCODE_SW:
            cpu->estatistica.total_ciclos_sw += 1;
            break;
        case OPCODE_BEQ:
            cpu->estatistica.total_ciclos_beq += 1;
            break;
        case OPCODE_J:
            cpu->estatistica.total_ciclos_j += 1;
            break;
        default:
            break;
    }
}
}

void contabilizar_estatisticas(CPU* cpu, InstrucaoDecodificada instrucao_decodificada)
{
    incrementar_ciclos(cpu, instrucao_decodificada.opcode, instrucao_decodificada.funct);
    // Incrementa o total por tipo de instrução
        switch (cpu->mem_wb.opcode)
        {
        case OPCODE_R:
            switch (instrucao_decodificada.funct)
            {
            case FUNCT_ADD:
                cpu->estatistica.total_add_executadas++;
                cpu->estatistica.total_ciclos_add += 1;
                break;
            case FUNCT_SUB:
                cpu->estatistica.total_sub_executadas++;
                cpu->estatistica.total_ciclos_sub += 1;
                break;
            case FUNCT_OR:
                cpu->estatistica.total_or_executadas++;
                cpu->estatistica.total_ciclos_or += 1;
                break;
            case FUNCT_AND:
                cpu->estatistica.total_and_executadas++;
                cpu->estatistica.total_ciclos_and += 1;
                break;
            default:
                break;
            }
            break;
        case OPCODE_ADDI:
            cpu->estatistica.total_addi_executadas++;
            cpu->estatistica.total_ciclos_addi += 1;
            break;
        case OPCODE_LW:
            cpu->estatistica.total_lw_executadas++;
            cpu->estatistica.total_ciclos_lw += 1;
            break;
        case OPCODE_SW:
            cpu->estatistica.total_sw_executadas++;
            cpu->estatistica.total_ciclos_sw += 1;
            break;
        case OPCODE_BEQ:
            cpu->estatistica.total_beq_executadas++;
            cpu->estatistica.total_ciclos_beq += 1;
            break;
        case OPCODE_J:
            cpu->estatistica.total_j_executadas++;
            cpu->estatistica.total_ciclos_j += 1;
            break;
        default:
            break;
        }
    }

void contabilizar_todas_instrucoes(CPU *cpu, uint8_t opcode, uint8_t funct)
{
    switch (opcode)
    {
    case OPCODE_R:
        switch (funct)
        {
        case FUNCT_ADD:
            cpu->estatistica.total_add++;
            break;
        case FUNCT_SUB:
            cpu->estatistica.total_sub++;
            break;
        case FUNCT_OR:
            cpu->estatistica.total_or++;
            break;
        case FUNCT_AND:
            cpu->estatistica.total_and++;
            break;
        }
        break;
    case OPCODE_ADDI:
        cpu->estatistica.total_addi++;
        break;
    case OPCODE_LW:
        cpu->estatistica.total_lw++;
        break;
    case OPCODE_SW:
        cpu->estatistica.total_sw++;
        break;
    case OPCODE_BEQ:
        cpu->estatistica.total_beq++;
        break;
    case OPCODE_J:
        cpu->estatistica.total_j++;
        break;
    default:
        break;
    }
}

void reset_estatisticas(CPU *cpu)
{
    memset(&cpu->estatistica, 0, sizeof(Estatisticas));
}

void mostrar_estatisticas(CPU *cpu)
{
    cpu->estatistica.total_tipo_j = cpu->estatistica.total_j;
    cpu->estatistica.total_tipo_i = cpu->estatistica.total_addi + cpu->estatistica.total_lw + cpu->estatistica.total_sw + cpu->estatistica.total_beq;
    cpu->estatistica.total_tipo_r = cpu->estatistica.total_sub + cpu->estatistica.total_add + cpu->estatistica.total_or + cpu->estatistica.total_and;

    cpu->estatistica.total_ciclos = cpu->estatistica.total_ciclos_add +
                         cpu->estatistica.total_ciclos_sub +
                         cpu->estatistica.total_ciclos_or +
                         cpu->estatistica.total_ciclos_and +
                         cpu->estatistica.total_ciclos_addi +
                         cpu->estatistica.total_ciclos_lw +
                         cpu->estatistica.total_ciclos_sw +
                         cpu->estatistica.total_ciclos_beq +
                         cpu->estatistica.total_ciclos_j;

    cpu->estatistica.total_tipo_j_executadas = cpu->estatistica.total_j_executadas;
    cpu->estatistica.total_tipo_i_executadas = cpu->estatistica.total_addi_executadas + cpu->estatistica.total_lw_executadas + cpu->estatistica.total_sw_executadas + cpu->estatistica.total_beq_executadas;
    cpu->estatistica.total_tipo_r_executadas = cpu->estatistica.total_add_executadas + cpu->estatistica.total_sub_executadas + cpu->estatistica.total_or_executadas + cpu->estatistica.total_and_executadas;

    cpu->estatistica.total_instrucoes =
        cpu->estatistica.total_add + cpu->estatistica.total_sub + cpu->estatistica.total_or + cpu->estatistica.total_and +
        cpu->estatistica.total_addi + cpu->estatistica.total_lw + cpu->estatistica.total_sw + cpu->estatistica.total_beq + cpu->estatistica.total_j;
    cpu->estatistica.total_instrucoes_executadas = cpu->estatistica.total_add_executadas + cpu->estatistica.total_sub_executadas + cpu->estatistica.total_or_executadas + cpu->estatistica.total_and_executadas +
    cpu->estatistica.total_addi_executadas + cpu->estatistica.total_lw_executadas + cpu->estatistica.total_sw_executadas + cpu->estatistica.total_beq_executadas + cpu->estatistica.total_j_executadas;
    if (cpu->estatistica.total_instrucoes != 0)
    {
        // Calculando o total de cada instrução:
        // cpu->estatistica.total_ciclos_add = ADD_CICLOS * cpu->estatistica.total_add;
        // cpu->estatistica.total_ciclos_sub = SUB_CICLOS * cpu->estatistica.total_sub;
        // cpu->estatistica.total_ciclos_or = OR_CICLOS * cpu->estatistica.total_or;
        // cpu->estatistica.total_ciclos_and = AND_CICLOS * cpu->estatistica.total_and;
        // cpu->estatistica.total_ciclos_addi = ADDI_CICLOS * cpu->estatistica.total_addi;
        // cpu->estatistica.total_ciclos_lw = LW_CICLOS * cpu->estatistica.total_lw;
        // cpu->estatistica.total_ciclos_sw = SW_CICLOS * cpu->estatistica.total_sw;
        // cpu->estatistica.total_ciclos_beq = BEQ_CICLOS * cpu->estatistica.total_beq;
        // cpu->estatistica.total_ciclos_j = J_CICLOS * cpu->estatistica.total_j;

        // Somando os valores para o total de ciclos
        cpu->estatistica.total_ciclos = cpu->estatistica.total_ciclos_add +
                             cpu->estatistica.total_ciclos_sub +
                             cpu->estatistica.total_ciclos_or +
                             cpu->estatistica.total_ciclos_and +
                             cpu->estatistica.total_ciclos_addi +
                             cpu->estatistica.total_ciclos_lw +
                             cpu->estatistica.total_ciclos_sw +
                             cpu->estatistica.total_ciclos_beq +
                             cpu->estatistica.total_ciclos_j;

        // Calculando os percentuais
        cpu->estatistica.perc_i = ((float)cpu->estatistica.total_tipo_i / cpu->estatistica.total_instrucoes) * 100;
        cpu->estatistica.perc_j = ((float)cpu->estatistica.total_tipo_j / cpu->estatistica.total_instrucoes) * 100;
        cpu->estatistica.perc_r = ((float)cpu->estatistica.total_tipo_r / cpu->estatistica.total_instrucoes) * 100;
        cpu->estatistica.cpi = ((float)cpu->estatistica.total_ciclos / cpu->estatistica.total_instrucoes) * 100;
    }

    printf("======================ESTATÍSTICAS======================\n");
    printf("Total de instruções:[%d]\n", cpu->estatistica.total_instrucoes);
    printf("Instruções TIPO R:  [%d]\n", cpu->estatistica.total_tipo_r);
    printf("Instruções TIPO I:  [%d]\n", cpu->estatistica.total_tipo_i);
    printf("Instruções TIPO J:  [%d]\n", cpu->estatistica.total_tipo_j);
    printf("=================TOTAL DE INSTRUÇÕES====================\n");
    printf("TOTAL INSTR. ADD:   [%d]\n", cpu->estatistica.total_add);
    printf("TOTAL INSTR. SUB:   [%d]\n", cpu->estatistica.total_sub);
    printf("TOTAL INSTR. OR:    [%d]\n", cpu->estatistica.total_or);
    printf("TOTAL INSTR. AND:   [%d]\n", cpu->estatistica.total_and);
    printf("TOTAL INSTR. ADDI:  [%d]\n", cpu->estatistica.total_addi);
    printf("TOTAL INSTR. LW:    [%d]\n", cpu->estatistica.total_lw);
    printf("TOTAL INSTR. SW:    [%d]\n", cpu->estatistica.total_sw);
    printf("TOTAL INSTR. BEQ:   [%d]\n", cpu->estatistica.total_beq);
    printf("TOTAL INSTR. J:     [%d]\n", cpu->estatistica.total_j);
    printf("TOTAL INSTRUÇÕES:   [%d]\n", cpu->estatistica.total_instrucoes);
    printf("=============TOTAL DE INSTRUÇÕES EXECUTADAS================\n");
    printf("TOTAL INSTR. ADD EXECUTADAS:   [%d]\n", cpu->estatistica.total_add_executadas);
    printf("TOTAL INSTR. SUB EXECUTADAS:   [%d]\n", cpu->estatistica.total_sub_executadas);
    printf("TOTAL INSTR. OR EXECUTADAS:    [%d]\n", cpu->estatistica.total_or_executadas  );
    printf("TOTAL INSTR. AND EXECUTADAS:   [%d]\n", cpu->estatistica.total_and_executadas);
    printf("TOTAL INSTR. ADDI EXECUTADAS:  [%d]\n", cpu->estatistica.total_addi_executadas);
    printf("TOTAL INSTR. LW EXECUTADAS:    [%d]\n", cpu->estatistica.total_lw_executadas);
    printf("TOTAL INSTR. SW EXECUTADAS:    [%d]\n", cpu->estatistica.total_sw_executadas);
    printf("TOTAL INSTR. BEQ EXECUTADAS:   [%d]\n", cpu->estatistica.total_beq_executadas);
    printf("TOTAL INSTR. J EXECUTADAS:     [%d]\n", cpu->estatistica.total_j_executadas);
    printf("TOTAL INSTRUÇÕES EXECUTADAS:   [%d]\n", cpu->estatistica.total_instrucoes_executadas);
    printf("===============CICLOS TOTAIS DO PROGRAMA:================\n");
    printf("TOTAL CICLOS ADD:   [%d]\n", cpu->estatistica.total_ciclos_add);
    printf("TOTAL CICLOS SUB:   [%d]\n", cpu->estatistica.total_ciclos_sub);
    printf("TOTAL CICLOS OR:    [%d]\n", cpu->estatistica.total_ciclos_or);
    printf("TOTAL CICLOS AND:   [%d]\n", cpu->estatistica.total_ciclos_and);
    printf("TOTAL CICLOS ADDI:  [%d]\n", cpu->estatistica.total_ciclos_addi);
    printf("TOTAL CICLOS LW:    [%d]\n", cpu->estatistica.total_ciclos_lw);
    printf("TOTAL CICLOS SW:    [%d]\n", cpu->estatistica.total_ciclos_sw);
    printf("TOTAL CICLOS BEQ:   [%d]\n", cpu->estatistica.total_ciclos_beq);
    printf("TOTAL CICLOS J:     [%d]\n", cpu->estatistica.total_ciclos_j);
    printf("TOTAL CICLOS:       [%d]\n", cpu->estatistica.total_ciclos);
    printf("=========================== INSTRUÇÕES %%===================\n");
    printf("%% TIPO R:              [%.1f%%]\n", cpu->estatistica.perc_r);
    printf("%% TIPO I:              [%.1f%%]\n", cpu->estatistica.perc_i);
    printf("%% TIPO J:              [%.1f%%]\n", cpu->estatistica.perc_j);
    if (cpu->estatistica.total_instrucoes_executadas != 0) {

    printf("=========================== CICLOS %%=======================\n");
    printf("%% ADD  / TOT. CICLOS:       [%.1f%%]\n", ((float)cpu->estatistica.total_ciclos_add / cpu->estatistica.total_ciclos) *100);
    printf("%% SUB  / TOT. CICLOS:       [%.1f%%]\n", ((float)cpu->estatistica.total_ciclos_sub / cpu->estatistica.total_ciclos) *100);
    printf("%% OR   / TOT. CICLOS:       [%.1f%%]\n", ((float)cpu->estatistica.total_ciclos_or / cpu->estatistica.total_ciclos) *100);
    printf("%% AND  / TOT. CICLOS:       [%.1f%%]\n", ((float)cpu->estatistica.total_ciclos_and / cpu->estatistica.total_ciclos) *100);
    printf("%% ADDI / TOT. CICLOS:       [%.1f%%]\n", ((float)cpu->estatistica.total_ciclos_addi / cpu->estatistica.total_ciclos) *100);
    printf("%% LW   / TOT. CICLOS:       [%.1f%%]\n", ((float)cpu->estatistica.total_ciclos_lw / cpu->estatistica.total_ciclos) *100);
    printf("%% SW   / TOT. CICLOS:       [%.1f%%]\n", ((float)cpu->estatistica.total_ciclos_sw / cpu->estatistica.total_ciclos) *100);
    printf("%% BEQ  / TOT. CICLOS:       [%.1f%%]\n", ((float)cpu->estatistica.total_ciclos_beq / cpu->estatistica.total_ciclos) *100);
    printf("%% J    / TOT. CICLOS:       [%.1f%%]\n", ((float)cpu->estatistica.total_ciclos_j / cpu->estatistica.total_ciclos) *100);
    printf("CPI MÉDIA:              [%f]\n", ((float)cpu->estatistica.total_ciclos / cpu->estatistica.total_instrucoes_executadas));
    printf("========================================================\n");

    }
}