#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include<assert.h>
#include<stdint.h>
#include<errno.h>

#define NUMBER_OF_REGISTERS 32
#define SIZE_OF_MEMORY 65536
#define NUMBER_OF_INSTRUCTIONS 18 

#define SUCCESS 1
#define HALT 0
#define ERROR -1

#define ERR_UNDEFINED_OPERATION 1
#define ERR_ILLEGAL_MEMORY_ACCESS 2
#define ERR_WRONG_REGISTER 3


/*
 * Struct used to hold current state of IMP machine.
 *
 * @author Lukasz Koprowski <azram19@gmail.com>
 */

struct IMPSS{
    int registers[NUMBER_OF_REGISTERS];
    char memory[SIZE_OF_MEMORY]; //I use `char` to get one byte memory cells
    int PC;
};

/*
 * Handles error codes.
 *
 * @author Lukasz Koprowski <azram19@gmail.com>
 */

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
 *
 * @author Lukasz Koprowski <azram19@gmail.com>
 * @return 32 bits from given memory address.
 */
int get_memory(struct IMPSS* state, int address){
    if(address < 0 || address >= SIZE_OF_MEMORY){
        error(ERR_ILLEGAL_MEMORY_ACCESS);
    }

    /*
     * Because memory is stored in bytes, it has to be merged into 32bit number.
     */
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
 * Sets memory to given value
 *
 * @author Lukasz Koprowski <azram19@gmail.com>
 * @return value which memory has been set to
 */
int set_memory(struct IMPSS* state, int address, int value){
    if(address < 0 || address >= SIZE_OF_MEMORY){
        error(ERR_ILLEGAL_MEMORY_ACCESS);
    }
    
    /*
     * Memory is stored in bytes, value has to be divided into four bytes and
     * then written to the right cells.
     */
    int mask = 511;
    int i;
    for(i = 0; i < 4; i++){
        state -> memory[address + i] = (value & (mask << (24 - 8*i))) >> (24 - 8*i);
    }
    return value;
}

/*
 * Returns value of 32 bit register.
 *
 * @author Lukasz Koprowski <azram19@gmail.com>
 * @return value of the given register
 */
int get_register(struct IMPSS* state, int reg_num){
    if(reg_num < 0 || reg_num >= NUMBER_OF_REGISTERS){
        error(ERR_WRONG_REGISTER);
    }
    
    return state -> registers[reg_num];
}

/* 
 * Sets value of 32 bit register.
 *
 * @author Lukasz Koprowski <azram19@gmail.com>
 * @return value which register has been set to
 */
int set_register(struct IMPSS* state, int reg_num, int value){
    if(reg_num < 0 || reg_num >= NUMBER_OF_REGISTERS){
        error(ERR_WRONG_REGISTER);
    }
    
    state -> registers[reg_num] = value;
    return value;
}

/*
 * Validates opcode.
 *
 * @author Lukasz Koprowski <azram19@gmail.com>
 * @return opcode
 */
int op_code(int op){
    if(op < 0 || op >= NUMBER_OF_INSTRUCTIONS){
        error(ERR_UNDEFINED_OPERATION);
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
			
	if(get_register(state, r1) <= get_register(state, r2)){
		state -> PC = state -> PC + (get_memory(state, immediate) << 2);
	} else {
	    state -> PC += 4;
	}
	return SUCCESS;
}

/*
 * Branches if r1 greater or equal to r2.
 *
 * @instruction-type I
 * @author Lukasz Koprowski <azram19@gmail.com>
 */
int bge(struct IMPSS* state, int body){
    int rMask = 31;
    int immMask = 131071;
    
    int r1 = (body & (rMask << 21)) >> 21;
    int r2 = (body & (rMask << 16)) >> 16;
    int immediate = body & immMask;						
    
    if(get_register(state, r1) >= get_register(state, r2)){
        state -> PC = state -> PC + (get_memory(state, immediate) << 2);
    } else {
	    state -> PC += 4;
	}
    
    return SUCCESS;
}

//------------------
int beq(struct IMPSS* state, int body){
	int r1mask = 0x03E00000;
	int r2mask = 0x001F0000;
	int valmask = 0x0000FFFF;
	int r1 = (body & r1mask) >> 21;
	int r2 = (body & r2mask) >> 16;
	int immediate = body & valmask;

	if(get_register(state, r1) == get_register(state, r2)){
		state -> PC = state -> PC + (get_memory(state, immediate) << 2);
	} else {
		state -> PC += 4;
	}
	return SUCCESS;
}

int bne(struct IMPSS* state, int body){
	int r1mask = 0x03E00000;
	int r2mask = 0x001F0000;
	int valmask = 0x0000FFFF;
	int r1 = (body & r1mask) >> 21;
	int r2 = (body & r2mask) >> 16;
	int immediate = body & valmask;

	if(get_register(state, r1) != get_register(state, r2)){
		state -> PC = state -> PC + (get_memory(state, immediate) << 2);
	} else {
		state -> PC += 4;
	}
	return SUCCESS;
}

int blt(struct IMPSS* state, int body){
	int r1mask = 0x03E00000;
	int r2mask = 0x001F0000;
	int valmask = 0x0000FFFF;
	int r1 = (body & r1mask) >> 21;
	int r2 = (body & r2mask) >> 16;
	int immediate = body & valmask;

	if(get_register(state, r1) < get_register(state, r2)){
		state -> PC = state -> PC + (get_memory(state, immediate) << 2);
	} else {
		state -> PC += 4;
	}
	return SUCCESS;
}

int bgt(struct IMPSS* state, int body){
	int r1mask = 0x03E00000;
	int r2mask = 0x001F0000;
	int valmask = 0x0000FFFF;
	int r1 = (body & r1mask) >> 21;
	int r2 = (body & r2mask) >> 16;
	int immediate = body & valmask;

	if(get_register(state, r1) > get_register(state, r2)){
		state -> PC = state -> PC + (get_memory(state, immediate) << 2);
	} else {
		state -> PC += 4;
	}
	return SUCCESS;
}

//------------------

/*
 * Jumps to the given address.
 * 
 * @instruction-type J
 * @author Lukasz Koprowski <azram19@gmail.com>
 */
int jmp(struct IMPSS* state, int body){
    int addrMask = 134217727;
    int address = body & addrMask;
    
    state -> PC = address;    

    return SUCCESS;
}

/*
 * Jumps to an address given in the register.
 *
 * @instruction-type R
 * @author Lukasz Koprowski <azram19@gmail.com>
 */
int jr(struct IMPSS* state, int body){
    int rMask = 31;
    int r1 = (body & (rMask << 21)) >> 21;

    state -> PC = get_register(state, r1);

    return SUCCESS;
}

/*
 * Jumps to the given address, and stores address of next instruction in 
 * a register.
 *
 * @instruction-type J
 * @author Lukasz Koprowski <azram19@gmail.com>
 */
int jal(struct IMPSS* state, int body){
    int addrMask = 134217727;
    int address = body & addrMask;
    
    set_register(state, 31, state -> PC + 4);
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

	set_register(state, r1, get_register(state, r2) + immediate);
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

	set_register(state, r1, get_register(state, r2) - immediate);
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

	set_register(state, r1, get_register(state, r2) * immediate);
	state -> PC += 4;
	return SUCCESS;
}


//❤        L S  .  .  . f  r  o  m         h   e  r   e       ❤

int halt(struct IMPSS* state, int body){

	return HALT;
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

	set_register(state, r1, get_register(state, r2) + get_register(state, r3));
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

	set_register(state, r1, get_register(state, r2) - get_register(state, r3));
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

	set_register(state, r1, get_register(state, r2) * get_register(state, r3));
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

	set_register(state, r1, get_memory(state, get_register(state, r2) + immediate));
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

	set_memory(state, get_register(state, r2) + immiediate, get_register(state, r1));
	state -> PC += 4;
	return SUCCESS;
}

//                 L   S   .  .   . e  n  d        ❤  //


// OpCodeFunction is a function pointer and points to function
// which takes IMPSS* and int as arguments and returns an int
typedef int (*OpCodeFunction)(struct IMPSS*, int);  

// Binary Reader START
//For tests
void print_bits(int x){
	int i;
	int mask = 1 << 31;
	for(i=1;i<=32;i++){
		if((x & mask) == 0) printf("0"); else printf("1");
		if(i==16) printf(" ");
		x = x << 1;
	}
	printf("\n");
}
//For tests
void print(int *instructions, int ninstructions){
	int i;
	printf("%d", ninstructions);
	for(i=0;i<ninstructions;i++){
		print_bits(instructions[i]);
	}
}

int * createarray(int ninstructions){
	int *instructions = malloc(ninstructions*sizeof(int));
	if(instructions==NULL){
		perror("malloc");
		return NULL;
	}
	int i;
	for(i=0;i<ninstructions;i++){
		instructions[i]=0;
	}
	return instructions;
}

int * binaryreader(char *filename, int *instructions, int ninstructions){
	FILE *fileptr = fopen(filename, "rb");
	instructions = (int *) createarray(ninstructions);
	assert(instructions!=NULL);
	fread(instructions, sizeof(instructions[0]), ninstructions, fileptr);
	fclose(fileptr);
	return instructions;
}
//For tests
void binarywriter(char *filename, int *instructions, int ninstructions){
	FILE *fileptr = fopen("test.txt", "wb");
	fwrite(instructions, sizeof(instructions[0]), ninstructions, fileptr);
	fclose(fileptr);
}

int arraysize(char *filename){
	FILE *fileptr = fopen(filename, "rb");
	if (fileptr == NULL) {
		printf("%s: can't open: %s\n",filename,strerror(errno));
		return 1;
	}
	fseek(fileptr, 0, SEEK_END);
	int ninstructions = ftell(fileptr) >> 2;
	rewind(fileptr);
	fclose(fileptr);
	return ninstructions;
}
// Binary Reader END

int main(int argc, char *argv[]){

	int *instructions = NULL;
	char filename[100];
	strcpy(filename,argv[1]);
	int ninstructions = arraysize(filename);
	instructions = binaryreader(filename, instructions, ninstructions);
	//For tests: writes to the file test.txt
	//binarywriter(filename,instructions, ninstructions);
	
	struct IMPSS impss;
	*impss state;
	int j;
	for(j=0; j < NUMBER_OF_REGISTERS; ++j){
		state -> registers[j] = 0;
	}
	
	for(j=0; j < SIZE_OF_MEMORY; ++j){
		state -> memory[j] = 0;
	}
	
	for(j = 0; j < ninstructions; j += 4){
	    set_memory(state, j, instructions[j/4]);
	}
	free(instructions);
	
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
	
	// mask used to extract opcode bits
	int mask = 0xFC000000;
	int i;

	// odczytaj adres pierszej instrukcji
	// wykonaj wszystkie instrukcje między pierwszą a adresem bez wykonywania haltów
	// halt: continue
	// i = adres
	// L_KPR:  Zmienilem instructions na get_memory(), bo składujemy instrukcje w pamieci, razem z danymi.
	for(i=0; i < ninstructions; ++i){
		int index = (get_memory(state, i) & mask) >> 26;
		int result = (*OpCodeToFunction[op_code(index)])(state, get_memory(state, i));
		if(result == HALT) break;
	}
	
	
	return 0;
}












