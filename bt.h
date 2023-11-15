#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

/* cada registro de índice/página tem 91 bytes */

/*
tive que inserir essa diretiva para desligar uma otimização
na hora da compilação que forçava a struct a ter 14 bytes
ao invés de 13 - a otimização ajuda o programa a acessar
mais rapidamente a memória, pois dependendo do endereço em que
o dado cai, é mais fácil e rápido ler na memória.
*/
#pragma pack(1)

// note que MAXKEYS guarda a quantidade máxima de chaves
// logo a quantidade de pointer to children = MAXKEYS + 1
#define MAXKEYS 3
#define MINKEYS 1  // quantidade mínima de chaves?
#define NIL (-1)

// é como se fosse o NIL/NULL mas para keys, indica vazio
#define NOKEY '@'    
       
#define NO 0
#define YES 1

#define MAX_SIZE_CLIENTE_CODE 12
#define MAX_SIZE_VEICULO_CODE 8
#define MAX_SIZE_CLIENTE_NOME 51
#define MAX_SIZE_VEICULO_NOME 51

#define SIZE_CLIENTE_CODE 11
#define SIZE_VEICULO_CODE 7
#define SIZE_CLIENTE_NOME 50
#define SIZE_VEICULO_NOME 50
#define SIZE_CHAVE_PRIMA 19

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

// struct que define uma página

typedef struct {
    short keycount; // quantidade de chaves em uma página
    reg_index key[MAXKEYS]; // array de indices
    short child[MAXKEYS+1]; // pointers para os RRNs dos descendentes
} BTPAGE;

/*
typedef struct {
       char key[12];
       long b_offset;
} PKEY;

typedef struct {
       short keycount;
       PKEY keys[MAXKEYS];       // array de chaves primárias
       short child[MAXKEYS+1]
} BTPAGE;
*/

#define PAGESIZE sizeof(BTPAGE)


// a keyword extern é usada para declarar uma variável global
// cuja definição pode estar em algum outro arquivo
extern short root; // rrn of root page
extern FILE* btfd; // file descriptor of btree file
extern FILE* infd; // file descriptor of input file

/* prototypes */
void btclose();
int btopen();
int btread(short rrn, BTPAGE *page_ptr);
int btwrite(short rrn, BTPAGE *page_ptr);
short create_root(reg_index key, short left, short right);
short create_tree(reg_index insert_me);
short getpage();
short getroot();
int insert(short rrn, reg_index key, short *promo_r_child, reg_index *promo_key, bool *insertion_flag);
void ins_in_page(reg_index key,short r_child, BTPAGE *p_page);
void pageinit(BTPAGE *p_page);
void putroot(short root);
int search_node(reg_index key, BTPAGE *p_page, short *pos);
void split(reg_index key, short r_child, BTPAGE *p_oldpage, reg_index *promo_key, short *promo_r_child, BTPAGE *p_newpage);
int search_btree(short rrn, reg_index key, short *found_rrn, short *found_pos);
reg_index get_index(short rrn, short pos, reg_index *picked_item);
void in_order_list(short rrn, FILE *db_file);

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

void print_single_reg(registro print_me);




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