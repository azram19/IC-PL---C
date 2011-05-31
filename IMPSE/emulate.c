#include<string.h>
#include<stdlib.h>
#include<stdio.h>

#define NUMBER_OF_REGISTERS 32
#define SIE_OF_MEMORY 65536 

struct IMPSS{
    int registers[NUMBER_OF_REGISTERS];
    char memory[SIZE_OF_MEMORY]; //I use `char` to get one byte memory cells
    int PC;
};


int main(int argc, char *argv[]){

	return 0;
}
