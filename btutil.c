/* btutil.c
Contém funções auxiliares para o programa
*/

#include "bt.h"
#include "projeto3.h"
#include <string.h>

/*
A diretriz de criação de uma raiz depende do tipo de implementação feita, e pode variar.
Esta função cria uma página raiz com uma chave e dois child pointers.
*/
// cria a pagina raiz, escreve ela no arquivo, e atualiza o header
short create_root(reg_index key, short left, short right){
    BTPAGE page;
    short rrn;

    // pega a quantidade de páginas em btfd
    rrn = getpage();

    //printf("RRN from getpage(), inside create_root: %d\n", rrn);

    // inicializa a página com valores nulos/vazios
    pageinit(&page);

    // copia a chave primária passada por parâmetro para a chave primária da página criada pela função
    strcpy(page.key[0].chave.chave, key.chave.chave);
    page.key[0].offset = key.offset;
    
    page.child[0] = left;
    page.child[1] = right;
    page.keycount = 1;

    // escreve a página no RRN designado
    btwrite(rrn, &page);

    // atualiza o header com o RRN da nova raiz
    putroot(rrn);

    return rrn;
}

// inicializa uma pagina com valores nulos/vazios
void pageinit(BTPAGE *p_page){
    int j;
    for (j = 0; j < MAXKEYS; j++){
        memset(p_page->key[j].chave.chave, NOKEY, SIZE_CHAVE_PRIMA);
        p_page->key[j].offset = 0;
        p_page->child[j] = NIL;
    }
    p_page->child[MAXKEYS] = NIL;
}

/*
pre:
    - p_page: página onde a busca será feita
    - key: a chave que estamos buscando
    - pos: onde devemos armazenar o valor da posição,
    obs.: a pos encontrada pela função é uma unidade maior
    do que a posição em que a chave deveria ser inserida na
    array de chaves, e fornece o índice correto para o filho
    direito caso a chave tivesse sido inserida na array de chaves
    na posição correta.
*/
// busca na pagina para verificar se a chave existe (posicao encontrada) 
// ou nao existe (posicao que deveria estar)
int search_node(reg_index key, BTPAGE *p_page, short *pos){
    int i;

    /*
    STRCMP:
        - return 0: as duas strings são iguais
        - return X > 0: a primeira string é "maior"
        - return X < 0: a segunda string é "maior"
    */
    
    for (i = 0; i < p_page->keycount && strcmp(key.chave.chave, p_page->key[i].chave.chave) > 0; i++);
    *pos = i;

    if (*pos < p_page->keycount && chave_prima_igual(key.chave, p_page->key[*pos].chave)){
        // a chave foi encontrada
        return (YES);
    }
    else{
        // a chave não foi encontrada
        return (NO);
    }
}

/*
a validação para ver se ainda cabem mais chaves é feita antes de a função ser chamada.
logo ainda há pelo menos um espaço vazio.
*/
// faz insercao ordenada na pagina
void ins_in_page(reg_index key, short r_child, BTPAGE *p_page){
    int j;

    /*
    STRCMP:
        - return 0: as duas strings são iguais
        - return X > 0: a primeira string é "maior" que a segunda
        - return X < 0: a primeira string é "menor" que a segunda
    */
    
    for (j = p_page->keycount; (strcmp(key.chave.chave, p_page->key[j - 1].chave.chave) < 0) && j > 0; j--){
        // vai abrindo espaço para a chave e child ptrs novas
        strcpy(p_page->key[j].chave.chave, p_page->key[j - 1].chave.chave);
        p_page->key[j].offset = p_page->key[j - 1].offset;

        p_page->child[j + 1] = p_page->child[j];
    }

    // adiciona os valores nas posições corretas
    p_page->keycount++;
    strcpy(p_page->key[j].chave.chave, key.chave.chave);
    p_page->key[j].offset = key.offset;
    p_page->child[j + 1] = r_child;
}

void split(reg_index key, short r_child, BTPAGE *p_oldpage, reg_index *promo_key, short *promo_r_child, BTPAGE *p_newpage){
    printf("\n!--- Divisão de nó! ---!\n\n");
    int j;  
    reg_index workkeys[MAXKEYS + 1];
    short workchil[MAXKEYS + 2];

    // copia os valores da página antiga para as arrays recém declaradas
    for (j = 0; j < MAXKEYS; j++){
        reg_index_copy(&(workkeys[j]), &(p_oldpage->key[j]));
        workchil[j] = p_oldpage->child[j];
    }

    // copia um valor a mais para os child pointers
    workchil[j] = p_oldpage->child[j];

    // abre espaço nas arrays para adicionar key e r_child
    // e insere respeitando a ordenação das chaves

    /*
    STRCMP:
        - return 0: as duas strings são iguais
        - return X > 0: a primeira string é "maior" que a segunda
        - return X < 0: a primeira string é "menor" que a segunda
    */
    for (j = MAXKEYS; (strcmp(key.chave.chave, workkeys[j-1].chave.chave)) < 0 && j > 0; j--){
        reg_index_copy(&(workkeys[j]), &(workkeys[j - 1]));
        workchil[j + 1] = workchil[j];
    }

    // insere as chaves
    reg_index_copy(&(workkeys[j]), &(key));
    workchil[j + 1] = r_child;

    *promo_r_child = getpage();     
    pageinit(p_newpage);            // inicializa uma nova página

    // reg_index_copy(&(), &());

    // note que o loop itera até j atingir MINKEYS
    // !!! --- ACHO QUE DEVE SER j <= MINKEYS --- !!!
    for (j = 0; j < MINKEYS; j++){
        reg_index_copy(&(p_oldpage->key[j]), &(workkeys[j]));
        p_oldpage->child[j] = workchil[j];

        reg_index_copy(&(p_newpage->key[j]), &(workkeys[j + 2 + MINKEYS]));
        p_newpage->child[j] = workchil[j + 2 + MINKEYS];


        init_chave_prima(p_oldpage->key[j + 1 + MINKEYS].chave.chave);                                                         
        p_oldpage->child[j + 2 + MINKEYS] = NIL;
    }
    reg_index_copy(&(p_oldpage->key[MINKEYS]), &(workkeys[MINKEYS]));

    p_oldpage->child[MINKEYS] = workchil[MINKEYS];
    p_oldpage->child[MINKEYS + 1] = workchil[MINKEYS + 1];

    p_newpage->child[MINKEYS] = workchil[j + 2 + MINKEYS];
    
    p_newpage->keycount = MAXKEYS - MINKEYS - 1;
    p_oldpage->keycount = MINKEYS + 1;

    reg_index_copy(promo_key, &(workkeys[MINKEYS + 1]));

    // printf("!--- Será promovido: %c ---!\n", *promo_key);
}

reg_index get_index(short rrn, short pos, reg_index *picked_item){
    rewind(btfd);
    fseek(btfd, 2 + (rrn * sizeof(BTPAGE)) + 2 + (pos * sizeof(reg_index)), SEEK_SET);
    fread(picked_item, sizeof(reg_index), 1, btfd);
}

void in_order_list(short rrn, FILE *db_file){
    if(rrn == -1)
        return;
    BTPAGE page;
    btread(rrn, &page);
    in_order_list(page.child[0], db_file);

    reg_index buffer;
    for(int i = 0; i < page.keycount; i++){
        if(page.key[i].chave.chave[0] != '@'){
            buffer = init_index();
            reg_index_copy(&buffer, &(page.key[i]));
            registro *print_me = get_registro_pelo_indice(buffer, db_file);
            printf("\n<<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>>\n\n");
            print_single_reg(*print_me);
            printf("\n<<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>>\n\n");
        }

        if(page.child[i + 1] == -1)
            continue;
        else
            in_order_list(page.child[i + 1], db_file);
    }
}