#include<stdio.h>
#include<stdlib.h>

#define ERROR -1

#define ERR_UNDEFINED_OPERATION 1
#define ERR_ILLEGAL_MEMORY_ACCESS 2
#define ERR_WRONG_REGISTER 3
#define ERR_NOT_ENOUGH_MEMORY 4
#define ERR_REPEATED_LABEL 5
#define ERR_CANT_OPEN_FILE 6
#define ERR_WRONG_NUM_OF_ARGS 7

int error(int);
int error_file(int, char *);
