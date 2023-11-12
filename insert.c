/* insert.c
A função insert() é utilizada para inserir uma chave em uma B-Tree.
É uma função recursiva que chama a si até que um nó folha seja alcançado,
somente então a inserção é feita.

Caso não haja espaço para a nova chave em um nó:
- função split() é chamada para dividir o nó
- a chave do meio é promovida e o rrn do novo nó
*/
#include "bt.h"

// insere as chaves na arvore B
/*
pre:
    - short rrn: o rrn da página atual que estamos pesquisando
    - char key: a chave que queremos inserir
    - short promo_r_child: ptr do filho direito de promo_key (chave promovida)
    - promo_key: a chave a ser promovida
*/
int insert(short rrn, char key, short *promo_r_child, char *promo_key){
    BTPAGE page,         // current page
        newpage;         // new page created if split occurs
    int found, promoted; // boolean values
    short pos,
        p_b_rrn;  // rrn promoted from below
    char p_b_key; // key promoted from below

    // se o rrn atual é igual a NIL, isto quer dizer que fomos além
    // do nó filho, e atingimos um nó inexistente
    if (rrn == NIL){
        *promo_key = key;
        *promo_r_child = NIL;
        return (YES);
    }

    // coloca o conteúdo da página cujo índice = rrn em 'page'
    btread(rrn, &page);
    // retorna 1 se achou a chave, 0 se não encontrou
    // além disso, encontra o índice certo para a var pos
    found = search_node(key, &page, &pos);

    if (found){
        printf("Error: attempt to insert duplicate key: %c \n\007", key);
        return (0);
    }

    promoted = insert(page.child[pos], key, &p_b_rrn, &p_b_key);
    if (!promoted){
        return (NO);
    }

    // se ainda há espaço para inserir
    if (page.keycount < MAXKEYS){
        ins_in_page(p_b_key, p_b_rrn, &page);
        btwrite(rrn, &page);
        return (NO);
    }
    else{ // não há espaço disponível dentro da página, tem q fazer split
        split(p_b_key, p_b_rrn, &page, promo_key, promo_r_child, &newpage);
        btwrite(rrn, &page);
        btwrite(*promo_r_child, &newpage);
        return (YES);
    }
}