#ifndef ERROR
#include "error.h"
#endif

#ifndef TREE
#include "tree.h"
#endif

#define TYPE_NA 4
#define TYPE_J 1
#define TYPE_I 2
#define TYPE_R 3
#define TYPE_S 5

#define SUCCESS 1
#define HALT 0

#define NUMBER_OF_INSTRUCTIONS 20
#define MAX_NUMBER_OF_REGISTERS 32

typedef struct command scommand;

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

