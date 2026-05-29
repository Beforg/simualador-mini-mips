# Documentacao do Pipeline Mini-MIPS (base monociclo)

## Objetivo
Este documento descreve as mudancas necessarias para evoluir o simulador atual (monociclo) para um pipeline de 5 estagios:
1) BI: busca de instrucao
2) DI: decodificacao + leitura de registradores
3) EX: execucao / calculo de endereco
4) MEM: acesso a memoria de dados
5) ER: escrita no banco de registradores

O foco aqui e apontar o que deve mudar no projeto, com destaque no impacto em [cpu.c](../cpu.c).

## Estado atual (monociclo)
- O fluxo de execucao esta todo concentrado em `executrar_ciclo`, em [cpu.c](../cpu.c).
- O controle e combinacional por opcode/funct em [controle.c](../controle.c) (sem FSM).
- A CPU tem memoria separada para instrucao e dados em [types.h](../types.h).
- O PC e atualizado no mesmo ciclo (incremento + possivel desvio) em `incrementar_pc` e `resolver_desvio`.
- As funcoes de decodificacao, ULA e memoria ja existem e podem ser reaproveitadas.

## Mudancas principais (pipeline)
### 1) Novos registradores de pipeline
Adicionar registradores de pipeline na struct `CPU` em [types.h](../types.h). Exemplo de blocos:
- `IF_ID`: instrucao buscada, `pc_plus1`.
- `ID_EX`: sinais EX/MEM/WB + campos decodificados (rs, rt, rd, imediato), valores lidos do banco, `pc_plus1`.
- `EX_MEM`: resultado da ULA, valor de rt para store, registrador destino, sinais MEM/WB.
- `MEM_WB`: dado da memoria, resultado da ULA, registrador destino, sinais WB.

Esses registradores passam a ser o estado central do pipeline e devem fazer parte do `CPU` (o back/stack continua funcionando automaticamente).

### 2) Separar sinais de controle por estagio
O `SinaisDeControle` atual representa todo o ciclo. No pipeline:
- gerar sinais na fase DI, por instrucao;
- armazenar apenas o necessario em `ID_EX`, `EX_MEM` e `MEM_WB`.

Sugestao: criar structs menores (EX/MEM/WB) ou reutilizar `SinaisDeControle` separando campos.

### 3) Reescrever a logica de `executrar_ciclo`
Manter o estilo (um fluxo unico de passos), mas trabalhar com os registradores de pipeline:
- ER: usa `MEM_WB` e escreve no banco de regs;
- MEM: usa `EX_MEM`, acessa memoria e grava `MEM_WB`;
- EX: usa `ID_EX`, executa ULA, grava `EX_MEM`;
- DI: usa `IF_ID`, decodifica e grava `ID_EX`;
- BI: busca instrucao (PC) e grava `IF_ID`.

A ordem inversa (ER->MEM->EX->DI->BI) evita sobrescrever dados do mesmo ciclo.

### 4) PC e controle de fluxo
Hoje o PC e incrementado e depois ajustado por `jump/branch`. No pipeline:
- calcular `pc_plus1` no BI;
- resolver branch/jump no EX;
- aplicar `flush` em `IF_ID` e `ID_EX` quando houver desvio tomado.

### 5) Hazards (minimo viavel)
- **Estrutural:** como a memoria de instrucao e dados sao separadas, BI e MEM podem ocorrer no mesmo ciclo sem conflito.
- **Controle (branch/jump):** se a decisao for em EX, aplicar `flush` em `IF_ID` e `ID_EX` quando necessario.
- **Dados (load-use):** pelo menos um stall de 1 ciclo quando `lw` em EX/MEM alimenta a instrucao seguinte.

### 6) Debug
O debug atual e por instrucao. No pipeline, o ideal e mostrar os registradores IF/ID, ID/EX, EX/MEM e MEM/WB em [debug.c](../debug.c).

### 7) Condicao de parada do programa
O `executar_programa_completo` hoje para quando a instrucao lida for zero. No pipeline, sera necessario:
- parar quando o PC sair da faixa **e** todos os registradores de pipeline estiverem vazios (pipeline drenado).

## Mudancas por arquivo (com destaque no pipeline)
### CPU e tipos
- [types.h](../types.h): adicionar structs dos registradores de pipeline e campos dentro de `CPU`.
- [cpu.c](../cpu.c): substituir fluxo monociclo por pipeline de 5 estagios usando os registradores.

### Controle
- [controle.c](../controle.c): manter geracao combinacional, mas separar sinais por estagio (EX/MEM/WB).
- [controle.h](../controle.h): ajustar structs ou novos prototipos se separar sinais.

### Memoria e banco de registradores
- [memoria.c](../memoria.c): em geral sem mudanca; apenas ajustar quem chama (agora via registradores de pipeline).
- [memoria.h](../memoria.h): sem mudanca estrutural.

### Debug
- [debug.c](../debug.c): adicionar impressao dos registradores de pipeline por estagio.
- [debug.h](../debug.h): novos prototipos se necessario.

## Sequencia recomendada de implementacao
1) Criar structs dos registradores de pipeline e adiciona-los ao `CPU`.
2) Separar sinais EX/MEM/WB a partir do controle combinacional atual.
3) Reescrever `executrar_ciclo` com a ordem ER->MEM->EX->DI->BI.
4) Ajustar a condicao de parada para drenar o pipeline.
5) Implementar `stall`/`flush` minimo (branch/jump e load-use).
6) Atualizar debug para mostrar o estado dos registradores de pipeline.

## Observacoes finais
- O pipeline muda a semantica de `executrar_ciclo`: cada ciclo avanca todas as instrucoes nos 5 estagios.
- A base monociclo facilita o inicio, pois o controle ja e combinacional e a memoria e separada.
- O maior impacto esta em organizar o estado por registradores de pipeline e inserir `stall/flush`.
