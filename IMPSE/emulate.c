#include<string.h>
#include<stdlib.h>
#include<stdio.h>

#define NUMBER_OF_REGISTERS 32
#define SIZE_OF_MEMORY 65536 

struct IMPSS{
    int registers[NUMBER_OF_REGISTERS];
    char memory[SIZE_OF_MEMORY]; //I use `char` to get one byte memory cells
    int PC;
};

int bge(struct IMPSS* state, int body){

    return 1;
}
int jmp(struct IMPSS* state, int body){

    return 1;
}
int jr(struct IMPSS* state, int body){

    return 1;
}
int jal(struct IMPSS* state, int body){

    return 1;
}

int main(int argc, char *argv[]){

	return 0;
}
