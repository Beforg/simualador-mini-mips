#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <stdbool.h>
#include <string.h>
#include "io.h"
#include "types.h"
#include "cpu.h"
#include "debug.h"
#include "memoria.h"
#include "estatisticas.h"


static int alterar_exibicao_do_debug();
static void imprimir_menu(int debug_ativo, int modo_view);

#pragma region MACROS

// netoe1: Definição de macro para limpar tela.
#if _WIN32
#define limparTela() system("cls")
#elif __linux__
#define limparTela() system("clear")
#endif

#define esperar()                                         \
    printf("mini-mips: Aperte ENTER para continuar..\n"); \
    setbuf(stdin, NULL);                                  \
    getchar();

#pragma endregion MACROS

#pragma region CONSTANTES

// netoe1: Mensagem para dar print no menu, colocada em uma constante

char *MENU_BUF =
    "====================MENU-PRINCIPAL: MINI-MIPS-8BITS====================\n"
    "1.  Carregar memória de instruções (.mem) \n"
    "2.  Carregar memória de Dados (.dat) \n"
    "3.  Imprimir memórias (instruções e dados) \n"
    "4.  Imprimir banco de registradores \n"
    "5.  Imprimir todo o simulador (registradores e memórias) \n"
    "6.  Salvar .asm \n"
    "7.  Salvar .dat \n"
    "8.  Executa Programa (run) \n"
    "9.  Executa uma instrução (Step) \n"
    "10. Volta uma instrução (Back) \n"
    "11. Ativar Debug\n"
    "12. Alterar visualização do debug (Decimal, Hexadecimal, Binário)\n"
    "13. Mostrar Estatísticas\n"
    "0.  Encerrar \n"
    "========================================================================";

// netoe1: Enum para organizar as opções do menu.
// Usando typedef para não precisar digitar enum toda a hora.
typedef enum
{
    FECHAR = 0,
    CARREGAR_MEMORIA_INSTRUCOES,
    CARREGAR_MEMORIA_DADOS,
    IMPRIMIR_MEMORIAS,
    IMPRIMIR_BANCO_REG,
    IMPRIMIR_TODO_SIMULADOR,
    SALVAR_ASM,
    SALVAR_DAT,
    EXECUTAR_PROGRAMA,
    EXECUTA_INSTRUCAO,
    VOLTAR_INSTRUCAO,
    ATIVAR_DEBUG,
    ALTERAR_DEBUG,
    MOSTRAR_ESTATISTICAS

} EOpcoes;

#pragma endregion CONSTANTES

#pragma region VARIAVEIS_GLOBAIS

int opc = -1; // netoe1: variável de opção do menu principal

#pragma endregion VARIAVEIS_GLOBAIS

#pragma region PROTOTIPO_FUNCOES

void carregar_menu_principal();                 // Função que executa o menu principal, com os loops e selecionando as opções.
void receber_opcao_sanitizada(const char *msg); // Recebe a opção do usuário e sanitiza para que não haja erros. Armazena na variavel opc

#pragma endregion PROTOTIPO_FUNCOES

CPU cpu;
PilhaCPU pilha_back; // Pilha para armazenar os estados anteriores da CPU, permitindo a funcionalidade de "voltar instrução".
Estatisticas est;   // Estatisticas para armazenar os dados.

int main(void)
{

	 // Variável de controle do debug, por padrão, é false.
    setlocale(LC_ALL, ""); // netoe1: Suporte a acentos
    reset_estatisticas();
    carregar_menu_principal();
    
    return EXIT_SUCCESS; // EXIT_SUCCESS é um label definido para 0.
}

#pragma region IMPLEMENTACAO_FUNCOES

void carregar_menu_principal()
{
    //==========Receber o valor digitado pelo usuário:==========
    // Cria o loop e verifica se a opção é válida
    // Opc é uma variável global declarada nos blocos de constantes.
    // Executa um loop e usa um switch para verificar as opções.
    // OBS: opc tem o valor = -1 para entrar no loop
    bool debugAtivado = false;
    int opcao_visualizacao_debug = 1; // 0 = Decimal, 1 = Hexadecimal, 2 = Binário
    while (opc != FECHAR)
    {
        limparTela();
        // netoe1: Dá o print da constate criada na REGION
        estado_atual_cpu(&cpu, opcao_visualizacao_debug);
        imprimir_menu(debugAtivado, opcao_visualizacao_debug);
        //puts(MENU_BUF);
        receber_opcao_sanitizada("Selecione uma das opções:");
       
        switch (opc)
        {
        case CARREGAR_MEMORIA_INSTRUCOES:
            inicializar_cpu(&cpu);
            inicializar_pilha(&pilha_back);
            char nome_arquivo[128];
            puts("Digite o nome do arquivos de instruções .mem para carregar:");
            setbuf(stdin,NULL);
            fgets(nome_arquivo,127,stdin);
            nome_arquivo[strcspn(nome_arquivo,"\n")] = '\0';
            carregar_instrucoes_e_dados(nome_arquivo, &cpu);
            break;
        case CARREGAR_MEMORIA_DADOS:
           char aux[128];
            puts("Digite o nome do arquivo de dados .dat para carregar:");
            setbuf(stdin,NULL);
            fgets(aux,127,stdin);
            aux[strcspn(aux,"\n")] = '\0';
            carregar_dat(aux, &cpu);

            break;
        case IMPRIMIR_MEMORIAS:
            imprimirMemoria(&cpu,INSTRUCAO,opcao_visualizacao_debug);
            imprimirMemoria(&cpu,DADOS,opcao_visualizacao_debug);
            break;
        case IMPRIMIR_BANCO_REG:
            imprimirMemoria(&cpu,REGISTRADOR,opcao_visualizacao_debug);
            break;
        case IMPRIMIR_TODO_SIMULADOR:
            imprimirMemoria(&cpu,INSTRUCAO,opcao_visualizacao_debug);
            imprimirMemoria(&cpu,DADOS,opcao_visualizacao_debug);
            imprimirMemoria(&cpu,REGISTRADOR,opcao_visualizacao_debug);
            break;
        case SALVAR_ASM:{
            // puts("opc = SALVAR_ASM");
            char buf[128];
            puts("Digite o nome do arquivo.asm para dump:");
            setbuf(stdin,NULL);
            fgets(buf,127,stdin);
            buf[strcspn(buf,"\n")] = '\0';
            salvar_asm(buf, &cpu);
            
            break;
        }
        case SALVAR_DAT:{
           // puts("opc = SALVAR_DAT");
            char buf[128];
            puts("Digite o nome do arquivo.dat para dump:");
            setbuf(stdin,NULL);
            fgets(buf,127,stdin);
            buf[strcspn(buf,"\n")] = '\0';
            salvar_dat(buf, &cpu);

            break;
        }
        case EXECUTAR_PROGRAMA:
            executar_cpu(&cpu);
            break;
        case EXECUTA_INSTRUCAO:
            avancar_cpu(&cpu, &pilha_back, opcao_visualizacao_debug);
            break;
        case VOLTAR_INSTRUCAO:
            voltar_cpu(&cpu, &pilha_back);
            break;
        case ATIVAR_DEBUG:
            debugAtivado = !debugAtivado; // Alterna o estado do debug
            set_debug(debugAtivado); // Alterna o estado do debug
            printf("mini-mips-info: Debug %s!\n", debugAtivado ? "ativado" : "desativado");
            break;
        case ALTERAR_DEBUG:
            opcao_visualizacao_debug = alterar_exibicao_do_debug();
            printf("mini-mips-info: Visualização do debug alterada para %s.\n", opcao_visualizacao_debug == 0 ? "Decimal" : opcao_visualizacao_debug == 1 ? "Hexadecimal" : "Binário");
            break;
        case MOSTRAR_ESTATISTICAS:
            int sub_aux = 0;
            est:
            limparTela();
            printf("====================ESTATÍSTICAS====================\n");
            printf("1. Mostrar Estatísticas\n");
            printf("2. Resetar\n");
            printf("0. Voltar\n");

            scanf("%d",&sub_aux);

            if(sub_aux == 1){
                mostrar_estatisticas();
            }
            else if(sub_aux == 2){
    
                reset_estatisticas();
                puts("mini-mips: as estatísticas foram resetadas com sucesso!");
            }
            else if(sub_aux == 0){
                puts("mini-mips: Você escolheu voltar!");
            }
            else{
                puts("mini-mips: opção inválida!");
                goto est;
            }


            break;
        case FECHAR:
            puts("mini-mips-info: Encerrando programa!");
            break;
        default:
            puts("mini-mips-err: opção inválida não reconhecida.");
            break;    
        }
        
        esperar(); // Usa o macro de esperar
    }
}

void receber_opcao_sanitizada(const char *msg)
{
    // Função feita com o GEMINI.
    int resultado;

    while (1)
    { // Loop infinito que só quebra (break) quando o dado for válido
        printf("%s", msg);

        resultado = scanf("%d", &opc); // Lendo direto na global 'opc'

        if (resultado == 1)
        {
            // Sucesso! Leu um número.
            // Agora limpamos o '\n' (Enter) que sobrou no buffer.
            while (getchar() != '\n')
                ;
            break;
        }
        else
        {
            // Erro! O usuário digitou uma letra ou símbolo.
            puts("mini-mips-err: O menu aceita apenas números.");

            // LIMPEZA DO BUFFER: remove as letras que travaram o scanf
            while (getchar() != '\n')
                ;
        }
    }
}

static int alterar_exibicao_do_debug() {
    // Função para alterar a exibição do debug entre Decimal, Hexadecimal e Binário.
    // Você pode implementar um menu ou ciclo para escolher a opção desejada.
    // Por exemplo:
    printf("Selecione o formato de exibição do debug:\n");
    printf("0. Decimal\n");
    printf("1. Hexadecimal\n");
    printf("2. Binário\n");
    while (1) {
        printf("Digite a opção (0-2): ");
        int escolha;
        if (scanf("%d", &escolha) != 1 || escolha < 0 || escolha > 2) {
            printf("Opção inválida. Tente novamente.\n");
            while (getchar() != '\n'); // Limpa o buffer
        } else {
            return escolha; // Retorna a opção escolhida
        }
    }
}

static void imprimir_menu(int debug_ativo, int modo_view) {
    const char *status_dbg = debug_ativo ? "ON" : "OFF";
   // const char *modos[] = {"Decimal", "Hexadecimal", "Binario"};

    printf("\n+----------------------------------------------------------------------+");
    printf("\n|                MENU PRINCIPAL: MINI-MIPS-8BITS                       |");
    printf("\n+--------------+-------------------------------------------------------+");
    printf("\n| ARQUIVOS     | 1. Carregar Instrucoes (.mem)                         |");
    printf("\n|              | 2. Carregar Dados (.dat)                              |");
    printf("\n|              | 6. Salvar Assembly (.asm)                             |");
    printf("\n|              | 7. Salvar Dados (.dat)                                |");
    printf("\n+--------------+-------------------------------------------------------+");
    printf("\n| MONITOR      | 3. Ver Memorias (Instr/Dados)                         |");
    printf("\n|              | 4. Ver Banco de Registradores                         |");
    printf("\n|              | 5. Imprimir Todo o Simulador                          |");
    printf("\n+--------------+-------------------------------------------------------+");
    printf("\n| EXECUCAO     | 8. Run (Completo)                                     |");
    printf("\n|              | 9. Step (Proxima Instrucao)                           |");
    printf("\n|              | 10. Back (Voltar Instrucao)                           |");
    printf("\n+--------------+-------------------------------------------------------+");
    printf("\n| SISTEMA      | 11. Debug: [%-3s]                                      |", status_dbg);
    printf("\n|              | 12. Alterar Visualizacao (Hex, Bin, Dec)              |");
    printf("\n|              | 13. Estatísticas                                      |");
    printf("\n|              | 0.  Sair                                              |");
    printf("\n+--------------+-------------------------------------------------------+\n");
}

#pragma endregion IMPLEMENTACAO_FUNCOES