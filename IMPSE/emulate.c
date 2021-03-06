/*
 * An IMPS emulator. Simulates the execution of an IMPS binary file on an 
 * IMPS machine.
 *
 * Team:
 * Agnieszka Szefer
 * Piotr Bar
 * Lukasz Kmiecik
 * Lukasz Koprowski
 */

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
#define ERR_EMPTY_STACK 6
#define ERR_FULL_STACK 7
#define STACK_INDEX_OUT_OF_BOUNDS 8

/*
 * Masks, they have to be shifted to the right position.
 */
#define M_OPCODE 0x0000003F
#define M_REGISTER 0x0000001F
#define M_ADDRESS 0x03FFFFFF
#define M_IMM 0x0000FFFF

#define MAX_STACK_SIZE 200

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
	struct stack* stackptr;
};

int get_register(struct IMPSS*, int);
int set_register(struct IMPSS*, int, int);
int get_memory(struct IMPSS*, int);
int set_memory(struct IMPSS*, int, int);

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
     
    int mask = 0x00000FF;
    int i;
    
    for(i = 0; i < 4; i++){
        state -> memory[address + i] = 
                (value & (mask << (24 - 8*i))) >> (24 - 8*i);
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
 * Handles error codes.
 *
 * @author Lukasz Koprowski <azram19@gmail.com>
 */

int error(int error_code){
    
    switch(error_code){
        case ERR_UNDEFINED_OPERATION: {
                fprintf(stderr, 
                        "Error: This operation is not defined.\n");  
                break;
            }
        case ERR_ILLEGAL_MEMORY_ACCESS: {
                 fprintf(stderr, 
                        "Error: Illegal memory access.\n");
                break;
            }
        case ERR_WRONG_REGISTER: {
                fprintf(stderr, 
                        "Error 404: register doesn't exist. Program terminated.\n");        
                break;
            }
        case ERR_NOT_ENOUGH_MEMORY: {
                fprintf(stderr, 
                        "Error: Not enough memory. Program terminated.\n");    
                break;
            }
        case ERR_CANT_OPEN_FILE: {
                fprintf(stderr, 
                        "Error: Can not open the file. Program terminated.\n");    
                break;
        }
				case ERR_EMPTY_STACK: {
								fprintf(stderr,
												"Error: Stack is empty.\n");
								break;
				}        
				case ERR_FULL_STACK: {
								fprintf(stderr,
												"Error: Stack is full.\n");
								break;
				}
				case STACK_INDEX_OUT_OF_BOUNDS: {
								fprintf(stderr,
												"Error: Index out of bounds.\n");
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
 * Terminates program, prints registers and program counter.
 *
 * @instruction-type N/A
 * @author Lukasz Koprowski <azram19@gmail.com>
 */
int halt(struct IMPSS* state, int body){
    state -> PC += 4;

    printf("\nRegisters:\n");
    int i;
    
    printf("PC : %10d (0x%.8x)\n", state -> PC, state -> PC);
    
    for(i = 0; i < NUMBER_OF_REGISTERS; i++){
        printf("$%-2d: %10d (0x%.8x)\n", i, 
                    get_register(state, i), get_register(state, i));
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
 * Adds immediate value to r2 and stores result in r1.
 *
 * @instruction-type I
 * @author Agnieszka Szefer <agnieszka.m.szefer@gmail.com>
 */
int addi(struct IMPSS* state, int body){

	int r1 = (body & (M_REGISTER << 21)) >> 21;
	int r2 = (body & (M_REGISTER << 16)) >> 16;
	int immediate = signed_extension(body & M_IMM);

	set_register(state, r1, get_register(state, r2) + immediate);
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
 * Subtracts immediate value from r2 and stores result in r1.
 *
 * @instruction-type I
 * @author Agnieszka Szefer <agnieszka.m.szefer@gmail.com>
 */
int subi(struct IMPSS* state, int body){

	int r1 = (body & (M_REGISTER << 21)) >> 21;
	int r2 = (body & (M_REGISTER << 16)) >> 16;
	int immediate = signed_extension(body & M_IMM);

	set_register(state, r1, get_register(state, r2) - immediate);
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
 * Multiplicates r2 by an immediate value and stores result in r1.
 *
 * @instruction-type I
 * @author Agnieszka Szefer <agnieszka.m.szefer@gmail.com>
 */
int muli(struct IMPSS* state, int body){

	int r1 = (body & (M_REGISTER << 21)) >> 21;
	int r2 = (body & (M_REGISTER << 16)) >> 16;
	int immediate = signed_extension(body & M_IMM);

	set_register(state, r1, get_register(state, r2) * immediate);
	state -> PC += 4;
	return SUCCESS;
}

/*
 * Stores contents of memory address determined by R2 + constant in R1
 *
 * @instruction-type I
 * @author Lukasz Kmiecik <moa.1991@gmail.com>
 */
int lw(struct IMPSS* state, int body) {
	//I type function.
	//0-5 opcode  | 6 - 10 R1 | 11 - 15 R2 | 16 - 31 Immediate value

	int r1 = (body & (M_REGISTER << 21)) >> 21;
	int r2 = (body & (M_REGISTER << 16)) >> 16;
	int immediate = signed_extension(body & M_IMM);

	set_register(state, r1, 
	                get_memory(state, get_register(state, r2) + immediate));
	                
	state -> PC += 4;
	return SUCCESS;
}

/*
 * Copies contents of R1 to a memory location determined by contents of 
 *  R2 + constant
 *
 * @instruction-type I
 * @author Lukasz Kmiecik <moa.1991@gmail.com>
 */
int sw(struct IMPSS* state, int body) {
	//I type function.
	//0-5 opcode  | 6 - 10 R1 | 11 - 15 R2 | 16 - 31 Immediate value

	int r1 = (body & (M_REGISTER << 21)) >> 21;
	int r2 = (body & (M_REGISTER << 16)) >> 16;
	int immediate = signed_extension(body & M_IMM);

	set_memory(state, get_register(state, r2) + immediate, get_register(state, r1));
	state -> PC += 4;
	return SUCCESS;
}

/*
 * Branches if r1 is equal to r2.
 *
 * @instruction-type I
 * @author Piotr Bar
 */
int beq(struct IMPSS* state, int body){

	int r1 = (body & (M_REGISTER << 21)) >> 21;
	int r2 = (body & (M_REGISTER << 16)) >> 16;
	int immediate = signed_extension(body & M_IMM);

	if(get_register(state, r1) == get_register(state, r2)){
		state -> PC = state -> PC + (immediate << 2);
	} else {
		state -> PC += 4;
	}
	return SUCCESS;
}

/*
 * Branches if r1 is not equal to r2.
 *
 * @instruction-type I
 * @author Piotr Bar
 */
int bne(struct IMPSS* state, int body){

	int r1 = (body & (M_REGISTER << 21)) >> 21;
	int r2 = (body & (M_REGISTER << 16)) >> 16;
	int immediate = signed_extension(body & M_IMM);

	if(get_register(state, r1) != get_register(state, r2)){
		state -> PC = state -> PC + (immediate << 2);
	} else {
		state -> PC += 4;
	}
	return SUCCESS;
}

/*
 * Branches if r1 is less than r2.
 *
 * @instruction-type I
 * @author Piotr Bar
 */
int blt(struct IMPSS* state, int body){

	int r1 = (body & (M_REGISTER << 21)) >> 21;
	int r2 = (body & (M_REGISTER << 16)) >> 16;
	int immediate = signed_extension(body & M_IMM);

	if(get_register(state, r1) < get_register(state, r2)){
		state -> PC = state -> PC + (immediate << 2);
	} else {
		state -> PC += 4;
	}
	return SUCCESS;
}

/*
 * Branches if r1 is greater than r2.
 *
 * @instruction-type I
 * @author Piotr Bar
 */
int bgt(struct IMPSS* state, int body){

	int r1 = (body & (M_REGISTER << 21)) >> 21;
	int r2 = (body & (M_REGISTER << 16)) >> 16;
	int immediate = signed_extension(body & M_IMM);

	if(get_register(state, r1) > get_register(state, r2)){
		state -> PC = state -> PC + (immediate << 2);
	} else {
		state -> PC += 4;
	}
	return SUCCESS;
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
	int immediate = signed_extension(body & M_IMM);	
			
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
    int immediate = signed_extension(body & M_IMM);						
    
    if(get_register(state, r1) >= get_register(state, r2)){
        state -> PC = state -> PC + (immediate << 2);
    } else {
	    state -> PC += 4;
	}
    
    return SUCCESS;
}

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
 * Calls another function
 * 
 * @instruction-type J
 * @author Agnieszka Szefer <agnieszka.m.szefer@gmail.com>
 */
int call(struct IMPSS* state, int body){
	int address = body & M_ADDRESS;
	  
  push(state->PC+4, state->stackptr);
	state->PC = address;
 
	return SUCCESS;
}

/*
 * Returns from a called function
 * 
 * @author Agnieszka Szefer <agnieszka.m.szefer@gmail.com>
 */
int ret(struct IMPSS* state){
	state->PC = pop(state->stackptr);	
	
	return SUCCESS;
}


/*
 * OpCodeFunction is a function pointer and points to a function
 * which takes IMPSS* and int as arguments and returns an int
 */

typedef int (*OpCodeFunction)(struct IMPSS*, int);  

/*
 * Method createarray takes an argument 'ninstructions' and allocates
 * memory for appropriate number of instructions. All instructions 
 * are initialised with value 0. It returns a pointer to an array of ints.
 *
 * @author Piotr Bar
 */
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

/*
 * Binaryreader takes 3 arguments, pointer to filename, pointer to an array
 * of instructions and a number of instructions. It reads instructions from a
 * file and puts them in the array of ints. Function returns a pointer to the
 * modified array.
 *
 * @author Piotr Bar
 */
int * binaryreader(char *filename, int *instructions, int ninstructions){
	FILE *fileptr = fopen(filename, "rb");
	instructions = (int *) createarray(ninstructions);
	assert(instructions!=NULL);
	fread(instructions, sizeof(instructions[0]), ninstructions, fileptr);
	fclose(fileptr);
	return instructions;
}

/*
 * Arraysize takes a filename as an argument and finds the number of 
 * instructions in a file. It returns an int ninstructions.
 *
 * @author Piotr Bar
 */
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


/* 
 * Extends 16bit number to 32 signed integer. 
 *
 * @author Lukasz Koprowski <azram19@gmail.com>
 * @return 32 signed integer
 */
int signed_extension(int in){
    int r = (0x0000FFFF & in);
    if (0x00008000 & in) {
        r |= 0xFFFF0000;
    }
    return r;
}


/*
 * Struct holds the stack
 *
 * @author Agnieszka Szefer <agnieszka.m.szefer@gmail.com>
 */
struct stack{
	int stackArr[MAX_STACK_SIZE];
	int top;
};

/*
 * Returns 1 if stack is empty, 0 otherwise.
 * 
 * @author Agnieszka Szefer <agnieszka.m.szefer@gmail.com>
 */
int isEmpty(struct stack * Stack){
	return (Stack->top)<=0;
}

/*
 * Puts newItem on the top of the stack.
 * 
 * @author Agnieszka Szefer <agnieszka.m.szefer@gmail.com>
 */
int push(int newItem, struct stack* Stack){
	if((Stack->top+1)>=MAX_STACK_SIZE) error(ERR_FULL_STACK);
	else{
		Stack->top++;
		Stack->stackArr[Stack->top] = newItem;
	}
	return SUCCESS;
}

/*
 * Removes top element from the stack.
 * 
 * @author Agnieszka Szefer <agnieszka.m.szefer@gmail.com>
 */
int pop(struct stack* Stack){
	if(isEmpty(Stack)) error(ERR_EMPTY_STACK);
	else{
		int topItem = Stack->stackArr[Stack->top];
		(Stack->top)--;
		return topItem;
	}
}

/*
 * Returns the item on the stack at given index.
 * 
 * @author Agnieszka Szefer <agnieszka.m.szefer@gmail.com>
 */
int get(int index, struct stack* Stack){
	if(index<0 || (index+1)>= MAX_STACK_SIZE) error(STACK_INDEX_OUT_OF_BOUNDS);
	else return Stack->stackArr[index];
}


/*
 * Main
 *
 * @author Agnieszka Szefer <agnieszka.m.szefer@gmail.com>
 * @author Piotr Bar
 */
int main(int argc, char *argv[]){
	int *instructions = NULL;
	char filename[100];
	strcpy(filename,argv[1]);
	int ninstructions = arraysize(filename);
	instructions = binaryreader(filename, instructions, ninstructions);
	
    /*
     * Initialising state of IMPS.
     */
	struct IMPSS impss;
	struct IMPSS *state = &impss;
	state -> stackptr = malloc(sizeof(struct stack)); 
    state -> stackptr -> top = 0;
    
    /*
     * Stack test 
    
    push(2, state -> stackptr);
    push(3, state -> stackptr);
    printf("%d %d", pop(state -> stackptr), pop(state -> stackptr));
    */
    
    return 0;
	int j;
	state -> PC = 0;
	for(j=0; j < NUMBER_OF_REGISTERS; ++j){
		state -> registers[j] = 0;
	}
	for(j=0; j < SIZE_OF_MEMORY; ++j){
		state -> memory[j] = 0;
	}
		
	for(j = 0; j < ninstructions; j++){
	    set_memory(state, j*4, instructions[j]);
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

    int result = SUCCESS;
	while(result != HALT){
		int index = (get_memory(state, state->PC) & (M_OPCODE << 26)) >> 26;
		result = (*OpCodeToFunction[op_code(index)])
		            (state, get_memory(state, state -> PC));
	}

	return 0;
}


