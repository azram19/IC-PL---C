typedef struct map_node snode;
typedef struct command scommand;

long long int get_hashcode(char * s);

snode * map_find(snode *, long long int);
int map_put(snode * root, char * key, int value);
int map_get(snode * root, char * key);
int freeTheTree(snode * root);

int op_char_to_int(char *, snode *);
int op_to_type(int);

int error(int);
int error_file(int, char *);

int reg_char_to_int(char *);

void Rtype(char *, scommand *);
void Itype(char *, scommand *);
void JorStype(char *, scommand *);

scommand * readToken(char *, snode *) ;

void assemblerPass1(snode *, scommand **, int);
int * assemblerPass2(snode *, scommand **, int *);

void binarywriter(char *, int *, int);

int signed_reduction(int);
int binary_converter(scommand *, int *, int *, int *, int *);
int replace_label(snode *, scommand *, int *);

scommand **createCommandArray(int);
snode * createLabelTree();
snode * createOpcodeTree();

int fileSize(FILE *);

void tokenise(FILE *, scommand **, int *, snode *);

void freeCommandArray(scommand **, int);

