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

#define ERR_REPEATED_LABEL 1
#define ERR_CANT_OPEN_FILE 2

char delims[] = " \t";


char *outputPath;

struct map_node {
	long long int key;
	int value;
	struct map_node * left;
	struct map_node * right;
};

struct map_node * op_codes_tree = NULL;

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
    long long int pow = p;
    
    while(s[i] != '\0'){
        h += s[i] * pow;
        pow *= p;
        i++;
    }
    
    return (h == -1) ? 1 : h;
}

/*
 * Inserts element into the map.
 * 
 * @author Lukasz Koprowski <azram19@gmail.com>
 */
int map_put(struct map_node * root, char * key, int value) {
    if(key[0] == '\0') return SUCCESS;

	struct map_node * node = malloc(sizeof(struct map_node));

	node -> key = get_hashcode(key);
	node -> value = value;
	node -> left = NULL;
	node -> right = NULL;

	tree_insert(root, node -> key, node);
	return SUCCESS;
}

int freeTheTree(struct map_node * root){
	if(root==NULL) {
		return SUCCESS;
	}
	freeTheTree(root->left);	
	freeTheTree(root->right);
	free(root);
	return SUCCESS;
}

/*
 * Inserts element into the map.
 * 
 * @author Lukasz Koprowski <azram19@gmail.com>
 */
int tree_insert(struct map_node * root, long long int key, struct map_node * node) {
	if (root -> key == EMPTY_KEY) {
		root -> key = key;
		root -> value = node -> value;
	} else if (root -> key > key) {
		if (root -> right == NULL) {
			root -> right = node;
		} else {
			tree_insert(root -> right, key, node);
		}
	} else {
		if (root -> left == NULL) {
			root -> left = node;
		} else {
			tree_insert(root -> left, key, node);
		}
	}
    
	return SUCCESS;
}

/*
 * Finds element in the map.
 * 
 * @author Lukasz Koprowski <azram19@gmail.com>
 * @return Either value assigned to the key, or an EMPTY_KEY.
 */
int map_get(struct map_node * root, char * key) {
	if (root == NULL || root -> key == EMPTY_KEY) {
		return ERROR;
	}

    long long int hash = get_hashcode(key);
    
    if (root -> key == hash) {
		return root -> value;
	} else if (root -> key > hash) {
		return map_get(root -> right, key);
	} else {
		return map_get(root -> left, key);
	}
}


/*
 * Returns OpCode assigned to the given instruction.
 *
 * @author Lukasz Koprowski <azram19@gmail.com>
 * @return OpCode
 */
int op_char_to_int(char * op_code) {
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

int error(int error_code){
    
    switch(error_code){
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




//❤        L S  .  .  . f  r  o  m         h   e  r   e       ❤

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
 * @modified_by Agnieszka Szefer <agnieszka.m.szefer@gmail.com>
 */
void Rtype(char * str, struct command *token){	
	char * tokenField;
	char * rest;
	tokenField = strtok_r(str, delims, &rest);
	str = rest;
	token->r1 = reg_char_to_int(tokenField); 
	tokenField = strtok_r(str, delims, &rest);
	str = rest;
	token->r2 = reg_char_to_int(tokenField);
	tokenField = strtok_r(str, delims, &rest);
	str = rest;
	token->r3 = reg_char_to_int(tokenField);	
}

/*
 * Parser for I-type instructions
 *
 * @author Lukasz Kmiecik <moa.1991@gmail.com>
 * @modified_by Agnieszka Szefer <agnieszka.m.szefer@gmail.com>
 */
void Itype(char * str, struct command *token){	
	int i;
	char *tokenField;
	char *rest;
	tokenField = strtok_r(str, delims, &rest);
	str = rest;
	token->r1 = reg_char_to_int(tokenField);
	tokenField = strtok_r(str, delims, &rest);
	str = rest;
	token->r2 = reg_char_to_int(tokenField);
	tokenField = strtok_r(str, delims, &rest); 
	str = rest;
	if(isalpha(tokenField[0])){	// tokenField is a label
		token->labelValue = (char *) malloc(16 * sizeof(char));
		for(i=0; i < 16; ++i) token->labelValue[i] = tokenField[i];
	}
	else {
		if(tokenField[0] == '0' && tokenField[1] == 'x'){ // tokenField is a hex 
			token->constantValue = strtol (tokenField, NULL, 0);
			for(i=0; i < 16; ++i) token->labelValue[i] = tokenField[i];
		}
		else token->constantValue = atoi(tokenField); // tokenField is an integer
	}
}

/*
 * Parser for J-type and S-type instructions
 *
 * @author Lukasz Kmiecik <moa.1991@gmail.com>
 * @modified_by Agnieszka Szefer <agnieszka.m.szefer@gmail.com>
 */
void JorStype(char * str, struct command *token){	
	int i;
	char *tokenField;
	char *rest;
	tokenField = strtok_r(str, delims, &rest);
	str = rest;
	if (isalpha(tokenField[0])){ // tokenField is a label
		token -> labelValue = (char *) malloc(16 * sizeof(char));
		for (i = 0; i < 16; i++) token->labelValue[i] = tokenField[i];
	} else {
		//it may be in int format, or hex format
			if(tokenField[0] == '0' && tokenField[1] == 'x')
				token->constantValue = strtol (tokenField,NULL,0);
			else token->constantValue = atoi(tokenField);
		}
}

/*
 * Translates code line into struct token, and then returns it
 *
 * @author Lukasz Kmiecik <moa.1991@gmail.com>
 */
struct command readToken(char * str) {
	struct command *token;
	token = (struct command *)malloc(1 *sizeof(struct command));

	char *rest;
	int registersNumber;
	int i;

	char * tokenField;

	tokenField = strtok_r(str, delims, &rest);
	str = rest;
	//first thing is either label, or opcode

	int lastCharIndex=(strlen(tokenField))-1;
	char lastCharacter=tokenField[lastCharIndex];
	//if ((tokenField[(strlen(tokenField)) - 1]) == ":") {
	if(lastCharacter==':'){
		//we have a label;
		for (i = 0; i < lastCharIndex; i++) {
			if(tokenField[i]==':') break;
			token->label[i] = tokenField[i];
		}
		//jump straight to next token
		tokenField = strtok_r(str, delims, &rest);
		str = rest;
	}
	//next thing HAS TO BE an opcode

//	token->opcode = 15;
//	token->type = 2;

	token -> opcode = op_char_to_int(tokenField);
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

	registersNumber = token->type;

	//and now to checking...

	if (registersNumber==1)
		registersNumber--;


	if (registersNumber == 3) Rtype(str, token);
	else if (registersNumber == 2) Itype(str, token);
	else if (registersNumber == 0 || registersNumber == 5) JorStype(str, token);

	//now we have a complete token.
	return *token;
}

//-------------PB

/*
 * I don't like this function's name :P, and I moved the creation of a labelTree `up`. LK
 */
void assemblerPass1(struct map_node * labelTree, struct command * commandArray, int size){ 
	int i;
	for(i = 0; i < size; i++){
		if(commandArray[i].label != NULL){
			if(map_get(labelTree, commandArray[i].label) != ERROR){
				error(ERR_REPEATED_LABEL);
			}else{
				map_put(labelTree, commandArray[i].label, 4*i);
			}
		}
	}
}

int * assemblerPass2(struct map_node * labelTree, struct command * commandArray, int * size){
	int * bitArray = (int *)malloc((*size) * sizeof(int));
	int i, j, nba = bitArray;
	for(i = 0, j =0; i < (*size); i++, j++){
	    replace_label(labelTree, &commandArray[j]);
	    bitArray[i] = binary_converter(&commandArray[j], &i, size, bitArray, &nba);
	    if(nba != bitArray){
	        bitArray = nba;
	    }
    }
	return bitArray;
}

void binarywriter(char *filename, int *instructions, int ninstructions){
	FILE *fileptr = fopen(filename, "wb");
	
	fwrite(instructions, sizeof(instructions[0]), ninstructions, fileptr);
	fclose(fileptr);
}

//-------------PB
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
int binary_converter(struct command * c, int * i, int * size, int * ba, int * nba){
    int instr = 0;
    
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
            (*size) += (c -> constantValue - 1);
            (*i) += (c -> constantValue - 1);
            nba = realloc(ba, (*size) * sizeof(int));
            int i;
            for(i = (*size) - (c -> constantValue); i < (*size); i++ ){
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
int replace_label(struct map_node * labels, struct command * c){
    int addr = 0;
    
    if(c -> labelValue != NULL){
        addr = map_get(labels, c -> labelValue);
        if(addr == ERROR){
            return ERROR;
        } else{
            c -> constantValue = addr;
        }
    }
    
    return SUCCESS;
}

/*
 * Converts big endian number to little endian.
 *
 * @author Lukasz Koprowski <azram19@gmail.com>
 */
int betole(int b){
    int l = 0;
    int i;
    
    for(i = 0; i < 4; i++){
        l <<= 8;
        l |= b%(1 << 7);
        b /= (1 << 7);
    }
    
    return l;
}

int main(int argc, char *argv[]) {
	char str[256]; //Wulgarne, paskudne, nie potrafie inaczej.
	op_codes_tree = (struct map_node * ) malloc(sizeof(struct map_node));

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
    
	if (argc != 3) {
		printf("usage: %s filename", argv[0]);
	} else {
		FILE *inputFile;
		inputFile = fopen(argv[1], "r");

		if (inputFile == NULL) {
		//	perror("error opening %s!", argv[1]);
			return 1;
		} else {
			//SUCCESS - FILE OPENED
			outputPath = argv[2];

			int x;
			const char EOL = '\n';

			int i=0;
			int j;
			int line=0;

			struct command * commandArray = NULL;

			int nonempty=0;
			int number_of_commands = 0;
			
			while ((x = fgetc(inputFile)) != EOF) {
			    if(x == '\n') number_of_commands++;
			}
			rewind(inputFile);		
			commandArray = (struct command *) malloc(number_of_commands * sizeof(struct command));
			
			while ((x = fgetc(inputFile)) != EOF) {
				//read one line
                
				if (x == EOL) {
					//fist we have to check if the line is empty, if it is, fuck passing.
					for(j = i; j > 0; j--){
						if(str[j] != ' ' || str[j] != '\n' || str[j] != '\t'){
							nonempty = 1;
						}
					}

					//pass the token to the command Array.
					if(nonempty){ 
					    commandArray[line] = readToken(str);
                        line++;
                    } else {
                        number_of_commands--; //line of code is empty
                    }
					//empty the buffer
					memset(str, '\0' ,sizeof(str));
					
					nonempty=0;
					i=0;
				} else {
					str[i] = (char) x;
					i++;
				}
			}
            
			fclose(inputFile);
			//❤        L S  .  .  . e   n   d                              ❤

                        //-----------PB

    		struct map_node * labelTree = (struct map_node *)malloc(sizeof(struct map_node)); 	
    		
    		labelTree -> key = EMPTY_KEY;
	        labelTree -> left = NULL;
	        labelTree -> right = NULL;
    		
			assemblerPass1(labelTree, commandArray, number_of_commands);
			int * bitArray = assemblerPass2(labelTree, commandArray, &number_of_commands);
			binarywriter(outputPath, bitArray, number_of_commands);		
					
			
			//-----------PB
			free(bitArray);
			free(commandArray);

			//Functions to do:
		    	freeTheTree(labelTree); 
			freeTheTree(op_codes_tree); 
		}
	}
	
	return 0;
}

/*
struct command readToken() {
	struct command *token;
	token = (struct command *)malloc(1*sizeof(struct command));

	return *token;
}

void reader(char *filename, char **instructions, int number_of_commands, int msize){
	FILE *fileptr = fopen(filename, "r");
	instructions = createcommands(number_of_commands);
	int i;
	assert(commands!=NULL);
	for(i=0; i<number_of_commands; i++){
		commands[i] = getcommand(msize, fileptr);
	}
	fclose(fileptr);
	return commands;
}

int arraysize(char *filename){
	int number_of_commands = 0;
	FILE *fileptr = fopen(filename, "r");
	if (fileptr == NULL) {
	    error_file(ERR_CANT_OPEN_FILE, filename);
		return ERROR;
	}
	while ((x = fgetc(fileptr)) != EOF) {
			    if(x == '\n') number_of_commands++;
			}
	rewind(fileptr);
	fclose(fileptr);
	return number_of_commands;
}

char * createcommand(int msize){
	char *cptr;
	cptr = (char *)malloc(msize*sizeof(char));
	if(cptr==NULL){
		perror("malloc");
		return(NULL);
	}
	return cptr;
}

char ** createcommands(int number_of_commands){
	int i;
	char ** cptr = (char **)malloc(number_of_commands*sizeof(char *));
	if(cptr==NULL){
		perror("malloc");
		return(NULL);
	}
	for(i = 0; i<number_of_commands; i++){
		cptr[i] = NULL;
	}
	return cptr;
}

char * getcommand(int msize, FILE *fileptr){
	char * command = createcommand(msize);
	assert(command!=NULL);
	fgets (command, msize, fileptr);
	return command;
}

int main(int argc, char *argv[]){
	char filename[100];
	strcpy(filename,argv[1]);
	int msize = 256;
	int number_of_commands = arraysize(filename);
	struct command *commandArray = (struct command *) malloc(number_of_commands * sizeof(struct command));
	char **instructions = NULL;
	instructions = reader(filename, instructions, number_of_commands, msize);
	return 0;
}

*/
