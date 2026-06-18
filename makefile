OUTPUT= simulador.o
CC= gcc
CFLAGS= -Wall -I./src
SRC= src/main.c \
	src/core/estatisticas.c \
	src/core/controle.c \
	src/core/conversor.c \
	src/core/cpu.c \
	src/utils/debug.c \
	src/core/decodificador.c \
	src/core/io.c \
	src/core/memoria.c \
	src/core/ula.c \
	src/utils/utils.c

all:
	${CC} -o ${OUTPUT} ${SRC} ${CFLAGS}

# Use make run para executar os arquivos 
run:
	./${OUTPUT} 

# Use o make clean para remover os .obj e.o executáveis.
clean:
	rm -rf *.obj *.o 