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
        while (1){
            printf("\n<<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>>\n\n");
            printf("Digite a chave que deseja inserir, ou '0' para sair do p: ");

            key = getchar();
            getc(stdin);

            printf("\n");

            if(key == '0'){
                printf("\n<<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>>\n\n");
                break;
            }
                
            promoted = insert(root, key, &promo_rrn, &promo_key);
            if (promoted){
                printf("\n!--- Chave promovida: %c ---!\n\n", promo_key);
                root = create_root(promo_key, root, promo_rrn);
                printf("\n!--- Chave inserida com sucesso: %c ---!\n", key);
                //printf("\n<<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>><<>>\n\n");
            }
        }
        btclose();
}
