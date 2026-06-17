# Análise: Stalls no Pipeline - Casos de Load-Use e Branch Tomado

## 📋 Resumo Executivo

Seu pipeline tem **2 tipos de hazards principais** que requerem stalls:

1. **Load-Use Hazard**: Quando um `lw` é seguido por instrução que usa esse registrador
2. **Branch Tomado**: Quando um `beq` ou `j` é tomado (requer flush de instruções incorretas)

## 🔍 Estado Atual do Pipeline

### Estrutura
- **5 estágios em 1 ciclo**: BI/DI → DI/EX → EX → EX/MEM → MEM/WB
- **Ordem de execução**: ER → MEM → EX → DI → BI
- **Forwarding já implementado** em:
  - ID/EX para branch (`mux_forward_id_ex`)
  - EX/MEM e MEM/WB para operandos da ULA
  - Store word

### Deficiências Encontradas

#### ❌ Sem Detecção de Load-Use
```c
// Em cpu.c, após decodificar a instrução NÃO há verificação:
// if (lw em DI/EX && próxima instrução usa ese registrador) → STALL
```

#### ⚠️ Flush de Branch Incompleto
```c
// O branch é detectado em desvio_condicional_tomado()
// Mas BI/DI e DI/EX NÃO são zerados quando tomado
```

---

## 🛠️ Caso 1: Load-Use Hazard (Stall Obrigatório)

### Cenário
```asm
lw  $t0, 0($sp)    # Ciclo N: está em DI/EX
add $t1, $t0, $t2  # Ciclo N: está em BI/DI (será decodificada)
```

### Timeline Sem Stall (❌ ERRADO)
```
Ciclo N:
  BI/DI: add      
  DI/EX: lw $t0       ← Valor será calculado/lido apenas em MEM/WB
  EX/MEM: <anterior>
  MEM/WB: <anterior2>

Ciclo N+1:
  BI/DI: <próxima>
  DI/EX: add $t0, ...  ← $t0 não está pronto! (ainda em MEM/WB ou saindo)
  EX/MEM: lw resultado (saindo de MEM)
  MEM/WB: <anterior>
```
❌ **Problema**: ADD tenta usar $t0 antes do LW completar

### Timeline Com Stall (✅ CORRETO)
```
Ciclo N:
  BI/DI: add      
  DI/EX: lw $t0
  EX/MEM: <anterior>
  MEM/WB: <anterior2>
  
  → Detecção: LW em DI/EX e rs/rt = $t0
  → AÇÃO: Inserir bubble

Ciclo N+1 (STALL):
  BI/DI: add      ← CONGELADO (não avança)
  DI/EX: NOP      ← BUBBLE injetado (sinais zerados)
  EX/MEM: lw resultado
  MEM/WB: <anterior>

Ciclo N+2:
  BI/DI: <próxima> ← Agora avança
  DI/EX: add $t0   ← $t0 disponível em MEM/WB para forward
  EX/MEM: NOP
  MEM/WB: lw resultado ✅ Pronto para read
```
✅ **Correto**: ADD usa $t0 com valor correto (forward ou já escrito)

---

## 🛠️ Caso 2: Branch Tomado (Flush Obrigatório)

### Cenário
```asm
beq $t0, $t1, label  # Ciclo N: em DI/EX, resultado = 1 (zero flag)
nop                  # Ciclo N: em BI/DI (será decodificada)
                     # NÃO deveria executar se branch for tomado!
```

### Timeline Sem Flush (❌ ERRADO)
```
Ciclo N:
  BI/DI: nop         ← Instrução INCORRETA buscada
  DI/EX: beq $t0, $t1
  EX/MEM: <anterior>
  MEM/WB: <anterior2>
  
  → Resultado: zero flag = 1 (branch tomado)
  → PC atualizado para label

Ciclo N+1:
  BI/DI: <label>     ← Correto agora, mas...
  DI/EX: nop         ← Ainda executa o nop (delay slot involuntário)
  EX/MEM: beq
  MEM/WB: <anterior>
```
❌ **Problema**: NOP e instrução de label executam no mesmo ciclo (sem delay slot!)

### Timeline Com Flush (✅ CORRETO)
```
Ciclo N:
  BI/DI: nop
  DI/EX: beq $t0, $t1
  EX/MEM: <anterior>
  MEM/WB: <anterior2>
  
  → Resultado: zero flag = 1 (branch tomado)
  → AÇÃO: Flush BI/DI e DI/EX

Ciclo N+1:
  BI/DI: <label>     ← Correto
  DI/EX: (nada)      ← Vazio (bubble)
  EX/MEM: beq
  MEM/WB: <anterior>
```
✅ **Correto**: Nop descartado, label executado corretamente

---

## 💻 Código de Implementação

### 1️⃣ Detecção de Load-Use (adicionar em cpu.c)

```c
// Após decodificar a instrução, mas ANTES de atualizar o pipeline
bool lw_use = (cpu->di_ex.opcode == OPCODE_LW) &&
              ((instrucao_decodificada.rs == cpu->di_ex.rt) ||
               (instrucao_decodificada.rt == cpu->di_ex.rt));

if (lw_use) {
    printf("mini-mips-hazard: Load-use detectado! Inserindo stall.\n");
    // Não atualizar PC
    // Não atualizar BI/DI
    // Injetar bubble em DI/EX
    cpu->di_ex.er = (ErSinais){0};
    cpu->di_ex.mem_sinais = (MemSinais){0};
    cpu->di_ex.ex_sinais = (ExSinais){0};
    return; // Sair sem atualizar o resto do pipeline
}
```

### 2️⃣ Flush em Branch Tomado (modificar em cpu.c)

```c
// Em executrar_ciclo(), após atualizar_pc()
if (desvio_condicional_tomado(cpu, resultadoUla) || sinais_de_controle.jump) {
    printf("mini-mips-info: Branch/Jump tomado! Fazendo flush.\n");
    
    // Flush: zerar BI/DI e DI/EX
    cpu->bi_di = (BI_DI){0};
    cpu->di_ex = (DI_EX){0};
    
    // EX/MEM e MEM/WB continuam normalmente
}
```

### 3️⃣ Modificação Estrutural (flags opcionais)

Se quiser rastrear state de stall, adicione em `types.h`:

```c
typedef struct {
    // ... campos existentes ...
    uint8_t em_stall;  // 1 se em stall por load-use, 0 caso contrário
} CPU;
```

---

## 📊 Tabela de Comparação

| Caso | Sem Tratamento | Com Stall/Flush | Ciclos | Status |
|------|---|---|---|---|
| Load-Use | ❌ Valor errado | ✅ Valor correto | +1 | Crítico |
| Branch Tomado | ⚠️ Delay slot involuntário | ✅ Sem delay | ±0 | Importante |
| RAW em EX | ❌ Possível erro | ✅ Forward resolve | 0 | Resolvido |
| Store Hazard | ⚠️ Pode ser incorreto | ✅ Forward resolve | 0 | Resolvido |

---

## 🎯 Plano de Implementação

### Fase 1: Load-Use (Crítica)
- [ ] Adicionar detecção em `executrar_ciclo()`
- [ ] Implementar mecanismo de stall
- [ ] Testar com programa `lw` + `add/sub/and/or`

### Fase 2: Branch Flush (Importante)
- [ ] Adicionar detecção de branch/jump tomado
- [ ] Implementar flush de BI/DI e DI/EX
- [ ] Testar com programa com desvios

### Fase 3: Validação
- [ ] Gerar teste específico para cada hazard
- [ ] Verificar que forwarding ainda funciona
- [ ] Validar sequências mistas (load-use + branch)

---

## 📝 Exemplos de Teste

### Teste 1: Load-Use
```asm
addi $t0, $zero, 5
sw   $t0, 0($sp)
lw   $t1, 0($sp)     ← $t1 será 5
add  $t2, $t1, $t1   ← Requer stall (ou programa errado)
```

### Teste 2: Branch Tomado
```asm
addi $t0, $zero, 5
beq  $t0, $t0, skip  ← Sempre tomado (zero = 1)
addi $t1, $zero, 1   ← Deve ser descartado
skip:
addi $t2, $zero, 2   ← Deve executar
```

### Teste 3: Combinado
```asm
lw   $t0, 0($sp)     ← Load
beq  $t0, $zero, label ← Branch usa $t0 (forwarding resolve)
add  $t1, $t0, $t0   ← Load-use detectado → STALL
```

---

## 🔗 Relacionamento com Código Existente

| Função | Propósito | Modificação |
|--------|-----------|---|
| `executrar_ciclo()` | Loop principal | Adicionar detecção de stall/flush |
| `desvio_condicional_tomado()` | Detecta branch tomado | ✅ Já funciona |
| `atualizar_pc()` | Atualiza PC | ✅ Já funciona |
| `mux_forward_id_ex()` | Forward para branch | ✅ Já funciona |
| `mux_operador_forward_di_ex()` | Forward em EX | ✅ Já funciona |

---

## ⚠️ Considerações Importantes

1. **Load-Use é crítico**: Sem stall, o ADD lerá valor errado
2. **Branch Flush opcional para compatibilidade**: 
   - Com flush: Sem delay slot (parecido com moderno MIPS)
   - Sem flush: Com delay slot (parecido com MIPS original)
3. **Forwarding reduz stalls**: Muitos RAWs já são resolvidos
4. **Order de execução**: ER → MEM → EX → DI → BI é importante!

---

## 📚 Referências Internas

- [hazards_pipeline.md](docs/hazards_pipeline.md) - Documentação original
- [cpu.h](cpu.h) - Estruturas de pipeline
- [types.h](types.h) - Definições de tipos
- [controle.c](controle.c) - Geração de sinais
