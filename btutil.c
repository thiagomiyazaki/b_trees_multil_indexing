/* btutil.c
Contém funções auxiliares para o programa
*/

#include "bt.h"

/*
A diretriz de criação de uma raiz depende do tipo de implementação feita, e pode variar.
Esta função cria uma página raiz com uma chave e dois child pointers.
*/
// cria a pagina raiz, escreve ela no arquivo, e atualiza o header
short create_root(char key, short left, short right){
    BTPAGE page;
    short rrn;

    // pega a quantidade de páginas em btfd
    rrn = getpage();

    printf("RRN from getpage(), inside create_root: %d\n", rrn);

    // inicializa a página com valores nulos/vazios
    pageinit(&page);

    page.key[0] = key;
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
        p_page->key[j] = NOKEY;
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
int search_node(char key, BTPAGE *p_page, short *pos){
    int i;

    for (i = 0; i < p_page->keycount && key > p_page->key[i]; i++);
    *pos = i;
    if (*pos < p_page->keycount && key == p_page->key[*pos]){
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
void ins_in_page(char key, short r_child, BTPAGE *p_page){
    int j;
    for (j = p_page->keycount; key < p_page->key[j - 1] && j > 0; j--){
        // vai abrindo espaço para a chave e child ptrs novas
        p_page->key[j] = p_page->key[j - 1];
        p_page->child[j + 1] = p_page->child[j];
    }
    // adiciona os valores nas posições corretas
    p_page->keycount++;
    p_page->key[j] = key;
    p_page->child[j + 1] = r_child;
}


/*
    key: chave a ser inerida
    r_child: filho direito da chave a ser inserida
    p_oldpage: a página antiga onde seria feita a inserção que causa o split
    promo_key: chave a ser promovida depois do split
    promo_r_child: o ptr p/filho direito da chave a ser promovida
    p_newpage: a nova página que será utilizada na redistribuição das chaves
*/
// faz a operacao de split de uma pagina
// somente para ordem par!! Adaptar para ordem impar!
void old_split(char key, short r_child, BTPAGE *p_oldpage, char *promo_key, short *promo_r_child, BTPAGE *p_newpage){
    int j;
    short mid;
    char workkeys[MAXKEYS + 1];
    short workchil[MAXKEYS + 2];

    // copia os valores da página antiga para as arrays recém declaradas
    for (j = 0; j < MAXKEYS; j++){
        workkeys[j] = p_oldpage->key[j];
        workchil[j] = p_oldpage->child[j];
    }

    // copia um valor a mais para os child pointers
    workchil[j] = p_oldpage->child[j];

    // abre espaço nas arrays para adicionar key e r_child
    // e insere respeitando a ordenação das chaves
    for (j = MAXKEYS; key < workkeys[j - 1] && j > 0; j--){
        workkeys[j] = workkeys[j - 1];
        workchil[j + 1] = workchil[j];
    }
    // insere as chaves
    workkeys[j] = key;
    workchil[j + 1] = r_child;


    *promo_r_child = getpage();     
    pageinit(p_newpage);            // inicializa uma nova página

    // note que o loop itera até j atingir MINKEYS
    // !!! --- ACHO QUE DEVE SER j <= MINKEYS --- !!!
    for (j = 0; j < MINKEYS; j++){
        p_oldpage->key[j] = workkeys[j];
        p_oldpage->child[j] = workchil[j];

        p_newpage->key[j] = workkeys[j + 1 + MINKEYS];
        p_newpage->child[j] = workchil[j + 1 + MINKEYS]; 
                                                         
        p_oldpage->key[j + MINKEYS] = NOKEY;
        p_oldpage->child[j + 1 + MINKEYS] = NIL;
    }

    p_oldpage->child[MINKEYS] = workchil[MINKEYS];

    p_newpage->child[MINKEYS] = workchil[j + 1 + MINKEYS];
    p_newpage->keycount = MAXKEYS - MINKEYS;
    p_oldpage->keycount = MINKEYS;
    *promo_key = workkeys[MINKEYS];
}

void split(char key, short r_child, BTPAGE *p_oldpage, char *promo_key, short *promo_r_child, BTPAGE *p_newpage){
    int j;  
    char workkeys[MAXKEYS + 1];
    short workchil[MAXKEYS + 2];

    // copia os valores da página antiga para as arrays recém declaradas
    for (j = 0; j < MAXKEYS; j++){
        workkeys[j] = p_oldpage->key[j];
        workchil[j] = p_oldpage->child[j];
    }

    // copia um valor a mais para os child pointers
    workchil[j] = p_oldpage->child[j];

    // abre espaço nas arrays para adicionar key e r_child
    // e insere respeitando a ordenação das chaves
    for (j = MAXKEYS; key < workkeys[j - 1] && j > 0; j--){
        workkeys[j] = workkeys[j - 1];
        workchil[j + 1] = workchil[j];
    }
    // insere as chaves
    workkeys[j] = key;
    workchil[j + 1] = r_child;

    *promo_r_child = getpage();     
    pageinit(p_newpage);            // inicializa uma nova página

    // note que o loop itera até j atingir MINKEYS
    // !!! --- ACHO QUE DEVE SER j <= MINKEYS --- !!!
    for (j = 0; j < MINKEYS; j++){
        p_oldpage->key[j] = workkeys[j];
        p_oldpage->child[j] = workchil[j];

        p_newpage->key[j] = workkeys[j + 2 + MINKEYS];
        p_newpage->child[j] = workchil[j + 2 + MINKEYS]; 
                                                         
        p_oldpage->key[j + 1 + MINKEYS] = NOKEY;
        p_oldpage->child[j + 2 + MINKEYS] = NIL;
    }
    p_oldpage->key[MINKEYS] = workkeys[MINKEYS];
    p_oldpage->child[MINKEYS] = workchil[MINKEYS];
    p_oldpage->child[MINKEYS + 1] = workchil[MINKEYS + 1];

    p_newpage->child[MINKEYS] = workchil[j + 2 + MINKEYS];
    
    p_newpage->keycount = MAXKEYS - MINKEYS - 1;
    p_oldpage->keycount = MINKEYS + 1;

    *promo_key = workkeys[MINKEYS + 1];

    printf("será promovido: %c\n", *promo_key);
}