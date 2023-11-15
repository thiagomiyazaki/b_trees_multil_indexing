#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include "bt.h"

#define MAX_SIZE_CLIENTE_CODE 12
#define MAX_SIZE_VEICULO_CODE 8
#define MAX_SIZE_CLIENTE_NOME 51
#define MAX_SIZE_VEICULO_NOME 51

#define SIZE_CLIENTE_CODE 11
#define SIZE_VEICULO_CODE 7
#define SIZE_CLIENTE_NOME 50
#define SIZE_VEICULO_NOME 50
#define SIZE_CHAVE_PRIMA 19


/*
// !--- DEFINIÇÃO DAS STRUCTS ---!

typedef struct chave_prima_t{
    char chave[SIZE_CHAVE_PRIMA];
} chave_prima;

typedef struct index_t{
    chave_prima chave;
    unsigned long int offset;
} reg_index;

typedef struct registro_ins_t{
    char cliente_code[MAX_SIZE_CLIENTE_CODE];
    char veiculo_code[MAX_SIZE_VEICULO_CODE];
    char cliente_nome[MAX_SIZE_CLIENTE_NOME];
    char veiculo_nome[MAX_SIZE_VEICULO_NOME];
    int no_dias;
    char size_registro;
} registro;


// !--- FUNÇÕES LOAD ---!
// funcao que le o arquivo e o carrega na memoria como
// uma array de structs
// pre: file struct     post: array of structs
registro *load_insere(FILE *f, int *qt_registros_insere);

// funcao que carrega as chaves primarias do arquivo de busca para a memória
chave_prima *load_busca(FILE *f, int *qt_registros_busca);




// !--- FUNÇÕES PRINT ---!
// funcao para printar os registros
// pre: pointer para o array, qt de registros (int)    post: nada
void print_registros(registro *array_de_registros, int qt_registros);




// !--- FUNÇÕES MENU ---!
// mostra o menu de opções
void show_main_menu();

// printa o menu de insercao
void insere_menu(registro *array_ins, int qt_registros);

// printa o menu de busca
void busca_menu(chave_prima *array_chaves, int qt_chaves);




// !--- FUNÇÕES AUXILIARES ---!
// funcao que compara duas structs de chave primaria para ver se sao iguais
// pre: duas chaves primarias para verificar se sao iguais  post: valor booleano
bool chave_prima_igual(chave_prima chave1, chave_prima chave2);

// calcula o tamanho de um registro e o armazena na variável size_registros
// pre: pointer para o array, qt de registros (int)    post: nada
void calculate_size(registro *array_de_registros, int qt_registros);

// retorna um tipo registro dependendo do indice fornecido
registro *get_registro_pelo_indice(reg_index item_escolhido, FILE *database_file);

// copia valores de uma struct para outra
void reg_index_copy(reg_index *dest, reg_index *source);


// !--- FUNÇÕES INIT ---!
// inicializa uma variável do tipo reg_index
reg_index init_index();

// inicializa uma variável do tipo chave_prima
chave_prima init_chave_prima();




// !--- FUNÇÕES DE INSERÇÃO/ESCRITA ---!
// insere um novo registro ao final da base de dados 
unsigned long int inserir_registro_base_de_dados(registro reg, FILE *f);

*/

int main(int argc, char **argv){
    /* !--- B-TREE VARS ---! */
    int promoted;           // boolean: tells if a promotion from below
    short root,             // rrn of root page
        promo_rrn;          // rrn promoted from below
    reg_index promo_key,    // key promoted from below
        key;                // next key to insert in tree
    /* !--- B-TREE VARS ---! */

    // verifica se foram passados arquivos via linha de comando, pois precisamos
    // de dois arquivos: insere.bin e remove.bin
    if (argc < 2){
        printf("É preciso passar dois arquivos pela linha de comando (insere.bin e remove.bin, nesta ordem). Saindo...");
        exit(1);
    }

    // !--- ABRE ARQUIVO COM REGISTROS PARA INSERÇÃO ---!
    FILE *insere_file;
    if ((insere_file = fopen(argv[1], "rb")) == NULL){
        fprintf(stderr, "Erro na abertura de arquivo.\n");
        exit(1);
    }

    // !--- ABRE ARQUIVO COM CHAVES PRIMÁRIAS PARA BUSCA ---!
    FILE *busca_file;
    if ((busca_file = fopen(argv[2], "rb")) == NULL){
        fprintf(stderr, "Erro na abertura de arquivo.\n");
        exit(1);
    }

    
    // !--- ARQUIVO BASE DE DADOS ---!
    // verifica se ele exista, caso não exista, ele é criado.
    FILE *output_file;
    if (access("database.bin", F_OK) == -1) {
        // ARQUIVO NÃO EXISTE: vamos criá-lo e posteriormente fechá-lo
        if ((output_file = fopen("database.bin", "wb")) == NULL){
            fprintf(stderr, "Erro na abertura de arquivo.\n");
            exit(1);
        }
        printf("** Arquivo database.bin não encontrado! Gerando arquivo **\n");
        fclose(output_file);
    }
    else
        // AVISA QUE O ARQUIVO EXISTE mas não o abre
        printf("** OK: o arquivo database.bin já existe **\n");
        getchar();

    
    // ARMAZENAM A QUANTIDADE DE ENTRADAS
    int qt_registros_insere = 0, qt_registros_busca = 0, qt_indices = 0;

    // DECLARAÇÃO E INICIALIZAÇÃO DAS ARRAYS DE REGISTROS, CHAVES PRIMÁRIAS PARA BUSCA E INDICES
    registro *array_de_registros_ins = load_insere(insere_file, &qt_registros_insere);
    chave_prima *array_de_chaves_busca = load_busca(busca_file, &qt_registros_busca);
    
    printf("Os arquivos insere.bin, busca_p.bin e indice.bin foram carregados na memória.\n");
    
    fclose(busca_file);
    fclose(insere_file);
    
    int opcao;
    while(1){
        // invoca funcao que printa o menu principal
        show_main_menu();

        // pega a opcao do usuario
        scanf("%d", &opcao);

        // pega o caractere '\n' oriundo da tecla ENTER
        fgetc(stdin);
        switch(opcao){
            case 1:
            // !---- INSERIR REGISTRO ----!
                bool exit_flag = false;
                

                // abre o arquivo de base de dados
                if ((output_file = fopen("database.bin", "a+b")) == NULL){
                    fprintf(stderr, "Erro na abertura de arquivo.\n");
                    exit(1);
                }

                // !--- PEGA OPÇÃO DO USUÁRIO ---!

                while(1){
                    bool insert_into_db = false;

                    // invoca a funcao que printa o menu de INSERCAO
                    insere_menu(array_de_registros_ins, qt_registros_insere);

                    // pega a opcao do usuario e verifica a validade
                    int opcao;
                    scanf("%d", &opcao);
                    fgetc(stdin);
                    printf("\n");
                    opcao--;

                    if(opcao < 0 || opcao > qt_registros_insere-1){
                        // define que vamos sair e que não vamos executar
                        // o bloco após o while
                        exit_flag = true;

                        if (opcao > qt_registros_insere-1){
                            printf("Opção inválida! Saindo...\n");
                            fclose(output_file);
                            // invocar função de ordenação do índice
                        }                   
                        else{
                            printf("Voltando ao menu principal...\n");
                            fclose(output_file);
                            // invocar função de ordenação do índice
                        }
                        break;
                    }

                // !--- GERA UM INDICE BASEADO EM UM REGISTRO ---!
                    reg_index index = init_index();

                // !--- INSERE O REGISTRO NA BASE DE DADOS ---!
                    // insere valores no buffer e insere na base de dados
                    // index.offset = inserir_registro_base_de_dados(array_de_registros_ins[opcao], output_file);
                    fseek(output_file, 0, SEEK_END);
                    index.offset = ftell(output_file);
                    strcat(index.chave.chave, array_de_registros_ins[opcao].cliente_code);
                    strcat(index.chave.chave, array_de_registros_ins[opcao].veiculo_code);


                // !--- DESCOMENTE PARA DEBUGAR ---!
                    printf("index offset: %ld\n", index.offset);
                    printf("index chave: %s\n\n", index.chave.chave);

                    if (btopen()){
                        // se o arquivo já existe entra aqui
                        root = getroot();
                        promoted = insert(root, index, &promo_rrn, &promo_key, &insert_into_db);
                        if (promoted){
                            printf("\n!--- Chave promovida: %s ---!\n\n", promo_key.chave.chave);
                            root = create_root(promo_key, root, promo_rrn);
                            printf("\n!--- Chave inserida com sucesso: %s ---!\n", index.chave.chave);
                        }
                    } 
                    else{
                        // se não existe vai ser criado
                        root = create_tree(index);
                        insert_into_db = true;
                    }

                    if(insert_into_db)
                        inserir_registro_base_de_dados(array_de_registros_ins[opcao], output_file);

                    btclose();
                }
                break;
            case 2:
                // !---- PESQUISA POR CHAVE PRIMARIA ----!
                while(1){
                    // abre o arquivo
                    output_file = fopen("database.bin", "rb");

                    bool found_flag = false;
                    registro *registro_encontrado;

                    // invoca a funcao que printa o menu de busca
                    busca_menu(array_de_chaves_busca, qt_registros_busca);

                    // pega a opcao do usuario e verifica a validade
                    int opcao;
                    scanf("%d", &opcao);
                    fgetc(stdin);
                    printf("\n");
                    opcao--;

                    if(opcao < 0 || opcao > qt_registros_insere-1){
                        if (opcao > qt_registros_insere-1)
                            printf("Opção inválida! Saindo...\n");
                        else
                            printf("Voltando ao menu principal...\n");
                        break;
                    }

                // !-- ARMAZENA A CHAVE ESCOLHIDA EM chave_escolhida ---!
                    chave_prima chave_escolhida = init_chave_prima();
                    strcpy(chave_escolhida.chave, array_de_chaves_busca[opcao].chave);

                /*
                // !-- BUSCA LINEAR PELA CHAVE ESCOLHIDA NO ARRAY DE INDICES ---!
                    // compara a chave de todos os indices com a chave escolhida
                    for(int i = 0; i < qt_indices; i++){
                        if(chave_prima_igual(chave_escolhida, array_indices[i].chave)){
                            registro_encontrado = get_registro_pelo_indice(array_indices[i], output_file);
                            printf("Código do Cliente: %s\n", registro_encontrado->cliente_code);
                            printf("Placa do veículo: %s\n", registro_encontrado->veiculo_code);
                            printf("Nome do cliente: %s\n", registro_encontrado->cliente_nome);
                            printf("Nome do veículo: %s\n", registro_encontrado->veiculo_nome);
                            printf("Quantidade de dias: %d\n\n", registro_encontrado->no_dias);
                            found_flag = true;
                        }
                    }
                */

                    if(!found_flag)
                        printf("*** Item não encontrado! ***\n\n");

                    // fechar o arquivo                 
                }
                break;
                
            case 3:
                printf("Fechando o programa...\n");
                exit(1);
                break;
            default:
                printf("Fechando o programa...\n");
                exit(1);
                break;
        }
    }
    return 0;
}
// END MAIN
