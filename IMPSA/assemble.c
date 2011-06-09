#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<ctype.h>

#define TYPE_NA 4
#define TYPE_J 1
#define TYPE_I 2
#define TYPE_R 3
#define TYPE_S 5

#define EMPTY_KEY -1

#define SUCCESS 1
#define HALT 0
#define ERROR -1

#define NUMBER_OF_INSTRUCTIONS 20
#define MAX_NUMBER_OF_REGISTERS 32

#define ERR_UNDEFINED_OPERATION 1
#define ERR_ILLEGAL_MEMORY_ACCESS 2
#define ERR_WRONG_REGISTER 3
#define ERR_NOT_ENOUGH_MEMORY 4
#define ERR_REPEATED_LABEL 5
#define ERR_CANT_OPEN_FILE 6
#define ERR_WRONG_NUM_OF_ARGS 7

char delims[] = " \t";

typedef struct map_node snode;
typedef struct command scommand;

snode * map_find(snode *, long long int);

struct map_node {
	long long int key;
	int value;
	snode * left;
	snode * right;
};


/*
 * Generates hashcode for the given string.
 * 
 * @author Lukasz Koprowski <azram19@gmail.com>
 * @return Hashcode
 */
long long int get_hashcode(char * s){
    long long int h = 0;
    int i = 0;
    int p = 101;
    long long int pow = 1;
    
    while(s[i] != '\0'){
        h += s[i++] * (pow *= p);
    }
    
    return (h == -1) ? 1 : h;
}

/*
 * Inserts element into the map.
 * 
 * @author Lukasz Koprowski <azram19@gmail.com>
 */
int map_put(snode * root, char * key, int value) {
    if(key[0] == '\0') return SUCCESS;

	snode * node = malloc(sizeof(snode));
	if(node == NULL){
	    error(ERR_NOT_ENOUGH_MEMORY);
		return ERROR;	
	}

	node -> key = get_hashcode(key);
	node -> value = value;
	node -> left = NULL;
	node -> right = NULL;

	snode * fnode = map_find(root, node -> key);
	
	if (fnode -> key == EMPTY_KEY) {
		fnode -> key = node -> key;
		fnode -> value = node -> value;
		free(node);
	} else if (fnode -> key > node -> key) {
		fnode -> right = node;
	} else {
		fnode -> left = node;
	}
	
	return SUCCESS;
}

/*
 * @author Piotr Bar
 */
int freeTheTree(snode * root){
	if(root == NULL) {
		return SUCCESS;
	}
	freeTheTree(root->left);	
	freeTheTree(root->right);
	free(root);
	return SUCCESS;
}

/*
 * Finds value of an element in the map.
 * 
 * @author Lukasz Koprowski <azram19@gmail.com>
 * @return Either value assigned to the key, or an EMPTY_KEY.
 */
int map_get(snode * root, char * key) {
    long long int hash = get_hashcode(key);
    
    snode * fnode = map_find(root, hash);
    
    if(fnode != NULL && fnode -> key == hash){
        return fnode -> value;
    } else {
        return ERROR;
    }
}

/*
 * Finds node in the map.
 * 
 * @author Lukasz Koprowski <azram19@gmail.com>
 * @return Either node ot a parent it'd have.
 */
snode * map_find(snode * root, long long int key){
    if (root == NULL ) {
		return NULL;
	} else if (root -> key == key || root -> key == EMPTY_KEY) {
		return root;
	} else if (root -> key > key) {
	    if(root -> right == NULL) return root;
		return map_find(root -> right, key);
	} else {
	    if(root -> left == NULL) return root;
		return map_find(root -> left, key);
	}
}

/*
 * Returns OpCode assigned to the given instruction.
 *
 * @author Lukasz Koprowski <azram19@gmail.com>
 * @return OpCode
 */
int op_char_to_int(char * op_code, snode * op_codes_tree) {
	return map_get(op_codes_tree, op_code);
}

/*
 * Returns type of the instruction.
 *
 * @author Lukasz Koprowski <azram19@gmail.com>
 * @return Type
 */
int op_to_type(int op_code) {
	int op_type[20] = { TYPE_NA, TYPE_R, TYPE_I, TYPE_R, TYPE_I, TYPE_R,
			TYPE_I, TYPE_I, TYPE_I, TYPE_I, TYPE_I, TYPE_I, TYPE_I, TYPE_I,
			TYPE_I, TYPE_J, TYPE_R, TYPE_J, TYPE_S, TYPE_S };
	return op_type[op_code];
}

/*
 * @author Piotr Bar
 * @author Lukasz Koprowski
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
        case ERR_REPEATED_LABEL: {
                fprintf(stderr, 
                        "Error: There are two labels with the same name.\n");  
                break;
            }
        case ERR_CANT_OPEN_FILE: {
                fprintf(stderr, 
                        "Error: Can not open the file. Program terminated.\n");    
                break;
        }    
	case ERR_WRONG_NUM_OF_ARGS: {
		fprintf(stderr, 
                        "Error: The number of arguments must be 2\n");    
                break;
	    }      
    }
    printf("\n");
    exit(EXIT_FAILURE);
    return ERROR;
}

/*
 * @author Lukasz Koprowski
 */
int error_file(int error_code, char * filename){
    fprintf(stderr, "Problem with file: %s\n", filename); 
    error(error_code);
    return ERROR;
}


/*
 * Converts register symbol to integer
 *
 * @author Lukasz Kmiecik <moa.1991@gmail.com>
 * @return number of register
 */
int reg_char_to_int(char * reg) {
	reg++;
	return atoi(reg);
}

/*
 * Holds each code line in a struct
 *
 * @author Lukasz Kmiecik <moa.1991@gmail.com>
 */
struct command {
	//R (1) type function.
	//0-5 opcode  | 6 - 10 R1 | 11 - 15 R2 | 16 - 20 R3 | unused |

	//I (2) type function.
	//0-5 opcode  | 6 - 10 R1 | 11 - 15 R2 | 16 - 31 Immediate value

	//J (3) type function.
	//0-5 opcode  | 6 - 31 Address
	int type; 

	char label[16];
	int opcode;

	int r1;
	int r2;
	int r3;

	int constantValue;
	char * labelValue;
};

/*
 * Parser for R-type instructions
 *
 * @author Lukasz Kmiecik <moa.1991@gmail.com>
 * @author Agnieszka Szefer <agnieszka.m.szefer@gmail.com>
 */
void Rtype(char * str, scommand *token){	
	char * tokenField = NULL;
	tokenField = strtok_r(str, delims, &str);
	token -> r1 = reg_char_to_int(tokenField); 
	tokenField = strtok_r(str, delims, &str);
	token -> r2 = reg_char_to_int(tokenField);
	tokenField = strtok_r(str, delims, &str);
	token -> r3 = reg_char_to_int(tokenField);	
}

/*
 * Parser for I-type instructions
 *
 * @author Lukasz Kmiecik <moa.1991@gmail.com>
 * @author Agnieszka Szefer <agnieszka.m.szefer@gmail.com>
 */
void Itype(char * str, scommand *token){	
	char *tokenField = NULL;
	tokenField = strtok_r(str, delims, &str);
	token -> r1 = reg_char_to_int(tokenField);
	tokenField = strtok_r(str, delims, &str);
	token -> r2 = reg_char_to_int(tokenField);
	tokenField = strtok_r(str, delims, &str);
	if(isalpha(tokenField[0])){	// tokenField is a label
		token -> labelValue = (char *) malloc(16 * sizeof(char));
		strcpy(token -> labelValue, tokenField);
	}
	else {
		if(tokenField[0] == '0' && tokenField[1] == 'x'){ // tokenField is a hex
			token -> constantValue = strtol(tokenField, NULL, 0);
			token -> labelValue = (char *) malloc(16 * sizeof(char));
			strcpy(token -> labelValue, tokenField);
		}
		else token->constantValue = atoi(tokenField); // tokenField is an integer
	}
}

/*
 * Parser for J-type and S-type instructions
 *
 * @author Lukasz Kmiecik <moa.1991@gmail.com>
 * @author Agnieszka Szefer <agnieszka.m.szefer@gmail.com>
 */
void JorStype(char * str, scommand *token){	
	char *tokenField = NULL;
	tokenField = strtok_r(str, delims, &str);
	if (isalpha(tokenField[0])){ // tokenField is a label
		token -> labelValue = (char *) malloc(16 * sizeof(char));
		strcpy(token -> labelValue, tokenField);
	} else {
		//it may be in int format, or hex format
			if(tokenField[0] == '0' && tokenField[1] == 'x')
				token->constantValue = strtol (tokenField, NULL, 0);
			else token->constantValue = atoi(tokenField);
		}
}

/*
 * Translates code line into struct token, and then returns it
 *
 * @author Lukasz Kmiecik <moa.1991@gmail.com>
 * @author Agniszka Szefer
 */
scommand * readToken(char * str, snode * op_codes_tree) {
	scommand * token = NULL;
	token = (scommand *)malloc(sizeof(scommand));

	int i = 0;
	char * tokenField = NULL;

	tokenField = strtok_r(str, delims, &str);
	//first thing is either label, or opcode

	int lastCharIndex = strlen(tokenField) - 1;
	char lastCharacter = tokenField[lastCharIndex];
	if(lastCharacter==':'){
		//we have a label;
		for (i = 0; i < lastCharIndex; i++) {
			if(tokenField[i] == ':') break;
			token -> label[i] = tokenField[i];
			
		}
		//jump straight to next token
		tokenField = strtok_r(str, delims, &str);
	}
	//next thing HAS TO BE an opcode

	token -> opcode = op_char_to_int(tokenField, op_codes_tree);
	token -> type = op_to_type(token->opcode);
  	token -> labelValue = NULL;
    
	/*and now, all that is left is:
	 * R (3)  | 6 - 10 R1 | 11 - 15 R2 | 16 - 20 R3 | unused |
	 * I (2)  | 6 - 10 R1 | 11 - 15 R2 | 16 - 31 Immediate value
	 * J (1)  | 6 - 31 Address
	 *
	 * So, we need to scan for 0-3 registers
	 *
	 * So, then scan for address or immediate value
	 *
	 */

	if (token -> type == TYPE_R) Rtype(str, token);
	else if (token -> type == TYPE_I) Itype(str, token);
	else if (token -> type == TYPE_J || token -> type == TYPE_S){
        JorStype(str, token);
    }

	return token;
}

/*
 * @author Piotr Bar
 */
void assemblerPass1(snode * labelTree, scommand ** commandArray, int size){ 
	int i = 0;
	for(i = 0; i < size; i++){
		if(commandArray[i] -> label != NULL){
			if(map_get(labelTree, commandArray[i] -> label) != ERROR){
				error(ERR_REPEATED_LABEL);
			}else{
				map_put(labelTree, commandArray[i] -> label, 4*i);
			}
		}
	}
}

/*
 * @author Piotr Bar
 * @author Lukasz Koprowski
 */
int * assemblerPass2(snode * labelTree, scommand ** commandArray, int * size){
	int * bitArray = (int *)malloc((*size) * sizeof(int));
	if(bitArray == NULL){
		error(ERR_NOT_ENOUGH_MEMORY);	
	}
	int i = 0, j = 0, nba = 0;
	for(i = 0, j = 0; i < (*size); i++, j++){
	    nba = 0;
	    replace_label(labelTree, commandArray[j], size);
	    bitArray[i] = binary_converter(commandArray[j], &i, size, bitArray, &nba);
	    if(nba != 0){
	        bitArray = nba;
	    }
    }
	return bitArray;
}

/*
 * @author Piotr Bar
 */
void binarywriter(char * filename, int * instructions, int ninstructions){
	FILE *fileptr = fopen(filename, "wb");
	fwrite(instructions, sizeof(instructions[0]), ninstructions, fileptr);
	fclose(fileptr);
}

/* 
 * Reduces 32bit signed integer to 16 bit signed integer. 
 *
 * @author Lukasz Koprowski <azram19@gmail.com>
 * @return 16 bit signed integer
 */
int signed_reduction(int r){
    return (r & 0x80000000) ? r & (0x0000FFFF) : r;
}

/*
 * Converts struct into its binary represantation; instruction.
 *
 * @author Lukasz Koprowski <azram19@gmail.com>
 */
int binary_converter(scommand * c, int * i, int * size, int * ba, int * nba){
    int instr = 0;
    if(c -> opcode < 0 || c -> opcode >= NUMBER_OF_INSTRUCTIONS){
        error(ERR_UNDEFINED_OPERATION);
    }
    if(c -> r1 < 0 || c -> r1 >= MAX_NUMBER_OF_REGISTERS ||
	c -> r2 < 0 || c -> r2 >= MAX_NUMBER_OF_REGISTERS ||
	c -> r3 < 0 || c -> r3 >= MAX_NUMBER_OF_REGISTERS){
        error(ERR_WRONG_REGISTER);
    }
    if(c -> type != TYPE_S) instr |= (c -> opcode << 26);
    
    if(c -> type == TYPE_R){
        instr |= (c -> r1 << 21);
        instr |= (c -> r2 << 16);
        instr |= (c -> r3 << 11);
    } else if(c -> type == TYPE_I){
        instr |= (c -> r1 << 21);
        instr |= (c -> r2 << 16);
        if(c -> opcode <= 14 && c -> opcode >= 9){
            instr |= signed_reduction((c -> constantValue - ((*i)<<2)) >> 2);
        } else {
            instr |= signed_reduction(c -> constantValue);
        }
    } else if(c -> opcode == 19){
        /*
         * Increase size of an output array and reallocate memory.
        */
        if(c -> constantValue > 1){
            *size += (c -> constantValue - 1);
            *i += (c -> constantValue - 1);
            *nba = realloc(ba, *size * sizeof(int));
            int i = 0;
            for(i = *size - c -> constantValue; i < *size; i++){
                ba[i] = 0;
            }
        }
    } else {
        instr |= signed_reduction(c -> constantValue);
    }
    
    return instr;
}

/*
 * Replaced textual representation of labels to their addresses
 *
 * @author Lukasz Koprowski <azram19@gmail.com>
 */
int replace_label(snode * labels, scommand * c, int * size){
    int addr = 0;
    
    if(c -> labelValue != NULL){
        addr = map_get(labels, c -> labelValue);
        if(addr < -1 || addr > 4 * (*size - 1)){
        	error(ERR_ILLEGAL_MEMORY_ACCESS);
    	}

        if(addr == ERROR){
            return ERROR;
        } else{
            c -> constantValue = addr;
        }
    }
    
    return SUCCESS;
}

/*
 * @author Piotr Bar
 */
scommand **createCommandArray(int number_of_commands){
	scommand **commandArray = 
	    (scommand **) malloc(number_of_commands * sizeof(scommand *));
	
	if(commandArray == NULL){
		error(ERR_NOT_ENOUGH_MEMORY);	
	}
	return commandArray;
}


/*
 * @author Lukasz Koprowski
 */
snode * createLabelTree(){
	snode * labelTree = (snode *) malloc(sizeof(snode)); 
	if(labelTree == NULL){
		error(ERR_NOT_ENOUGH_MEMORY);	
	}	
   	labelTree -> key = EMPTY_KEY;
    labelTree -> left = NULL;
	labelTree -> right = NULL;
	return labelTree;
}

int op_prec(const char op){
    if(op == '*') return 3;
    if(op == '/') return 3;
    if(op == '+') return 2;
    if(op == '-') return 2;
    if(op == '%') return 3;
}

char * evaluate_expression(char * expr){
    char * output = malloc(32 * sizeof(char));
    char stack[16];
    char number[16];
    int number_end = 0;
    int stack_end = 0;
    int output_end = 0;
    int i = 0, j;
    
    memset(number, '\0', 16);
    
    while(expr[i] != '\0'){
        if(isdigit(expr[i])){
            number[number_end++] = expr[i];
        } else if(isspace(expr[i])){
            if(number_end == 0) {i++; continue;}
            
            for(j = 0; j < number_end; j++){
                output[output_end++] = number[j];
            }
            output[output_end++] = ' ';
            
            number_end = 0;
            memset(number, '\0', 16);
            
        } else if(expr[i] == '(') {
            stack[stack_end++] = '(';
        } else if(expr[i] == ')') {
            int j = stack_end;
            while(stack_end - 1 != '('){
                output[output_end++] = stack[--stack_end];
                output[output_end++] = ' ';
            }
            stack_end--;
        } else {
            int pre = op_prec(expr[i]);
            while(op_prec(stack[stack_end - 1]) >= pre){
                output[output_end++] = stack[--stack_end];
                output[output_end++] = ' ';
            }
            stack[stack_end++] = expr[i];
        }
        i++;
    }
    
    for(j = 0; j < number_end; j++){
        output[output_end++] = number[j];
    }
    output[output_end++] = ' ';
    
    while(stack_end > 0){
        output[output_end++] = stack[--stack_end];
        output[output_end++] = ' ';
    }
    
    return output;
}

int compute_RPN(char * expr){
    int stack[16];
    char number[16];
    int number_end = 0;
    int stack_end = 0;
    int i = 0, a, b;
    
    memset(number, '\0', 16);
    
    while(expr[i] != '\0'){
        if(isdigit(expr[i])){
            number[number_end++] = expr[i];
        } else if(isspace(expr[i])){
            if(number_end == 0){i++; continue;}
            stack[stack_end++] = atoi(number);
            number_end = 0;
            memset(number, '\0', 16);
        } else {
            a = stack[--stack_end];
            b = stack[--stack_end];
            
           
            if(expr[i] == '+'){
                stack[stack_end++] = b + a;
            } else if(expr[i] == '-'){
                stack[stack_end++] = b - a;
            } else if(expr[i] == '/'){
                stack[stack_end++] = b / a;
            } else if(expr[i] == '*'){
                stack[stack_end++] = b * a;
            } else if(expr[i] == '%'){
                stack[stack_end++] = b % a;
            }

        }
        i++;
    }
    
    free(expr);
    return stack[0];   
}

/*
 * @author Lukasz Koprowski
 */
snode * createOpcodeTree(){
	snode * op_codes_tree = (snode * ) malloc(sizeof(snode));
	if(op_codes_tree == NULL){
		error(ERR_NOT_ENOUGH_MEMORY);
	}
	
	op_codes_tree -> key = EMPTY_KEY;
	op_codes_tree -> left = NULL;
	op_codes_tree -> right = NULL;

	map_put(op_codes_tree, "halt", 0);
	map_put(op_codes_tree, "add", 1);
	map_put(op_codes_tree, "addi", 2);
	map_put(op_codes_tree, "sub", 3);
	map_put(op_codes_tree, "subi", 4);
	map_put(op_codes_tree, "mul", 5);
	map_put(op_codes_tree, "muli", 6);
	map_put(op_codes_tree, "lw", 7);
	map_put(op_codes_tree, "sw", 8);
	map_put(op_codes_tree, "beq", 9);
	map_put(op_codes_tree, "bne", 10);
	map_put(op_codes_tree, "blt", 11);
	map_put(op_codes_tree, "bgt", 12);
	map_put(op_codes_tree, "ble", 13);
	map_put(op_codes_tree, "bge", 14);
	map_put(op_codes_tree, "jmp", 15);
	map_put(op_codes_tree, "jr", 16);
	map_put(op_codes_tree, "jal", 17);
	map_put(op_codes_tree, ".fill", 18);
	map_put(op_codes_tree, ".skip", 19);
	
	return op_codes_tree;
}

int fileSize(FILE *inputFile){
	int number_of_commands=0;
	int x;
	while ((x = fgetc(inputFile)) != EOF) {
		if(x == '\n') number_of_commands++;
	}
	rewind(inputFile);
	return number_of_commands;
}

/*
 * @author Lukasz Kmiecik
 */
void tokenise(FILE *inputFile, scommand **commandArray, int * number_of_commands, snode * op_codes_tree){
	char str[256];
	int x;
	const char EOL = '\n';
	int i = 0;
	int j;
	int line = 0;
	int nonempty = 0;
	while ((x = fgetc(inputFile)) != EOF) {
	//read one line
		if (x == EOL) {
		//fist we have to check if the line is empty, if it is, fuck passing.
			for(j = i; j > 0; j--){
				if(!isspace(str[j])){
					nonempty = 1;
				}
			}
			//pass the token to the command Array.
			if(nonempty){ 
				commandArray[line] = readToken(str, op_codes_tree);
				line++;
			} else {
				(*number_of_commands) --; //line of code is empty
			}
			//empty the buffer
			memset(str, '\0', sizeof(str));
			nonempty = 0;
			i = 0;
		} else {
			str[i] = (char) x;
			i++;
		}
	}
}

/*
 * @author Piotr Bar
 * @author Lukasz Koprowski
 */
void freeCommandArray(scommand **commandArray, int array_length){
	int i = 0;
	for(i = 0; i < array_length; i++){
	    if(commandArray[i] != NULL) free(commandArray[i] -> labelValue);
	    free(commandArray[i]);
	}
	free(commandArray);
}

/*
 * @author Piotr Bar
 */
int main(int argc, char *argv[]) {
	if (argc != 3) {
		error(ERR_WRONG_NUM_OF_ARGS);
		return ERROR;
	}
	FILE *inputFile = NULL;
	inputFile = fopen(argv[1], "r");
	if (inputFile == NULL) {
	 	error_file(ERR_CANT_OPEN_FILE, argv[1]);
		return ERROR;
	}
	snode * op_codes_tree = createOpcodeTree(); 
	char *outputPath = argv[2];
	int number_of_commands = fileSize(inputFile);
	int noca_length = number_of_commands;
	scommand **commandArray = createCommandArray(number_of_commands);
	tokenise(inputFile, commandArray, &number_of_commands, op_codes_tree);
	fclose(inputFile);
	freeTheTree(op_codes_tree);
			
	snode * labelTree = createLabelTree();	
	assemblerPass1(labelTree, commandArray, number_of_commands);
	int * bitArray = assemblerPass2(labelTree, commandArray, &number_of_commands);
	binarywriter(outputPath, bitArray, number_of_commands);		
					
	free(bitArray);
	freeTheTree(labelTree); 
	freeCommandArray(commandArray, noca_length);
	return 0;
}
