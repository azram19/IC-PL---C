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
    
    if(state -> registers[r1] > state -> registers[r2]){
        state -> PC = state -> PC + (memory(state, immidiate) << 2);
    }
    
    return 1;
}

int jmp(struct IMPSS* state, int body){
    int addrMask = 134217727;
    int address = body & addrMask;
    
    state -> PC = address;    

    return 1;
}

int jr(struct IMPSS* state, int body){
    int rMask = 31;
    int r1 = (body & (rMask << 21)) >> 21;

    state -> PC = state -> registers[r1];

    return 1;
}

int jal(struct IMPSS* state, int body){
    int addrMask = 134217727;
    int address = body & addrMask;
    
    state -> registers[31] = state -> PC + 4;
    state -> PC = address;

    return 1;
}

int addi(struct IMPSS* state, int address){
	int mask = 0x03E00000;
	int r1 = address & mask;
	r1 = r1 >> 21;
	
	mask = 0x001F0000;
	int r2 = address & mask;
	r2 = r2 >> 16;

	mask = 0x0000FFFF;
	int immediate = address & mask;

	state -> registers[r1] = state -> registers[r2] + immediate;
	state -> PC += 4;
	return 1;
}

int subi(struct IMPSS* state, int address){
	int mask = 0x03E00000;
	int r1 = address & mask;
	r1 = r1 >> 21;
	
	mask = 0x001F0000;
	int r2 = address & mask;
	r2 = r2 >> 16;

	mask = 0x0000FFFF;
	int immediate = address & mask;

	state -> registers[r1] = state -> registers[r2] - immediate;
	state -> PC += 4;
	return 1;
}

int main(int argc, char *argv[]){
	return 0;
}
