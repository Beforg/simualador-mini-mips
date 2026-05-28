# Documentacao do Simulador Mini-MIPS 8 bits

## Visao geral
O simulador Mini-MIPS 8 bits executa um subconjunto de instrucoes estilo MIPS com memoria de instrucoes e dados. A interface e baseada em menu e permite carregar arquivos, executar programa inteiro, fazer step/back e visualizar o estado da CPU.

## Requisitos
- GCC
- Make
- Sistema Linux ou Windows (o menu usa limpeza de tela por sistema)

## Compilar e executar
```bash
make all
./simulador.o
```

Alternativamente:
```bash
make run
```

## Arquitetura simulada
- PC: 16 bits, enderecando 256 posicoes de memoria de instrucoes.
- Memoria de instrucoes: 256 palavras de 16 bits.
- Memoria de dados: 256 bytes (int8_t).
- Banco de registradores: 8 registradores (R0 a R7), cada um com 8 bits (int8_t).
- Encerramento automatico: o modo "run" para quando a instruacao corrente for 0.

## Conjunto de instrucoes suportado
- Tipo R: add, sub, and, or
- Tipo I: addi, lw, sw, beq
- Tipo J: j

## Formatos de arquivo
### .mem (instrucoes)
- Um binario de 16 bits por linha (exatamente 16 caracteres 0/1).
- Linhas vazias sao ignoradas.
- Carrega ate 256 instrucoes.

Exemplo:
```
0000000000000000
0100000100000011
0010000000000010
```

### .dat (dados)
- Um inteiro decimal com sinal por linha.
- Intervalo valido: -128 a 127.
- Carrega ate 256 valores.

Exemplo:
```
10
-3
127
```

### .asm (dump)
- Arquivo gerado a partir da memoria de instrucoes atual.
- Cada linha contem a instrucao convertida para assembly do Mini-MIPS.

## Menu do simulador
1. Carregar memoria de instrucoes (.mem)
2. Carregar memoria de dados (.dat)
3. Imprimir memorias (instrucoes e dados)
4. Imprimir banco de registradores
5. Imprimir todo o simulador (registradores e memorias)
6. Salvar .asm
7. Salvar .dat
8. Executa programa (run)
9. Executa uma instrucao (step)
10. Volta uma instrucao (back)
11. Ativar debug
12. Alterar visualizacao do debug (decimal, hexadecimal, binario)
0. Encerrar

## Debug e visualizacao
- O debug pode ser ativado e desativado pelo menu.
- A visualizacao permite alternar entre decimal, hexadecimal e binario.
- O estado atual da CPU (PC, instrucao e registradores) aparece no topo do menu.

## Observacoes e limitacoes
- O "back" usa uma pilha de estados completos da CPU, permitindo desfazer o ultimo step.
- Valores fora do intervalo de int8_t em .dat sao reportados como erro.
- Instrucoes invalidas geram sinais de controle zerados e nao alteram o estado de forma relevante.
