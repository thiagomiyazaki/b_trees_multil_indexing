#include <stdio.h>
#include "bt.h"
    // loop principal
    int main(){
        int promoted;   // boolean: tells if a promotion from below
        short root,     // rrn of root page
            promo_rrn;  // rrn promoted from below
        char promo_key, // key promoted from below
            key;        // next key to insert in tree
        if (btopen()){
            // se o arquivo já existe entra aqui
            root = getroot();
        } 
        else{
            // se não existe vai ser criado
            root = create_tree();
        }
        while ((key = getchar()) != 'q'){
            getc(stdin);
            promoted = insert(root, key, &promo_rrn, &promo_key);
            if (promoted)
                root = create_root(promo_key, root, promo_rrn);
        }
        btclose();
}
