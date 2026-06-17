# Load-Use Hazard: Explicação Detalhada com Exemplos ASM

## 🎯 O Que é Load-Use Hazard?

**Load-use hazard** ocorre quando:
1. Uma instrução **LW (Load Word)** lê um valor da memória para um registrador
2. A **próxima instrução** tenta usar esse registrador antes do valor estar disponível

---

## 📚 Exemplo 1: Load-Use Simples

### Código Assembly
```asm
lw  $t0, 0($sp)      # Instrução 1: Carrega valor da memória em $t0
add $t1, $t0, $t2    # Instrução 2: Usa $t0 (PROBLEMA!)
```

### O Problema

**Sem stall (❌ ERRADO):**

```
Ciclo 1:
  BI/DI:  lw $t0, 0($sp)       ← Sendo buscada
  (Pipeline vazio)

Ciclo 2:
  BI/DI:  add $t1, $t0, $t2     ← Próxima instrução
  DI/EX:  lw $t0, 0($sp)        ← Em execução
  (Resultado do LW ainda não está pronto!)

Ciclo 3:
  BI/DI:  (próxima)
  DI/EX:  add $t1, $t0, $t2     ← Tenta usar $t0, mas ainda não tem valor!
  EX/MEM: lw $t0, 0($sp)        ← LW ainda em MEM
  
  ❌ ERRO: ADD usa $t0 com valor INCORRETO ou ZERO

Ciclo 4:
  BI/DI:  (próxima)
  DI/EX:  (próxima)
  EX/MEM: add $t1, $t0, $t2     ← Saindo de EX/MEM
  MEM/WB: lw $t0, 0($sp)        ← LW finalmente em WB (valor pronto!)
  
  Mas é TARDE! ADD já usou valor errado.
```

**Com stall (✅ CORRETO):**

```
Ciclo 1:
  BI/DI:  lw $t0, 0($sp)
  (Pipeline vazio)

Ciclo 2:
  BI/DI:  add $t1, $t0, $t2     ← Detecta load-use hazard!
  DI/EX:  lw $t0, 0($sp)        ← Em execução
  
  → Sistema detecta: "LW em DI/EX, ADD em BI/DI usa $t0"
  → AÇÃO: Inserir stall

Ciclo 2.5 (STALL):
  BI/DI:  add $t1, $t0, $t2     ← CONGELADO (não avança)
  DI/EX:  (NOP)                 ← BUBBLE injetado (sinais zerados)
  EX/MEM: lw $t0, 0($sp)        ← Avança normalmente
  
  PC não muda, BI/DI não muda

Ciclo 3:
  BI/DI:  add $t1, $t0, $t2     ← Agora entra em DI/EX
  DI/EX:  add $t1, $t0, $t2     ← 
  EX/MEM: (NOP)                 ← Bubble saindo
  MEM/WB: lw $t0, 0($sp)        ← ✅ VALOR PRONTO AQUI!
  
  ✅ CORRETO: ADD pode usar $t0 via forwarding de MEM/WB ou
             via valor já escrito no banco de registradores
```

---

## 📊 Timeline Comparativa (Ciclo por Ciclo)

### ❌ SEM STALL (Resultado Errado)

```
       Ciclo 1    Ciclo 2    Ciclo 3    Ciclo 4    Ciclo 5
BI/DI   lw $t0    add $t1    nop        nop        nop
DI/EX   (vazio)   lw $t0    add $t1    nop        nop
EX/MEM  (vazio)  (vazio)   lw $t0    add $t1    nop
MEM/WB  (vazio)  (vazio)  (vazio)   lw $t0    add $t1

Ciclo 3: ❌ ADD executa com $t0 = ??? (valor não chegou ainda)
Ciclo 4: ❌ ADD move para MEM/WB com valor ERRADO
Ciclo 5: ❌ ADD escreve resultado ERRADO no banco de regs
```

**Resultado final: $t1 tem valor INCORRETO** ❌

---

### ✅ COM STALL (Resultado Correto)

```
       Ciclo 1    Ciclo 2    Ciclo 3    Ciclo 4    Ciclo 5    Ciclo 6
BI/DI   lw $t0    add $t1    add $t1    nop        nop        nop
DI/EX   (vazio)   lw $t0     NOP        add $t1    nop        nop
EX/MEM  (vazio)  (vazio)    lw $t0     NOP        add $t1    nop
MEM/WB  (vazio)  (vazio)   (vazio)   lw $t0     NOP        add $t1

Ciclo 2: Detecta load-use em ADD (usa $t0)
Ciclo 3: STALL - PC congelado, BI/DI congelado, DI/EX = NOP
Ciclo 4: ✅ ADD agora em DI/EX, $t0 em MEM/WB (pronto via forward!)
Ciclo 5: ✅ ADD move para EX/MEM com valor CORRETO
Ciclo 6: ✅ ADD escreve resultado CORRETO em $t1
```

**Resultado final: $t1 tem valor CORRETO** ✅

---

## 🔍 Por Que Isso Acontece?

### Timeline de um LW

```
Ciclo N:     Ciclo N+1:    Ciclo N+2:    Ciclo N+3:
BI/DI: lw    DI/EX: lw     EX/MEM: lw    MEM/WB: lw
             (calcula)     (lê MEM)      (escreve reg)
                                         ← Só agora $t0 tem valor!
```

Se a próxima instrução tentar usar $t0 no **Ciclo N+1** ou **N+2**, o valor **não está pronto**.

### Forwarding Não Resolve

Por quê forwarding não funciona no load-use?

```
Ciclo N+2 (durante EX/MEM do LW):
  - EX/MEM tem: ula_saida = X (endereço calculado)
  - Forwarding em EX enviaria ula_saida para operandos
  - MAS: O valor de $t0 ainda está na memória de dados!
  
  → Forwarding só funciona com resultados da ULA
  → LW precisa ler da MEMÓRIA, não da ULA
  → Valor só fica pronto em MEM/WB (Ciclo N+3)
```

---

## 💡 Exemplos Práticos do Seu Projeto

### Exemplo 2: Load-Use com ADD

```asm
addi $t0, $zero, 100   # $t0 = 100
sw   $t0, 0($sp)       # Escreve 100 na memória
lw   $t1, 0($sp)       # Carrega valor (100) de volta em $t1
add  $t2, $t1, $t1     # $t2 = $t1 + $t1 = 200 (mas $t1 não está pronto!)
```

**Esperado:** $t2 = 200  
**Sem stall:** $t2 = valor incorreto ou 0  
**Com stall:** $t2 = 200 ✅

### Exemplo 3: Load-Use com SUB

```asm
lw  $s0, 0($sp)        # Carrega $s0
sub $s1, $s2, $s0      # Subtrai $s0 (não está pronto!)
```

**O que pode acontecer:**
- **Sem stall:** $s1 = $s2 - 0 (ou valor random)
- **Com stall:** $s1 = $s2 - (valor correto de $s0) ✅

### Exemplo 4: Load-Use com AND

```asm
lw  $a0, 4($sp)        # Carrega $a0 do endereço sp+4
and $a1, $a0, $a2      # Faz AND com $a0 (não está pronto!)
```

---

## ⚠️ Quando Detectar Load-Use?

### Em DI/EX:

Você verifica **no começo de cada ciclo**:

```c
// Se há LW em DI/EX E a próxima instrução usa o mesmo registrador:
bool lw_use = (cpu->di_ex.opcode == OPCODE_LW) &&
              ((instrucao_decodificada.rs == cpu->di_ex.rt) ||
               (instrucao_decodificada.rt == cpu->di_ex.rt));
```

**Tradução:**
- `cpu->di_ex.opcode == OPCODE_LW` → Há um LW em execução
- `instrucao_decodificada.rs == cpu->di_ex.rt` → Próxima instrução usa rs = registrador de destino do LW
- `instrucao_decodificada.rt == cpu->di_ex.rt` → OU próxima instrução usa rt = registrador de destino do LW

---

## 📋 Resumo: O Que Acontece em Cada Caso

| Instrução | Usa $t0? | Load-Use? | Ação |
|-----------|----------|-----------|------|
| `add $t1, $t0, $t2` | Sim (rs=$t0) | **SIM** | Stall! |
| `sub $t1, $t0, $t2` | Sim (rs=$t0) | **SIM** | Stall! |
| `and $t1, $t0, $t2` | Sim (rs=$t0) | **SIM** | Stall! |
| `or $t1, $t0, $t2` | Sim (rs=$t0) | **SIM** | Stall! |
| `addi $t1, $t0, 5` | Sim (rs=$t0) | **SIM** | Stall! |
| `sw $t0, 0($sp)` | Sim (rs=$t0) | **SIM** | Stall! |
| `beq $t0, $t1, label` | Sim (rs=$t0) | **SIM** | Stall! |
| `add $t1, $t2, $t3` | Não | **NÃO** | Normal |
| `lw $t1, 0($sp)` | Não | **NÃO** | Normal |
| `j label` | Não | **NÃO** | Normal |

---

## 🔧 Como Seu Código Detecta

```c
// Em cpu.c, função verificar_stall_lw()

// Verificar se:
// 1. Há um LW em DI/EX
// 2. A próxima instrução usa esse registrador (rt do LW)

bool lw_use = (cpu->di_ex.opcode == OPCODE_LW) &&
              ((instrucao_decodificada.rs == cpu->di_ex.rt) ||
               (instrucao_decodificada.rt == cpu->di_ex.rt));

if (lw_use) {
    // STALL: Congelar tudo
    cpu->di_ex.er = (ErSinais){0};          // Zera sinais de escrita
    cpu->di_ex.mem_sinais = (MemSinais){0}; // Zera sinais de memória
    cpu->di_ex.ex_sinais = (ExSinais){0};   // Zera sinais de EX
    return 1; // Sai de executrar_ciclo() sem atualizar nada
}
```

---

## 🎓 Visualização Final

```
CENÁRIO: lw $t0, 0($sp) seguido de add $t1, $t0, $t2

SEM STALL (ERRADO):
  Ciclo 1: Lê $sp+0 (início do LW)
  Ciclo 2: Calcula endereço (ainda é $sp+0)
  Ciclo 3: Lê da memória → $t0 = 42 (está aqui!)
  Ciclo 3: Também executa ADD → $t1 = ?? + ?? (ERRADO! $t0 não está disponível)
  
  Resultado: Cálculo errado

COM STALL (CORRETO):
  Ciclo 1: Lê $sp+0 (início do LW)
  Ciclo 2: Calcula endereço (detecta load-use, insere stall)
  Ciclo 3: Lê da memória → $t0 = 42 (pronto aqui!)
  Ciclo 3: ADD espera em BI/DI
  Ciclo 4: ADD executa → $t1 = 42 + ?? (CORRETO! $t0 já está disponível)
  
  Resultado: Cálculo correto
```

---

## 🎯 Conclusão

**Load-Use Hazard = "Usar um registrador que LW acaba de carregar"**

- **Problema:** LW precisa de 3 ciclos para trazer valor da memória
- **Solução:** Inserir 1 ciclo de stall (bubble) para aguardar
- **Resultado:** Próxima instrução executa com valor correto ✅
