// note que MAXKEYS guarda a quantidade máxima de chaves
// logo a quantidade de pointer to children = MAXKEYS + 1
#define MAXKEYS 4 
#define MINKEYS MAXKEYS/2   // quantidade mínima de chaves?
#define NIL (-1)

// é como se fosse o NIL/NULL mas para keys, indica vazio
#define NOKEY '@'    
       
#define NO 0
#define YES 1

// struct que define uma página
/*
typedef struct {
    short keycount; // quantidade de chaves em uma página
    char key[MAXKEYS]; // array de chaves
    short child[MAXKEYS+1]; // pointers para os RRNs dos descendentes
} BTPAGE;
*/

typedef struct {
       char key[12];
       long b_offset;
} PKEY;

typedef struct {
       short keycount;
       PKEY keys[MAXKEYS];       // array de chaves primárias
       short child[MAXKEYS+1]
} BTPAGE;

#define PAGESIZE sizeof(BTPAGE)

// a keyword extern é usada para declarar uma variável global
// cuja definição pode estar em algum outro arquivo
extern short root; // rrn of root page
extern FILE* btfd; // file descriptor of btree file
extern FILE* infd; // file descriptor of input file

/* prototypes */
btclose ();
btopen ();
btread (short rrn, BTPAGE *page_ptr);
btwrite (short rrn, BTPAGE *page_ptr);
create_root (char key, short left, short right);
short create_tree();
short getpage ();
short getroot ();
insert (short rrn, char key, short *promo_r_child, char *promo_key);
ins_in_page (char key,short r_child, BTPAGE *p_page);
pageinit (BTPAGE *p_page);
putroot(short root);
search_node (char key, BTPAGE *p_page, short *pos);
split(char key, short r_child, BTPAGE *p_oldpage, char *promo_key, short *promo_r_child, BTPAGE *p_newpage);
