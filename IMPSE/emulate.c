#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include<stdint.h>

#define NUMBER_OF_REGISTERS 32
#define SIE_OF_MEMORY 65536 

struct IMPSS{
    int registers[NUMBER_OF_REGISTERS];
    char memory[SIZE_OF_MEMORY]; //I use `char` to get one byte memory cells
    int PC;
};


int addi(struct *IMPSS impssptr, int32_t body){ // change to 32 bits
	
}

int main(int argc, char *argv[]){

	return 0;//HEhe
}
