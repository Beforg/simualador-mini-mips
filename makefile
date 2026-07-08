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
	src/utils/utils.c \
	src/core/assembler.c \
	src/utils/utils-asm.c \
	src/utils/ui.c

CFLAGS += $(shell pkg-config --cflags ncurses)
LDLIBS += $(shell pkg-config --libs ncurses)

all:
	$(CC) -o $(OUTPUT) $(SRC) $(CFLAGS) $(LDLIBS) -lncursesw
# Use make run para executar os arquivos 
run:
	./${OUTPUT} 

# Use o make clean para remover os .obj e.o executáveis.
clean:
	rm -rf *.obj *.o 