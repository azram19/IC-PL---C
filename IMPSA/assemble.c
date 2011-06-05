#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<ctype.h>

#define TYPE_NA 4
#define TYPE_J 1
#define TYPE_I 2
#define TYPE_R 3
#define TYPE_S 5

#define SUCCESS 1
#define HALT 0
#define ERROR -1

char str[80]; //Wulgarne, paskudne, nie potrafie inaczej.

char *outputPath;

struct map_node {
	char * key;
	int value;
	int colour;
	struct map_node * left;
	struct map_node * right;
};

struct map_node * op_codes_tree = NULL;

int map_put(struct map_node * root, char * key, int value) {
	struct map_node * node_ptr = malloc(sizeof(struct map_node));

	node_ptr -> key = key;
	node_ptr -> value = value;
	node_ptr -> left = NULL;
	node_ptr -> right = NULL;

	tree_insert(root, key, node_ptr);
	return SUCCESS;
}

int tree_insert(struct map_node * root, char * key, struct map_node * node) {
	if (root -> key == NULL) {
		root -> key = node -> key;
		root -> value = node -> value;
	} else if (strcmp(root -> key, key) > 0) {
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

int map_get(struct map_node * root, char * key) {
	if (root == NULL || root -> key == NULL) {
		return ERROR;
	}

    int compare = strcmp(root -> key, key);

	if (compare == 0) {
		return root -> value;
	} else if (compare > 0) {
		return map_get(root -> right, key);
	} else {
		return map_get(root -> left, key);
	}
}

int op_char_to_int(char * op_code) {
	return map_get(op_codes_tree, op_code);
}

int op_to_type(int op_code) {
	int op_type[20] = { TYPE_NA, TYPE_R, TYPE_I, TYPE_R, TYPE_I, TYPE_R,
			TYPE_I, TYPE_I, TYPE_I, TYPE_I, TYPE_I, TYPE_I, TYPE_I, TYPE_I,
			TYPE_I, TYPE_J, TYPE_R, TYPE_J, TYPE_S, TYPE_S };
	return op_type[op_code];
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
 * Translates code line into struct token, and then returns it
 *
 * @author Lukasz Kmiecik <moa.1991@gmail.com>
 */
struct command readToken() {
	struct command *token;
	token = (struct command *)malloc(1*sizeof(struct command));

	int registersNumber;
	int i;


	char delims[] = " \t";
	char * tokenField;

	tokenField = strtok(str, delims);
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
		tokenField = strtok(NULL, delims);
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


	//jump to next
	tokenField = strtok(NULL, delims);

	//and now to checking...

	if (registersNumber == 1)
		registersNumber--;

	if (registersNumber == 3) {
		//scan for 3 registers
		//		if ((tokenField[0] == "$"))
		token->r1 = reg_char_to_int(tokenField);
		tokenField = strtok(NULL, delims);
		token->r2 = reg_char_to_int(tokenField);
		tokenField = strtok(NULL, delims);
		token->r3 = reg_char_to_int(tokenField);

		//thats it, add and GTFO
	} else if (registersNumber == 2) {
		token->r1 = reg_char_to_int(tokenField);
		tokenField = strtok(NULL, delims);
		token->r2 = reg_char_to_int(tokenField);
		//next thing will be an immediatevalue/labelvalue

		//jump!
		tokenField = strtok(NULL, delims);
		if (isalpha(tokenField[0])) {
			//this is a label
			token -> labelValue = (char *) malloc(16 * sizeof(char));
			for (i = 0; i < 16; i++) {
				token->labelValue[i] = tokenField[i];
			}
		} else {
			//this is a constantvalue/address
			token->constantValue = atoi(tokenField);
		}

	} else if (registersNumber == 0){
		if (isalpha(tokenField[0])) {
			//this is a label
			token -> labelValue = (char *) malloc(16 * sizeof(char));
			for (i = 0; i < 16; i++) {
				token->labelValue[i] = tokenField[i];
			}
		} else {
			//this is a constantvalue/address
			token->constantValue = atoi(tokenField);
		}
	}

	//now we have a complete token.
	return *token;
}

//-------------PB

/*
 * I don't like this function's name :P, and I moved the creation of a labelTree `up`. LK
 */
void assemblerPass1(struct map_node * labelTree, struct command *commandArray, int size){ 
	int i;
	for(i = 0; i < size; i++){
		if(commandArray[i].label != NULL){
			map_put(labelTree, commandArray[i].label, 4*i);
		}
	}
}

int * assemblerPass2(struct map_node *labelTree, struct command *commandArray, int size){
	int *bitArray = (int *)malloc(size*sizeof(int));
	int i;
	for(i = 0; i < size; i++){
	    replace_label(labelTree, &commandArray[i]);
	    bitArray[i] = binary_converter(&commandArray[i], i);
    }
	return bitArray;
}

void binarywriter(char *filename, int *instructions, int ninstructions){
	FILE *fileptr = fopen(filename, "wb");
	
	/*
	 * Converting from big to little endian.
	 */
	int i;
	for(i = 0; i < ninstructions; i++){
	    instructions[i] = betole(instructions[i]);
	}
	
	fwrite(instructions, sizeof(instructions[0]), ninstructions, fileptr);
	fclose(fileptr);
}

//-------------PB

/*
 * Converts srtuct into binary represantation; instruction.
 *
 * @author Lukasz Koprowski <azram19@gmail.com>
 */
int binary_converter(struct command * c, int i){
    int instr = 0;
    
    instr |= (c -> opcode << 26);
    if(c -> type == TYPE_R){
        instr |= (c -> r1 << 21);
        instr |= (c -> r2 << 16);
        instr |= (c -> r3 << 22);
    } else if(c -> type == TYPE_I){
        instr |= (c -> r1 << 21);
        instr |= (c -> r2 << 16);
        
        if(c -> opcode <= 14 && c -> opcode >= 9){
            instr |= (c -> constantValue - (i<<2)) >> 2;
        } else {
            instr |= c -> constantValue;
        }
    } else {
        instr |= c -> constantValue;
    }
    printf("%#x\n", instr);
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
	op_codes_tree = (struct map_node * ) malloc(sizeof(struct map_node));
    struct map_node ** nodes_table = (struct map_node **) malloc(sizeof(struct map_node * ));

	op_codes_tree -> key = NULL;
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
		//	const char EOC = '-'; //end of code;

	//		char lineBuffer[80]; //kurwa paskudne strasznie no ale potrzebuje stringa.
	//		nieaktualne, wymyslilem cos paskudniejszego, GLOBALNA, STATYCZNA TABLICA, FUCK YEAH
			int i=0;
			int j;
			int line=0;
//*******************************************************************************************************
//**********************************************RATUNKU**************************************************
			struct command commandArray[100];
//*******************************************************************************************************
//*******************************************************************************************************
			int nonempty=0;
			while ((x = fgetc(inputFile)) != EOF) {
				//read one line
                
//				if (x == EOL || x == EOC) {
				if (x == EOL) {
					//fist we have to check if the line is empty, if it is, fuck passing.
					for(j=i; j>0; j--){
						if(str[j]!=' '||str[j]!="\n"||str[j]!="\t"){
							nonempty=1;
						}
					}

					//pass the line to tokeniser
					//readToken(str);

					//pass the token to the command Array.
					if(nonempty)
					commandArray[line]=readToken();

					//empty the buffer
					for (j=i; j > 0; j--) {
						str[j] = '\0';
					}
					nonempty=0;
					i=0;
					line++;
				} else {
					str[i] = (char) x;
					i++;
				}
			}
            
			fclose(inputFile);
			//❤        L S  .  .  . e   n   d                              ❤

                        //-----------PB

    		struct map_node * labelTree = (struct map_node *)malloc(sizeof(struct map_node)); 	
			assemblerPass1(labelTree, commandArray, line);
			int *bitArray = assemblerPass2(labelTree, commandArray, line);
			binarywriter(outputPath, bitArray, line);		
					
			//-----------PB
		    //free(labelTree);
			//free(bitArray);
		}
	}
	
	return 0;
}
