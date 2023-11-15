#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

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
*/


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
