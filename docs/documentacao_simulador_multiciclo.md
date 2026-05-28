# Documentacao do Simulador Mini-MIPS 8 bits (Multiciclo)

## Visao geral
O simulador Mini-MIPS 8 bits (multiciclo) executa um subconjunto de instrucoes estilo MIPS usando uma maquina de estados para o controle. A memoria e unificada: um unico vetor guarda instrucoes e dados em faixas distintas de endereco. A interface e baseada em menu e permite carregar arquivos, executar programa inteiro, fazer step/back e visualizar o estado da CPU.

## Diferencas em relacao ao monociclo
- Controle por maquina de estados (FSM) em vez de logica combinacional direta.
- Memoria unificada para instrucoes e dados.
- Leitura de arquivo de instrucoes com secao de dados embutida.

## Memoria (unificada)
- Enderecos 0..127: memoria de instrucoes.
- Enderecos 128..255: memoria de dados.

## Maquina de estados (controle)
O controle segue uma sequencia de estados para buscar, decodificar, executar e escrever resultados. Isso permite dividir a execucao em ciclos menores (fetch, decode, execute/mem, write-back) e reutilizar a mesma memoria para instrucoes e dados.

## Formatos de arquivo
### .mem (instrucoes + dados)
O arquivo de instrucoes agora pode conter a secao de dados. A leitura segue este fluxo:
- Linhas vazias e comentarios iniciados por # sao ignorados.
- Linhas de instrucoes (16 bits em binario) sao carregadas em 0..127.
- Ao encontrar `.data`, o modo muda para dados e a leitura passa a preencher 128..255.

Formato de dados aceito na secao `.data`:
- Binario sequencial (cada linha e um binario de 16 bits; carrega a partir de 128).
- Ou formato `endereco:binario` para escrita direta em um endereco (128..255).

Exemplo:
```
0000000000000000
0100000100000011
0000000000000010
.data
10000000:0000000000001010
0000000000001111
```

### .dat (dados)
O arquivo `.dat` continua aceito para carregar apenas dados. Ele exige o cabecalho `.data` na primeira linha, seguido por valores decimais com sinal (-128..127), um por linha.

Exemplo:
```
.data
10
-3
127
```

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
13. Mostrar estatisticas
0. Encerrar

## Observacoes
- O modo run encerra quando o PC ultrapassa a faixa de instrucoes (0..127).
- A secao `.data` no arquivo `.mem` permite preparar dados junto com as instrucoes.
