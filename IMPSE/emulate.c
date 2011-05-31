#include<string.h>
#include<stdlib.h>
#include<stdio.h>

#define NUMBER_OF_REGISTERS 32
#define SIZE_OF_MEMORY 65536
#define NUMBER_OF_INSTRUCTIONS 18 
#define SUCCESS 1
#define HALT 0
#define ERROR -1
#define ERR_UNDEFINED_OPERATION 1
#define ERR_ILLEGAL_MEMORY_ACCESS 2
#define ERR_WRONG_REGISTER 3



struct IMPSS{
    int registers[NUMBER_OF_REGISTERS];
    char memory[SIZE_OF_MEMORY]; //I use `char` to get one byte memory cells
    int PC;
};

int error(int error_code){
    
    switch(error_code){
        case ERR_UNDEFINED_OPERATION: {
                fprintf(stderr, "Someone lied to you, we don't have it in our offer. Program terminated.");    
                break;
            }
        case ERR_ILLEGAL_MEMORY_ACCESS: {
                fprintf(stderr, "You've just tried to rape memory, we don't like our memory being raped. Don't ever do it again. Program terminated.");
                break;
            }
        case ERR_WRONG_REGISTER: {
                fprintf(stderr, "404; register doesn't exist. Program terminated.");    
                break;
            }         
    }
    
    exit(EXIT_FAILURE);
}



/*
 * Returns 32 bits from memory.
 */
int memory(struct IMPSS* state, int address){
    if(address < 0 || address >= SIZE_OF_MEMORY){
        error(ERR_ILLEGAL_MEMORY_ACCESS);
    }

    int value = 0;
    int i;
    for(i = 0; i < 4; i++){
        value += (int)state -> memory[address];
        address++;
        value <<= 8;
    }
    return value;
}

/*
 * Returns 32 bits register.
 */
int get_register(struct IMPSS* state, int reg_num){
    if(reg_num < 0 || reg_num >= NUMBER_OF_REGISTERS){
        error(ERR_WRONG_REGISTER);
    }
    
    return state -> registers[reg_num];
}

/*
 * Validates opcode.
 */
int op_code(int op){
    if(op < 0 || op >= NUMBER_OF_INSTRUCTIONS){
        error(ERR_ILLEGAL_MEMORY_ACCESS);
    }
    
    return op;
}

// by Agnieszka:
int ble(struct IMPSS* state, int body){
	int mask = 0x03E00000;
	int r1 = (body & mask) >> 21;
	
	mask = 0x001F0000;
	int r2 = (body & mask) >> 16;

	mask = 0x0000FFFF;
	int immediate = body & mask;	
			
	if(state -> registers[r1] <= state -> registers[r2]){
		state -> PC = state -> PC + (memory(state, immediate) << 2);
	} else {
	    state -> PC += 4;
	}
	return SUCCESS;
}

int bge(struct IMPSS* state, int body){
    int rMask = 31;
    int immMask = 131071;
    
    int r1 = (body & (rMask << 21)) >> 21;
    int r2 = (body & (rMask << 16)) >> 16;
    int immediate = body & immMask;						
    
    if(state -> registers[r1] >= state -> registers[r2]){
        state -> PC = state -> PC + (memory(state, immediate) << 2);
    } else {
	    state -> PC += 4;
	}
    
    return SUCCESS;
}

int jmp(struct IMPSS* state, int body){
    int addrMask = 134217727;
    int address = body & addrMask;
    
    state -> PC = address;    

    return SUCCESS;
}

int jr(struct IMPSS* state, int body){
    int rMask = 31;
    int r1 = (body & (rMask << 21)) >> 21;

    state -> PC = state -> registers[r1];

    return SUCCESS;
}

int jal(struct IMPSS* state, int body){
    int addrMask = 134217727;
    int address = body & addrMask;
    
    state -> registers[31] = state -> PC + 4;
    state -> PC = address;

    return SUCCESS;
}

// by Agnieszka:
int addi(struct IMPSS* state, int body){
	int mask = 0x03E00000;
	int r1 = (body & mask) >> 21;
	
	mask = 0x001F0000;
	int r2 = (body & mask) >> 16;

	mask = 0x0000FFFF;
	int immediate = body & mask;

	state -> registers[r1] = state -> registers[r2] + immediate;
	state -> PC += 4;
	return SUCCESS;
}

// by Agnieszka:
int subi(struct IMPSS* state, int body){
	int mask = 0x03E00000;
	int r1 = (body & mask) >> 21;
	
	mask = 0x001F0000;
	int r2 = (body & mask) >> 16;

	mask = 0x0000FFFF;
	int immediate = body & mask;

	state -> registers[r1] = state -> registers[r2] - immediate;
	state -> PC += 4;
	return SUCCESS;
}

// by Agnieszka:
int muli(struct IMPSS* state, int body){
	int mask = 0x03E00000;
	int r1 = (body & mask) >> 21;
	
	mask = 0x001F0000;
	int r2 = (body & mask) >> 16;

	mask = 0x0000FFFF;
	int immediate = body & mask;

	state -> registers[r1] = state -> registers[r2] * immediate;
	state -> PC += 4;
	return SUCCESS;
}


//❤                   L S                   ❤

int halt(struct IMPSS* state, int body){

	return SUCCESS;
}

int add(struct IMPSS* state, int body){
	//R type function.
	//0-5 opcode  | 6 - 10 R1 | 11 - 15 R2 | 16 - 20 R3 | unused |

	int maskR1 = 0x03E00000;
	int maskR2 = 0x001F0000;
	int maskR3 = 0x0000F100;

	int r1 = (mody & maskR1) >> 21;
	int r2 = (mody & maskR2) >> 16;
	int r3 = (mody & maskR3) >> 11;

	state -> registers[r1] = (state -> registers[r2]) + (state -> registers[r3]);
	state -> PC += 4;
	return SUCCESS;
}

int sub(struct IMPSS* state, int body) {
	//R type function.
	//0-5 opcode  | 6 - 10 R1 | 11 - 15 R2 | 16 - 20 R3 | unused |

	int maskR1 = 0x03E00000;
	int maskR2 = 0x001F0000;
	int maskR3 = 0x0000F100;

	int r1 = (mody & maskR1) >> 21;
	int r2 = (mody & maskR2) >> 16;
	int r3 = (mody & maskR3) >> 11;

	state -> registers[r1] = (state -> registers[r2]) - (state -> registers[r3]);
	state -> PC += 4;
	return SUCCESS;
}

int mul(struct IMPSS* state, int body) {
	//R type function.
	//0-5 opcode  | 6 - 10 R1 | 11 - 15 R2 | 16 - 20 R3 | unused |

	int maskR1 = 0x03E00000;
	int maskR2 = 0x001F0000;
	int maskR3 = 0x0000F100;

	int r1 = (mody & maskR1) >> 21;
	int r2 = (mody & maskR2) >> 16;
	int r3 = (mody & maskR3) >> 11;

	state -> registers[r1] = (state -> registers[r2]) * (state -> registers[r3]);
	state -> PC += 4;
	return SUCCESS;
}

int lw(struct IMPSS* state, int body) {
	//I type function.
	//0-5 opcode  | 6 - 10 R1 | 11 - 15 R2 | 16 - 31 Immediate value

	int maskR1 = 0x03E00000;
	int maskR2 = 0x001F0000;
	int maskImmediate = 0x0000FFFF;

	int r1 = (body & maskR1) >> 21;
	int r2 = (body & maskR2) >> 16;

	int immediate = body & maskImmediate;

	state -> registers[r1] = state -> memory[state -> registers[r2] + immediate];
	state -> PC += 4;
	return SUCCESS;
}

int sw(struct IMPSS* state, int body) {
	//I type function.
	//0-5 opcode  | 6 - 10 R1 | 11 - 15 R2 | 16 - 31 Immediate value

	int maskR1 = 0x03E00000;
	int maskR2 = 0x001F0000;
	int maskImmediate = 0x0000FFFF;

	int r1 = (body & maskR1) >> 21;
	int r2 = (body & maskR2) >> 16;

	int immediate = body & maskImmediate;

	state -> memory[state -> registers[r2] + immediate] = state -> registers[r1];
	state -> PC += 4;
	return SUCCESS;
}

//                 L   S   .  .   . e  n  d ❤  //


// OpCodeFunction is a function pointer and points to function
// which takes IMPSS* and int as arguments and returns an int
typedef int (*OpCodeFunction)(struct IMPSS*, int);  


int main(int argc, char *argv[]){
	// instructions[]
	
	struct IMPSS impss;
	
	OpCodeFunction OpCodeToFunction[18];

	OpCodeToFunction[0] = &halt;
	OpCodeToFunction[1] = &add;
	OpCodeToFunction[2] = &addi;
	OpCodeToFunction[3] = &sub;
	OpCodeToFunction[4] = &subi;
	OpCodeToFunction[5] = &mul;
	OpCodeToFunction[6] = &muli;
	OpCodeToFunction[7] = &lw;
	OpCodeToFunction[8] = &sw;
	OpCodeToFunction[9] = &beq;
	OpCodeToFunction[10] = &bne;
	OpCodeToFunction[11] = &blt;
	OpCodeToFunction[12] = &bgt;
	OpCodeToFunction[13] = &ble;
	OpCodeToFunction[14] = &bge;
	OpCodeToFunction[15] = &jmp;
	OpCodeToFunction[16] = &jr;
	OpCodeToFunction[17] = &jal;
	

	OpCodeToFunction[0] = (*OpCodeFunction)(*impss,instructions[0]);
	

	return 0;
}












