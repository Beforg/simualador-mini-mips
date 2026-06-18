#include "utils/utils-asm.h"
#include "core/assembler.h"

// Arquivos para leitura
FILE *input_file = NULL;    // Ponteiro para referência do arquivo de entrada.
FILE *output_file = NULL;   // Ponteiro para referencia do arquivo de saída.

// Variáveis para leitura
static char buf_leitura[100] = {0}; // Leitura de linha do arquivo.
static int linha_atual = 0;        // Contador das linhas

uint16_t instrucao_atual; // Usando unsigned int com 16 bits, para processar.
static int opcode_value = 0;
static int funct_value = 0;
static int imm_value = 0;


// Flags para usar no processar_asm()
bool tipo_r = false;
bool tipo_i = false;
bool tipo_j = false;


// Ativar Debug de assembler
bool debug_assembler = false;


// Definição das instruções válidas, em caso de dúvida, cheque o struct Instruções.
Instrucoes tabela_instrucoes[] = { 
    // Tipo R
    {ADD_LBL,'R',0x0,0x0},  //HASH 0
    {SUB_LBL,'R',0x0,0x2},  //HASH 1
    {AND_LBL,'R',0x0,0x4},  //HASH 2
    {OR_LBL,'R',0x0,0x5},   //HASH 3
    // Tipo I
    {ADDI_LBL,'I',0x4,0x0}, //HASH 4
    {LW_LBL,'I',0xB,0x0},   //HASH 5
    {SW_LBL,'I',0xF,0x0},   //HASH 6
    {BEQ_LBL,'I',0x8,0x0},  //HASH 7
    // Tipo J
    {J_LBL,'J',0x2,0x0}     //HASH 8
};

// Definindo Registradores Válidos e seus valores inteiros.
Registradores tabela_lbl_regs[] = {
    {R0_LBL,0},
    {R1_LBL,1},
    {R2_LBL,2},
    {R3_LBL,3},
    {R4_LBL,4},
    {R5_LBL,5},
    {R6_LBL,6},
    {R7_LBL,7}
};


static int get_reg_num(char *reg_str){
    // Percorre a tabela

    for(int i = 0; i < 8; i++){
        if(strcmp(reg_str,tabela_lbl_regs[i].lbl)==0){
            return tabela_lbl_regs[i].index;
        }
    }
    fprintf(stderr,"assembler-mini-mips-err-l:%d: Registrador (%s) não reconhecido na tabela de registradores.\n",linha_atual,reg_str);
    exit(EXIT_FAILURE);
    return -1;
}

static void msg_debug_assembler(char *msg){
    printf("mini-mips-assembler-err-l.%d:%s\n",linha_atual,msg);
}


void reset_controle_assembler(){
    // Reseta as variáveis de controle.
    instrucao_atual =
    opcode_value = 
    funct_value = 
    imm_value = 0;

    // Reseta as flags dos tipos:
    
    tipo_i = 
    tipo_j =
    tipo_r = false;
}

void fechar_arquivos(){
    // Verifica se os ponteiros dos arquivos estão abertos e fecha se necessário.
    if(input_file){
        fclose(input_file);
    }  

    if(output_file){
        fclose(output_file);
    }
    // Reseta os ponteiros por segurança.
    input_file = output_file = NULL;
}

void carregar_arq_asm(char *dirname){
    // Carregar arquivo;
    input_file = fopen(dirname,"r");
    if(!input_file){
        fprintf(stderr,"assembler-mini-mips: O diretório não existe ou não pôde ser processado para o arquivo de entrada.\n");
        return;
    }
}

// Função Processar_asm()
// 1 - Ler linha com fgets() ===> ENTRADA DE DADOS
// 2 - Parsear token usando strtok() ===> PARSER
// 3 - Identificar tipo de instrução, usando as flags tipo_r, tipo_i, tipo_j ===> DEFINIR FLAGS
// 4 - Selecionando as flags com o if, definimos opcode, funct ou imed, de acordo com cada tipo. ==> CODIFICAÇÃO
// 5 - Definindo instrução completa em bits ===> CODIFICAÇÃO
// 6 - No final de cada codificação, escreve no arquivo de saída.


void processar_asm(){

    // Loop para processar todo o arquivo.
    // Usa a função fgets() para ler o arquivo, utilizando a varíavel global definida no começo.
    // Esse primeiro loop irá servir apenas para decodificar

    if(input_file==NULL){msg_debug_assembler("O arquivo de input não está definido!"); return;}
    if(output_file==NULL){msg_debug_assembler("O arquivo de output não foi definido ou não foi criado!"); return;}

    while(fgets(buf_leitura,99,input_file) != NULL){

        reset_controle_assembler();
        // Realiza o trim_str na string, remove os espaços e caracteres nulos.
        trim_str(buf_leitura);
        to_lower_str(buf_leitura);

        // Verifica se a linha está vazia:
        if(buf_leitura[0] == '\0'){goto proxima_linha;};
        // 1) Parsear Opcode:
        char *tk = strtok(buf_leitura," ");
        if(debug_assembler){printf("TK-OP:%s\n",tk);}
        trim_str(tk);
        to_lower_str(tk);

        // Sai da função caso não tenha tokens.
        if(!tk){msg_debug_assembler("Erro de parser no processo de identificar operação.");goto final;}

        // Adicionando suporte a comentários de linha inteira.
        if(tk[0]  == '#' || ( tk[0] == '/' && tk[1] == '/')){goto proxima_linha;}

        // 2) Identificando tipo de instrução e armazenando nas flags bool criadas.
        
        // Verificando caso TIPO_R:
        if(
            strcmp(tk,tabela_instrucoes[ADD_HASH].nome) == 0 ||
            strcmp(tk,tabela_instrucoes[SUB_HASH].nome) == 0 ||
            strcmp(tk,tabela_instrucoes[AND_HASH].nome) == 0 ||
            strcmp(tk,tabela_instrucoes[OR_HASH].nome) == 0){
                // Definindo o op code com cada posição do vetor de char.
                // Estamos poupando tempo para não precisar repetir código.


                // Ligando a flag.
                tipo_r = true;
            }
        // Verificando se é tipo TIPO_I
        else if(
            strcmp(tk,tabela_instrucoes[ADDI_HASH].nome) == 0 ||
            strcmp(tk,tabela_instrucoes[LW_HASH].nome) == 0 || 
            strcmp(tk,tabela_instrucoes[SW_HASH].nome) == 0 ||
            strcmp(tk,tabela_instrucoes[BEQ_HASH].nome) == 0
        )
        {
            // O opcode é definido de forma diferente para cada operação, vamos fazer no switch com mais calma.
            tipo_i = true;
        }
        
        // O opcode é unico aqui, podemos fazer essa atribuiçãp
        else if(strcmp(tk,tabela_instrucoes[J_HASH].nome) == 0){
            tipo_j = true;
        }

        // Caso o tipo não seja válido, sai do algoritmo.
        else{msg_debug_assembler("Instrução encontrada não é válida."); goto final;} 

        if(tipo_r){
    
            if(strcmp(tk,tabela_instrucoes[ADD_HASH].nome) == 0){   opcode_value = 0b0000;  funct_value = 0b000;}
            if(strcmp(tk,tabela_instrucoes[SUB_HASH].nome) == 0){   opcode_value = 0b0000;  funct_value = 0b010;}
            if(strcmp(tk,tabela_instrucoes[AND_HASH].nome) == 0){   opcode_value = 0b0000;  funct_value = 0b100;}
            if(strcmp(tk,tabela_instrucoes[OR_HASH].nome) == 0){    opcode_value = 0b0000;  funct_value = 0b101;}

            // Remove os tokens da linha.
            char *rd_tk = strtok(NULL, ","); 
            char *rs_tk = strtok(NULL, ","); 
            char *rt_tk = strtok(NULL, ",");

            // Verifica se os tokens foram tokenizados corretamente:
            if(!rd_tk){msg_debug_assembler("Registrador destino (RD) fora de sintaxe."); goto final;}
            if(!rs_tk){msg_debug_assembler("Registrador Origem (RS) fora de sintaxe."); goto final;}
            if(!rt_tk){msg_debug_assembler("Registrador Alvo (RT) fora de sintaxe."); goto final;}

            if(debug_assembler){printf("Regs: RD(%s) - RS(%s) - RT(%s) \n",rd_tk,rs_tk,rt_tk);}

            // Realiza o trim na string e coloca como to_lower
            trim_str(rd_tk);
            trim_str(rs_tk);
            trim_str(rt_tk);
            to_lower_str(rd_tk);
            to_lower_str(rs_tk);
            to_lower_str(rt_tk);

            // Transforma o lbl do registrador em inteiro. Irá fechar repentinamente, se tiver um token não reconhecido.
            int rd_int = get_reg_num(rd_tk);
            int rs_int = get_reg_num(rs_tk);
            int rt_int = get_reg_num(rt_tk);

            if(debug_assembler){printf("Val regs: RD(%d) - RS(%d) - RT(%d) \n",rd_int,rs_int,rt_int);};

            // 1.Faz uma máscara de bits 
            // 0000|0000|0000|0000
            // 0000|0000|0000|0111 

            // 2. Faz a AND
            // 0000|0000|0000|0111

            // 3. Desloca para a posição 13
            // 0111|0000|0000|0000
            
            // Definindo o opcode

            instrucao_atual = (opcode_value << 12) | (rs_int << 9) | (rt_int << 6) | (rd_int << 3) | (funct_value);
           
            if(debug_assembler){printf("Instrução do tipo R: %016b\n",instrucao_atual);};
        }
        else if(tipo_j){
            // Aqui não precisamos de comparações, pois existe apenas uma opção para instrução do tipo j, que é o jump.
            // Setando o funct como 0b0000
            opcode_value = 0b0010; 
            funct_value = 0b000;

            char *imd_tk = strtok(NULL," ");

            if(!imd_tk){ printf("mini-mips-assembler-err-l.%d: imediato fora dos padrões de sintaxe!\n",linha_atual);}

            trim_str(imd_tk);
            to_lower_str(imd_tk);
            imm_value = atoi(imd_tk);
            if(imm_value>=128){msg_debug_assembler("Imediato acima do limite de [0,128["); goto final; }
            instrucao_atual = (opcode_value << 12) | (imm_value & 0x3F);

            if(debug_assembler)printf("mini-mips-assembler-debug: instrução tipo J:[%016b]\n",instrucao_atual);

        }

        else if(tipo_i)
        {
           
            // Identifica qual instrução é para definir o opcode
            if(strcmp(tk, tabela_instrucoes[ADDI_HASH].nome) == 0)      { opcode_value = 0b0100; }
            else if(strcmp(tk, tabela_instrucoes[LW_HASH].nome) == 0)  { opcode_value = 0b1011; }
            else if(strcmp(tk, tabela_instrucoes[SW_HASH].nome) == 0)  { opcode_value = 0b1111; }
            else if(strcmp(tk, tabela_instrucoes[BEQ_HASH].nome) == 0) { opcode_value = 0b1000; }

            char *rt_tk = NULL;
            char *rs_tk = NULL;
            char *imm_tk = NULL;

            if (opcode_value == 0b1011 || opcode_value == 0b1111) { 
                /// Verifica a sintaxe para lw e sw.
                rt_tk = strtok(NULL, ", ");
                imm_tk = strtok(NULL, " ("); // Pega o que está antes do '('
                rs_tk = strtok(NULL, " )");  // Pega o que está antes do ')'
            } 
            else {
                // Addi e beq
                rt_tk = strtok(NULL, ", ");
                rs_tk = strtok(NULL, ", ");
                imm_tk = strtok(NULL, ", ");
            }

            // Validação de segurança dos tokens
            if(!rt_tk || !rs_tk || !imm_tk) {
                msg_debug_assembler("Erro na sintaxe dos operandos (Tipo I).");
                goto final;
            }

            // Realizar sanitização das strings.
            trim_str(rs_tk); trim_str(rt_tk); trim_str(imm_tk);
            to_lower_str(rs_tk); to_lower_str(rt_tk);

            // Conversão para inteiros
            int rt_int = get_reg_num(rt_tk);
            int rs_int = get_reg_num(rs_tk);
            imm_value = atoi(imm_tk);

            // Validação do imediato (6 bits conforme imagem = máx 63)
            if(imm_value > 31 || imm_value < -32) {
                msg_debug_assembler("Imediato fora do limite de 6 bits (-32 a 31).");
                goto final;
            }
            if(debug_assembler){printf("Print do imediato:%d\n",imm_value);}
            instrucao_atual = (opcode_value << 12) | (rs_int << 9) | (rt_int << 6) |(imm_value & 0x3F);
                             
            if(debug_assembler) {
                printf("Instrução Tipo I: [%s] Op:%b RS:%d RT:%d Imm:%d -> Bin:[%016b]\n", 
                        tk, opcode_value, rs_int, rt_int, imm_value, instrucao_atual);
            }
        }

        fprintf(output_file,"%016b\n",instrucao_atual);
        // Vai para próxima linha
        proxima_linha:
        linha_atual++;
        reset_controle_assembler();

    }
    final:
    fechar_arquivos();
}

void exportar_asm_bin(char *dirname){
    output_file = fopen(dirname,"w");
}

