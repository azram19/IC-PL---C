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
#define ERR_NOT_ENOUGH_MEMORY 4
#define ERR_CANT_OPEN_FILE 5

/*
 * Masks, they have to be shifted to the right position.
 */
#define M_OPCODE 0x0000003F
#define M_REGISTER 0x0000001F
#define M_ADDRESS 0x03FFFFFF
#define M_IMM 0x0000FFFF

/*
 * Struct used to hold current state of IMP machine.
 *
 * Functions:
 * get_register() - gets register's value
 * set_register() - sets register to the given value
 * get_memory()   - gets value at the given memory address
 * set_memory()   - sets value at the given memoty address
 *
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
                fprintf(stderr, "Someone lied to you, we don't have it in our offer. Program terminated.\n");    
                break;
            }
        case ERR_ILLEGAL_MEMORY_ACCESS: {
                fprintf(stderr, "You've just tried to rape a memory, we don't like our memory being raped. Don't ever do it again. Program terminated.\n");
                break;
            }
        case ERR_WRONG_REGISTER: {
                fprintf(stderr, "404; register doesn't exist. Program terminated.\n");    
                break;
            }
        case ERR_NOT_ENOUGH_MEMORY: {
                fprintf(stderr, "You wanted more than we could give you. Program terminated.\n");    
                break;
            }
        case ERR_CANT_OPEN_FILE: {
            fprintf(stderr, "We couldn't open it, are you sure you have the right key? Program terminated.\n");    
            break;
        }         
    }
    printf("\n");
    exit(EXIT_FAILURE);
    return ERROR;
}

int error_file(int error_code, char * filename){
    fprintf(stderr, "Problem with file: %s\n", filename); 
    error(error_code);
    return ERROR;
}

/*
 * Returns 32 bits from memory.
 *
 * @author Lukasz Koprowski <azram19@gmail.com>
 * @return 32 bits from given memory address.
 */
int get_memory(struct IMPSS* state, int address){
    if(address < 0 || address >= SIZE_OF_MEMORY){
        printf("E: %d\n", address);
        error(ERR_ILLEGAL_MEMORY_ACCESS);
    }

    /*
     * Because memory is stored in bytes, it has to be merged into 32bit number.
     */
    int value = 0;
    int i;
    
    for(i = 0; i < 4; i++){
        value <<= 8;
        value += (state -> memory[address] & 0x000000FF);
        address++;
        //printf("%.2x ", state -> memory[address + i]); //r
    }
    
    //printf("\n%.8x\n", value); //r
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
     
    int mask = 0x00000FF;
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

/*
 * Branches if r1 less or equal to r2.
 *
 * @instruction-type I
 * @author Agnieszka Szefer <agnieszka.m.szefer@gmail.com>
 */
int ble(struct IMPSS* state, int body){
	
	int r1 = (body & (M_REGISTER << 21)) >> 21;
	int r2 = (body & (M_REGISTER << 16)) >> 16;
	int immediate = sign_extension(body & M_IMM);	
			
	if(get_register(state, r1) <= get_register(state, r2)){
		state -> PC = state -> PC + (immediate << 2);
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
    
    int r1 = (body & (M_REGISTER << 21)) >> 21;
    int r2 = (body & (M_REGISTER << 16)) >> 16;
    int immediate = sign_extension(body & M_IMM);						
    
    if(get_register(state, r1) >= get_register(state, r2)){
        state -> PC = state -> PC + (immediate << 2);
    } else {
	    state -> PC += 4;
	}
    
    return SUCCESS;
}

//------------------
int beq(struct IMPSS* state, int body){

	int r1 = (body & (M_REGISTER << 21)) >> 21;
	int r2 = (body & (M_REGISTER << 16)) >> 16;
	int immediate = sign_extension(body & M_IMM);

	if(get_register(state, r1) == get_register(state, r2)){
		state -> PC = state -> PC + (immediate << 2);
	} else {
		state -> PC += 4;
	}
	return SUCCESS;
}

int bne(struct IMPSS* state, int body){

	int r1 = (body & (M_REGISTER << 21)) >> 21;
	int r2 = (body & (M_REGISTER << 16)) >> 16;
	int immediate = sign_extension(body & M_IMM);

	if(get_register(state, r1) != get_register(state, r2)){
		state -> PC = state -> PC + (immediate << 2);
	} else {
		state -> PC += 4;
	}
	return SUCCESS;
}

int blt(struct IMPSS* state, int body){

	int r1 = (body & (M_REGISTER << 21)) >> 21;
	int r2 = (body & (M_REGISTER << 16)) >> 16;
	int immediate = sign_extension(body & M_IMM);

	if(get_register(state, r1) < get_register(state, r2)){
		state -> PC = state -> PC + (immediate << 2);
	} else {
		state -> PC += 4;
	}
	return SUCCESS;
}

int bgt(struct IMPSS* state, int body){

	int r1 = (body & (M_REGISTER << 21)) >> 21;
	int r2 = (body & (M_REGISTER << 16)) >> 16;
	int immediate = sign_extension(body & M_IMM);

	if(get_register(state, r1) > get_register(state, r2)){
		state -> PC = state -> PC + (immediate << 2);
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

    int address = body & M_ADDRESS;
    
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

    int r1 = (body & (M_REGISTER << 21)) >> 21;

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

    int address = body & M_ADDRESS;
    
    set_register(state, 31, state -> PC + 4);
    state -> PC = address;

    return SUCCESS;
}

/*
 * Adds immediate value to r2 and stores result in r1.
 *
 * @instruction-type I
 * @author Agnieszka Szefer <agnieszka.m.szefer@gmail.com>
 */
int addi(struct IMPSS* state, int body){

	int r1 = (body & (M_REGISTER << 21)) >> 21;
	int r2 = (body & (M_REGISTER << 16)) >> 16;
	int immediate = sign_extension(body & M_IMM);

	set_register(state, r1, get_register(state, r2) + immediate);
	state -> PC += 4;
	return SUCCESS;
}

/*
 * Subtracts immediate value from r2 and stores result in r1.
 *
 * @instruction-type I
 * @author Agnieszka Szefer <agnieszka.m.szefer@gmail.com>
 */
int subi(struct IMPSS* state, int body){

	int r1 = (body & (M_REGISTER << 21)) >> 21;
	int r2 = (body & (M_REGISTER << 16)) >> 16;
	int immediate = sign_extension(body & M_IMM);

	set_register(state, r1, get_register(state, r2) - immediate);
	state -> PC += 4;
	return SUCCESS;
}

/*
 * Multiplicates r2 by an immediate value and stores result in r1.
 *
 * @instruction-type I
 * @author Agnieszka Szefer <agnieszka.m.szefer@gmail.com>
 */
int muli(struct IMPSS* state, int body){

	int r1 = (body & (M_REGISTER << 21)) >> 21;
	int r2 = (body & (M_REGISTER << 16)) >> 16;
	int immediate = sign_extension(body & M_IMM);

	set_register(state, r1, get_register(state, r2) * immediate);
	state -> PC += 4;
	return SUCCESS;
}

/*
 * Terminates program, prints registers and program counter.
 *
 * @instruction-type N/A
 * @author Lukasz Koprowski <azram19@gmail.com>
 */
int halt(struct IMPSS* state, int body){
    state -> PC += 4;

    printf("Regsiters:\n");
    int i;
    
    printf("PC: %d (%#x)\n", state -> PC, state -> PC);
    
    for(i = 0; i < NUMBER_OF_REGISTERS; i++){
        printf("$%d: %d (%#x)\n", i, get_register(state, i), get_register(state, i));
    }
    
    return HALT;
}

/*
 * Adds contents of R2 and R3 and stores them in R1
 *
 * @instruction-type R
 * @author Lukasz Kmiecik <moa.1991@gmail.com>
 */
int add(struct IMPSS* state, int body){
	//R type function.
	//0-5 opcode  | 6 - 10 R1 | 11 - 15 R2 | 16 - 20 R3 | unused |

	int r1 = (body & (M_REGISTER << 21)) >> 21;
	int r2 = (body & (M_REGISTER << 16)) >> 16;
	int r3 = (body & (M_REGISTER << 11)) >> 11;

	set_register(state, r1, get_register(state, r2) + get_register(state, r3));
	state -> PC += 4;
	return SUCCESS;
}

/*
 * Substitutes contents of R3 from R2 and stores them in R1
 *
 * @instruction-type R
 * @author Lukasz Kmiecik <moa.1991@gmail.com>
 */
int sub(struct IMPSS* state, int body) {
	//R type function.
	//0-5 opcode  | 6 - 10 R1 | 11 - 15 R2 | 16 - 20 R3 | unused |

	int r1 = (body & (M_REGISTER << 21)) >> 21;
	int r2 = (body & (M_REGISTER << 16)) >> 16;
	int r3 = (body & (M_REGISTER << 11)) >> 11;

	set_register(state, r1, get_register(state, r2) - get_register(state, r3));
	state -> PC += 4;
	return SUCCESS;
}

/*
 * Multiplies contents of R3 by contents of R2 and stores them in R1
 *
 * @instruction-type R
 * @author Lukasz Kmiecik <moa.1991@gmail.com>
 */
int mul(struct IMPSS* state, int body) {
	//R type function.
	//0-5 opcode  | 6 - 10 R1 | 11 - 15 R2 | 16 - 20 R3 | unused |

	int r1 = (body & (M_REGISTER << 21)) >> 21;
	int r2 = (body & (M_REGISTER << 16)) >> 16;
	int r3 = (body & (M_REGISTER << 11)) >> 11;

	set_register(state, r1, get_register(state, r2) * get_register(state, r3));
	state -> PC += 4;
	return SUCCESS;
}

/*
 * Stores contents of memory address determined by R2 + constant in R1
 *
 * @instruction-type R
 * @author Lukasz Kmiecik <moa.1991@gmail.com>
 */
int lw(struct IMPSS* state, int body) {
	//I type function.
	//0-5 opcode  | 6 - 10 R1 | 11 - 15 R2 | 16 - 31 Immediate value

	int r1 = (body & (M_REGISTER << 21)) >> 21;
	int r2 = (body & (M_REGISTER << 16)) >> 16;
	int immediate = sign_extension(body & M_IMM);

	set_register(state, r1, get_memory(state, get_register(state, r2) + immediate));
	state -> PC += 4;
	return SUCCESS;
}

/*
 * Copies contents of R1 to a memory location determined by contents of R2 + constant
 *
 * @instruction-type R
 * @author Lukasz Kmiecik <moa.1991@gmail.com>
 */
int sw(struct IMPSS* state, int body) {
	//I type function.
	//0-5 opcode  | 6 - 10 R1 | 11 - 15 R2 | 16 - 31 Immediate value

	int r1 = (body & (M_REGISTER << 21)) >> 21;
	int r2 = (body & (M_REGISTER << 16)) >> 16;
	int immediate = sign_extension(body & M_IMM);

	set_memory(state, get_register(state, r2) + immediate, get_register(state, r1));
	state -> PC += 4;
	return SUCCESS;
}


/*
 * OpCodeFunction is a function pointer and points to function
 * which takes IMPSS* and int as arguments and returns an int
 */

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
		error(ERR_NOT_ENOUGH_MEMORY);
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
	    error_file(ERR_CANT_OPEN_FILE, filename);
		return ERROR;
	}
	fseek(fileptr, 0, SEEK_END);
	int ninstructions = ftell(fileptr) >> 2;
	rewind(fileptr);
	fclose(fileptr);
	return ninstructions;
}
// Binary Reader END

/*
 * Converts between endiness.
 * 
 * @author Lukasz Koprowski <azram19@gmail.com>
 */
int letobe(int l){
    int b = 0;
    int i;
    
    int mask = 0x0000FF;
    
    /*for(i = 0; i < 32; i++){
        b = (b << 1) | ((l & ((1 << 31) >> i)) >> (31 - i));
    }*/
    /*for(i = 0; i < 4; i++){
        b = (b << 8) | ();
    }*/
    
    /*printf("l %#.8x\n",l);
    printf("b %#.8x\n",b); 
    b = (b << 8) | ((l & 0x000000FF));
    printf("b %#.8x\n",b); 
    b = (b << 8) | ((l & 0x0000FF00) >> 8);
    printf("b %#.8x\n",b);
    b = (b << 8) | ((l & 0x00FF0000) >> 16); 
    printf("b %#.8x\n",b);
    b = (b << 8) | ((l & 0xFF000000) >> 24);
    
    
    printf("b %#.8x\n\n",b);*/    
    
    
    return l;
}

int sign_extension(int instr){
    return instr;
    
    int value = (0x0000FFFF & instr);
    int mask = 0x00008000;
    int sign = (mask & instr) >> 15;
    if (sign == 1)
        value += 0xFFFF0000;
    
    return value;
}

/*
 * Main
 * @author Agnieszka Szefer <agnieszka.m.szefer@gmail.com>
 * @author Piotr Bar
 */
int main(int argc, char *argv[]){
    letobe(0xFF000000);
	
	int *instructions = NULL;
	char filename[100];
	strcpy(filename,argv[1]);
	int ninstructions = arraysize(filename);
	instructions = binaryreader(filename, instructions, ninstructions);
	//For tests: writes to the file test.txt
	//binarywriter(filename,instructions, ninstructions);
	
/*
 * Initialising state of IMPS.
 */
	struct IMPSS impss;
	struct IMPSS *state = &impss;

	int j;
	state -> PC = 0;
	
	for(j=0; j < NUMBER_OF_REGISTERS; ++j){
		state -> registers[j] = 0;
	}
	
	for(j=0; j < SIZE_OF_MEMORY; ++j){
		state -> memory[j] = 0;
	}
	
	set_memory(state, 4, 0x00DD0000);
	set_memory(state, 8, 0x0000DD00);
	set_memory(state, 12, 0xFF00C79F);
	set_memory(state, 16, 0xFF00C79F);
	set_memory(state, 20, 0xFF00C79F);
	
	printf("T %#.8x\n", -7);
	printf("T %#.8x\n", sign_extension(-7));
	
	printf("%#.8x\n", get_memory(state, 4));
	printf("%#.8x\n", get_memory(state, 8));
	printf("%#.8x\n", get_memory(state, 12));
	printf("%#.8x\n", get_memory(state, 16));
	printf("%#.8x\n", get_memory(state, 20));
	
	struct IMPSS state_copy;
	state_copy = impss;
		
	for(j = 0; j < ninstructions; j++){
	    set_memory(state, j*4, letobe(instructions[j]));
	    //printf("\nM: %d - %d (%#x)\n", j*4, (letobe(instructions[j]) & (M_OPCODE << 26)) >> 26, letobe(instructions[j]));
	    //printf("M: %d - %d (%#x)\n", j*4, (get_memory(state, j*4) & (M_OPCODE << 26)) >> 26, get_memory(state, j*4));
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

	// EMULATOR LOOP
	// data section - niepotrzebne (tak mi sie wydaje, sorry :/)
	//state -> PC = (get_memory(state, 0) & M_ADDRESS);	
	/*int i;
	printf("DATA:\n"); //r
	for(i = 4; i < (state -> PC); i += 4){
		printf("D: %d \n", get_memory(state, i));//r
	}*/
	// instructions section
	printf("\nINSTRUCTIONS:\n"); //r
	while(1){
	    printf("IN: %#x\n", get_memory(state, state->PC));
		int index = (get_memory(state, state->PC) & (M_OPCODE << 26)) >> 26;
		printf("OP: %d PC %d\n", index, state -> PC);//r
		int result = (*OpCodeToFunction[op_code(index)])(state, get_memory(state, state -> PC));
		if(result == HALT) break;
		/*for(j = 0; j < ninstructions; j++){
	    //set_memory(state, j*4, letobe(instructions[j]));
	        printf("M: %d - %d (%#x)\n", j*4, (get_memory(state, j*4) & (M_OPCODE << 26)) >> 26, get_memory(state, j*4));
	    }*/
	}
			
	
	/*printf("%d\n", (0x01080000 & (M_REGISTER << 21)) >> 21);
	printf("%d\n", (0x01080000 & (M_REGISTER << 16)) >> 16);
	printf("%d\n", (0x01084000 & (M_REGISTER << 11)) >> 11);
	printf("%d\n", (0x01080000 & M_IMM));
	printf("%d\n", (0x01080000 & M_ADDRESS));*/
	/*
	int i;
	for(i = 1; i < 18; i++){
	    printf("\nI - %d\n", i);
	    state_copy2 = state_copy;
	    (*OpCodeToFunction[i])(&state_copy, 0x32478123);
	    halt(&state_copy, 0);
	}
	*/
		
	printf("\n");
	return 0;
}












