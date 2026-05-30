# Hazard Unit e Controle de Fluxo no Pipeline Atual

## Objetivo
Descrever como implementar tratamento de hazards (dados e controle) no pipeline de 5 estagios ja presente no projeto, sem depender de NOPs inseridos pelo loader.

## Premissas
- O pipeline segue a ordem ER -> MEM -> EX -> DI -> BI dentro de um ciclo.
- A memoria de instrucao e de dados sao separadas.
- O loader nao deve inserir NOPs nem ajustar enderecos de desvio.

## Mudancas no loader (io.c)
- Remover insercao automatica de NOPs.
- Nao ajustar enderecos de beq/j. O .mem deve vir limpo do assembler (offset relativo para beq e destino absoluto para j).

## Mudancas no fluxo do CPU (cpu.c)
### 1) Write-back antes de leitura
Executar o write-back antes de ler registradores no estagio DI, para reduzir RAW simples:

```c
valor_write_back = mux_write_back(cpu);
escrever_registrador(cpu, cpu->mem_wb.reg_destino, valor_write_back, sinais_de_controle);
valor_reg_a = ler_registrador(cpu, instrucao_decodificada.rs);
valor_reg_b = ler_registrador(cpu, instrucao_decodificada.rt);
```

### 2) Forwarding no EX (ja existe)
Manter o forwarding em EX via:
- mux_operador_forward (A)
- mux_operador_ou_imediato_forward (B)

Esses muxes comparam o destino em EX/MEM e MEM/WB com rs/rt do DI/EX.

### 3) Forwarding no ID para branch (ja existe)
Antes de travar DI/EX, aplicar forwarding para beq:

```c
if (sinais_de_controle.branch) {
    valor_reg_a = mux_forward_id(cpu, instrucao_decodificada.rs, valor_reg_a);
    valor_reg_b = mux_forward_id(cpu, instrucao_decodificada.rt, valor_reg_b);
}
```

### 4) Forwarding do dado de store (novo)
Se a instrucao em MEM for sw, o dado pode estar em MEM/WB. Resolver com um mux antes de escrever:

```c
int8_t store_data = cpu->ex_mem.b;
if (cpu->mem_wb.er.escrever_reg &&
    cpu->mem_wb.reg_destino == cpu->ex_mem.reg_destino_store) {
    store_data = (cpu->mem_wb.er.memoria_para_reg == 0)
        ? cpu->mem_wb.memoria_saida
        : cpu->mem_wb.ula_saida;
}
```

Observacao: e preciso carregar no EX/MEM o registrador origem do store.

### 5) Load-use hazard (stall de 1 ciclo)
Quando um lw esta em DI/EX e a proxima instrucao usa o mesmo registrador, inserir bubble:

```c
bool lw_use = (cpu->di_ex.opcode == OPCODE_LW) &&
              ((instrucao_decodificada.rs == cpu->di_ex.rt) ||
               (instrucao_decodificada.rt == cpu->di_ex.rt));

if (lw_use) {
    // congela PC e BI/DI
    // injeta NOP em DI/EX (zera sinais)
}
```

O stall deve:
- manter o PC atual
- manter BI/DI (nao avanca a instrucao)
- zerar DI/EX (bubble)
- permitir que EX/MEM e MEM/WB avancem normalmente

### 6) Flush em branch/jump tomado
Quando branch ou jump for tomado, limpar BI/DI e DI/EX no mesmo ciclo:

```c
if (branch_tomado || sinais_de_controle.jump) {
    cpu->bi_di = (BI_DI){0};
    cpu->di_ex = (DI_EX){0};
}
```

Isso elimina delay slots e remove a necessidade de NOPs manuais.

### 7) Acesso a memoria de dados apenas em lw
Evitar ler memoria de dados quando a instrucao nao for lw:

```c
valor_memoria = 0;
if (cpu->ex_mem.er.memoria_para_reg == 0 && cpu->ex_mem.er.escrever_reg == 1) {
    valor_memoria = ler_end_mem_dados(cpu, (uint8_t)cpu->ex_mem.ula_saida);
}
```

## Resumo dos hazards cobertos
- RAW em EX: forwarding EX/MEM e MEM/WB
- RAW em branch: forwarding no ID
- lw-use: stall de 1 ciclo
- store data: forwarding no MEM
- controle: flush em branch/jump

## Resultado esperado
- .mem limpo, sem NOPs manuais
- desvio correto sem delay slot
- load-use correto com stall
- sw recebe dado correto mesmo com dependencia

## Observacao final
Este fluxo reproduz o comportamento tipico de um pipeline simples com forwarding e hazard unit basica. Se o objetivo for fidelidade a um MIPS com delay slot, basta desativar o flush de branch e manter 1 instrucao no caminho errado.
