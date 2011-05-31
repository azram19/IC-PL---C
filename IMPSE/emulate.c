#include<string.h>
#include<stdlib.h>
#include<stdio.h>

#define NUMBER_OF_REGISTERS 32
#define SIZE_OF_MEMORY 65536 

struct IMPSS {
	int registers[NUMBER_OF_REGISTERS];
	char memory[SIZE_OF_MEMORY]; //I use `char` to get one byte memory cells
	int PC;
};

/*
 * Returns 32 bits from memory.
 */
int memory(struct IMPSS* state, int address) {
	int value = 0;
	int i;
	for (i = 0; i < 4; i++) {
		value += (int) state -> memory[address];
		address++;
		value <<= 8;
	}
	return value;
}

// by Agnieszka:
int ble(struct IMPSS* state, int body) {
	int mask = 0x03E00000;
	int r1 = (body & mask) >> 21;

	mask = 0x001F0000;
	int r2 = (body & mask) >> 16;

	mask = 0x0000FFFF;
	int immediate = body & mask;

	if (state -> registers[r1] <= state -> registers[r2]) {
		state -> PC = state -> PC + (memory(state, immediate) << 2);
	}
	return SUCCESS;
}

int bge(struct IMPSS* state, int body) {
	int rMask = 31;
	int immMask = 131071;

	int r1 = (body & (rMask << 21)) >> 21;
	int r2 = (body & (rMask << 16)) >> 16;
	int immediate = body & immMask; // immediate not immidiate ;)

	if (state -> registers[r1] > state -> registers[r2]) { // shouldn't it be >= ???
		state -> PC = state -> PC + (memory(state, immediate) << 2);
	}

	return SUCCESS;
}

int jmp(struct IMPSS* state, int body) {
	int addrMask = 134217727;
	int address = body & addrMask;

	state -> PC = address;

	return SUCCESS;
}

int jr(struct IMPSS* state, int body) {
	int rMask = 31;
	int r1 = (body & (rMask << 21)) >> 21;

	state -> PC = state -> registers[r1];

	return SUCCESS;
}

int jal(struct IMPSS* state, int body) {
	int addrMask = 134217727;
	int address = body & addrMask;

	state -> registers[31] = state -> PC + 4;
	state -> PC = address;

	return SUCCESS;
}

// by Agnieszka:
int addi(struct IMPSS* state, int body) {
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
int subi(struct IMPSS* state, int body) {
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
int muli(struct IMPSS* state, int body) {
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

int halt(struct IMPSS* state, int body) {
	return 0;
}

int add(struct IMPSS* state, int body) {
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

int main(int argc, char *argv[]) {
	return 0;
}
