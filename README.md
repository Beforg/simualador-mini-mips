# MINI-MIPS 8 Bits
Simulador didatico de um processador Mini-MIPS de 8 bits.

## Visao geral
Projeto da disciplina PI II, com implementacoes de arquitetura monociclo,
multiciclo e pipeline. Inclui assembler, simulador e utilitarios para testes.

## Recursos
- Simuladores: monociclo, multiciclo e pipeline.
- Assembler e conversor de arquivos .asm/.mem.
- Instrumentacao de depuracao e estatisticas.

## Estrutura do projeto
- codigo do simulador: arquivos .c/.h na raiz.
- docs/: documentacao tecnica.
- mem/: exemplos de programas e memorias.

## Como compilar
Use o makefile na raiz:

```bash
make
```

## Como executar
O binario gerado fica em ./simulador. Exemplos comuns:

```bash
./simulador
```

## Documentacao
Consulte:
- docs/documentacao_simulador.md
- docs/documentacao_simulador_multiciclo.md
- docs/documentacao_pipeline.md
- docs/assembler.md

## Status
- Monociclo  [X]
- Multiciclo [X]
- Pipeline   [X]

## Autor
Bruno Forgiarini