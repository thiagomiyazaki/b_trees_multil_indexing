#include "bt.h"

/*
Exemplo de Inserção
<input> 11111111111ABC1234
<output> Chave encontrada: 11111111111ABC1234 | pag: 0 | pos: 0
<output> <dados do registro>
<input> 33333333333CDE9874
<output> Chave não encontrada: 33333333333CDE9874
*/

int search_btree(short rrn, reg_index key, short *found_rrn, short *found_pos){
    if(rrn == -1)
        return NO;
    
    BTPAGE page;
    btread(rrn, &page);

    short pos, search_result;
    search_result = search_node(key, &page, &pos);

    if(search_result == YES){
        *found_rrn = rrn;
        *found_pos = pos;
        return YES;
    }
    else
        return search_btree(page.child[pos], key, found_rrn, found_pos);
}