/* btio.c Contém funções que controlam I/O: */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "bt.h"
//#include "fileio.h" acho que n precisa mais

FILE *btfd; // ptr para arquivo da B-Tree (mudei de file descriptor para FILE struct)

// abre o arquivo da arvore-B
int btopen(){
    if (access("btree.dat", F_OK) == -1)        
        return 0;
    btfd = fopen("btree.dat", "rb+");
    return 1;
    /*
    btfd = fopen("btree.dat", "w+"); // trocar por fopen()
    return (btfd != NULL ? 1 : 0);
    */
}

// fecha o arquivo da arvore-B
void btclose(){
    fclose(btfd);
}

// le o header do arquivo da arvore-B e recupera RRN da pagina raiz
short getroot(){
    printf("função get_root()\n");
    short root;
    fseek(btfd, 0, SEEK_SET);   // podia ser rewind() tb
    if (fread(&root, sizeof(short), 1, btfd) == 0){ // trocar por fread()
        printf("Error: Unable to get root. \007\n");
        exit(1);
    }
    return (root);
}

// atualiza o header do arquivo da arvore-B com RRN da nova pagina raiz
void putroot(short root){
    rewind(btfd),
    fwrite(&root, sizeof(short), 1, btfd);
}

// cria o arquivo da arvore-B e insere primeira chave na primeira pagina
short create_tree(){
    short header = -1;
    char key;
    // cria o arquivo, e se já existe, apaga tudo
    btfd = fopen("btree.dat", "wb");
    fwrite(&header, sizeof(short), 1, btfd);  // escreve a raiz
    fclose(btfd);                      // lembrar de escrever um header = -1 para a posição já existir e ser possível
    btopen();                         //                                            os deslocamentos em outros pontos
    scanf("%c", &key);    // pega a chave de stdin
    getc(stdin);
    return (create_root(key, NIL, NIL));
}

/*
CÁLCULO DA FUNÇÃO GETPAGE():
END_OFF = byte offset do último byte do arquivo
HEADER_SIZE = tamanho do header
PAGE_SIZE = tamanho da página

QTD_PAGINAS = (END_OFF - HEADER_SIZE) / PAGE_SIZE

O Cálculo vai nos fornecer o RRN da próxima página. E como isto é feito?
Primeiro calculamos quantas páginas temos no arquivo. Caso seja criada uma nova página
sabemos que o seu RRN é dado pela quantidade de páginas, pois os RRNs são 0-indexed.
*/

// descobre o proximo RRN disponivel
short getpage(){
    long addr;
    fseek(btfd, 0, SEEK_END);
    addr = ftell(btfd);
    return (short)((addr-2) / PAGESIZE);
}


/*
pre:
    - short rrn: indica o RRN da página a ser lida
    - BTPAGE *page_ptr: o conteúdo lido do arquivo será inserido no espaço apontado
    por *page_ptr
*/
// le uma pagina
int btread(short rrn, BTPAGE *page_ptr){
    long addr;

    // cálculo do byte offset do RRN da página que queremos acessar
    //      - coloca o cursor no 1o. byte da página de cujo rrn foi passado
    //      via parâmetro.
    addr = (long)rrn * (long)PAGESIZE + 2;          

    // posiciona o cursor no byte offset, preparando para a leitura da página    
    fseek(btfd, addr, SEEK_SET);

    // insere a página no espaço apontado por page_ptr e retorna a quantidade lida
    return fread(page_ptr, sizeof(BTPAGE), 1, btfd);
}

/*
pre:
    - short rrn: RRN da página a ser escrita
    - BTPAGE *page_ptr: o conteúdo apontado por page_ptr será escrito em btfd
*/
// escreve uma pagina
int btwrite(short rrn, BTPAGE *page_ptr){
    long addr;

    // cálculo do byte offset, usado para posicionar o cursor no lugar correto
    addr = (long)rrn * (long)PAGESIZE + 2;

    // posiciona o cursor
    fseek(btfd, addr, SEEK_SET);

    printf("-#-#-#-#-#-#-#-#-#-#-#-#\n");

    printf("RRN: %d\n", rrn);
    printf("sizeof: %ld\n", sizeof(BTPAGE));
    printf("keycount: %d\nkeys: ", page_ptr->keycount);
    for(int i = 0; i < MAXKEYS; i++)
        printf("%c ", page_ptr->key[i]);
    printf("\nchild ptrs: ");
    for(int i = 0; i < MAXKEYS; i++)
        printf("%d ", page_ptr->child[i]);
    printf("\n");

    printf("-#-#-#-#-#-#-#-#-#-#-#-#\n");

    // faz a escrita, de fato
    return fwrite(page_ptr, sizeof(BTPAGE), 1, btfd);
}