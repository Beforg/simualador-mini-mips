# Constantes para automatizar
OUTPUT= simulador.o

# Rode make all para compilar o arquivo, o output será simulador.o
all:	
	gcc -o ${OUTPUT} main.c estatisticas.c controle.c conversor.c cpu.c debug.c decodificador.c io.c memoria.c ula.c utils.c utils-asm.c assembler.c fsm.c -Wall

# Use make run para executar os arquivos 
run:
	./${OUTPUT} 

# Use o make clean para remover os .obj e.o executáveis.
clean:
	rm -rf *.obj *.o 