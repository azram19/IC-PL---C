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

/*
 * Returns 32 bits from memory.
*/
int memory(struct IMPSS* state, int address){
    int value = 0;
    int i;
    for(i = 0; i < 4; i++){
        value += (int)state -> memory[address];
        address++;
        value <<= 8;
    }
    return value;
}

int bge(struct IMPSS* state, int body){
    int rMask = 31;
    int immMask = 131071;
    
    int r1 = (body & (rMask << 21)) >> 21;
    int r2 = (body & (rMask << 16)) >> 16;
    int immidiate = body & immMask;
    
    if(r1 > r2){
        state -> PC = state -> PC + (memory(state, immidiate) << 2);
    }
    
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

int addi(struct IMPSS* impssptr, int body){ // change to 32 bits
	int mask = 1 << 26	
	int i = body & mask;
	printf("%d\n", i);

	return 1;
}

int main(int argc, char *argv[]){
	return 0;
}
