#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<ctype.h>

#define TYPE_NA 0
#define TYPE_J 1
#define TYPE_I 2
#define TYPE_R 3

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
	struct map_node * parent;
};

struct map_map {
	struct map_node * root;
};

struct map_node * op_codes_tree;


void rb_insert(struct map_node *, char *, struct map_node *);
void rb_insert_case1(struct map_node *);
void rb_insert_case2(struct map_node *);
void rb_insert_case3(struct map_node *);
void rb_insert_case4(struct map_node *);
void rb_insert_case5(struct map_node *);

int map_put(struct map_node * root, char * key, int value) {
	struct map_node node;
	struct map_node * node_ptr = &node;

	node_ptr -> key = key;
	node_ptr -> value = value;
	node_ptr -> left = NULL;
	node_ptr -> right = NULL;

	tree_insert(root, key, node_ptr);
	//rb_insert(root, key, node_ptr); KUUURRRWAAA!!

	return SUCCESS;
}

int tree_insert(struct map_node * root, char * key, struct map_node * node) {
	if (root -> key == NULL) {
		root -> key = node -> key;
		root -> value = node -> value;
	} else if (strcmp(root -> key, key) > 0) {
		if (root -> right == NULL) {
			root -> right = node;
			node -> parent = node -> right;
		} else {
			tree_insert(root -> right, key, node);
		}
	} else {
		if (root -> left == NULL) {
			root -> left = node;
			node -> parent = node -> left;
		} else {
			tree_insert(root -> left, key, node);
		}
	}

	return SUCCESS;
}

void tree_rotate_right(struct map_map * map) {
	struct map_node * root = map -> root;
	map -> root = map -> root -> left;
	root -> left = map -> root -> right;
	map -> root -> right = root;
}

void tree_rotate_left(struct map_map * map) {
	struct map_node * root = map -> root;
	map -> root = map -> root -> right;
	root -> right = map -> root -> left;
	map -> root -> left = root;
}


void rb_insert(struct map_node * root, char * key, struct map_node * node) {
    rb_insert_case1(node);
}

struct map_node * grandparent(struct map_node * node) {
	if (node != NULL && node -> parent != NULL) {
		return node -> parent -> parent;
	} else {
		return NULL;
	}
}

struct map_node * uncle(struct map_node * node) {
	struct map_node * g = grandparent(node);

	if (g == NULL) {
		return NULL;
	} else if (node -> parent == g -> left) {
		return g -> right;
	} else {
		return g -> left;
	}
}
void rb_insert_case1(struct map_node * node) {
	if (node -> parent == NULL) {
        node -> colour = 1;
    } else {
	    rb_insert_case2(node);
    }
}
void rb_insert_case2(struct map_node * node) {
	if (node -> parent -> colour == 1) {
		return;
	} else {
		rb_insert_case3(node);
	}
}
void rb_insert_case3(struct map_node * node) {
	struct map_node * u = uncle(node), *g;

	if (u != NULL && u -> colour == 2) {
		node -> parent -> colour = 1;
		u -> colour = 1;
		g = grandparent(node);
		g -> colour = 2;
	} else {
		rb_insert_case4(node);
	}
}
void rb_insert_case4(struct map_node * node) {
	struct map_node * g = grandparent(node);
	struct map_map m;
	struct map_map * m_ptr = &m;
	m.root = node -> parent;

 
    if ((node == node -> parent -> right) && (node -> parent == g -> left)) {
        tree_rotate_left(m_ptr);
        node = node -> left;
    } else if ((node == node -> parent -> left) && (node -> parent == g -> right)) {
        tree_rotate_right(m_ptr);
        node = node -> right;
    }
    rb_insert_case5(node);
}
void rb_insert_case5(struct map_node * node) {
    struct map_node * g = grandparent(node);
	struct map_map m;
	struct map_map * m_ptr = &m;
	m.root = g;
 
    node -> parent -> colour = 1;
    g -> colour = 2;
    if ((node == node -> parent -> left) && (node -> parent == g -> left)) {
        tree_rotate_right(m_ptr);
    } else {
        tree_rotate_left(m_ptr);
    }
}

int map_get(struct map_node * root, char * key) {
	if (root -> key == NULL) {
		return ERROR;
	}

	if (strcmp(root -> key, key) == 0) {
		return root -> value;
	} else if (strcmp(root -> key, key) > 0) {
		return map_get(root -> left, key);
	} else {
		return map_get(root -> right, key);
	}
}

int op_char_to_int(char * op_code) {
	return map_get(op_codes_tree, op_code);
}

int op_to_type(int op_code) {
	int op_type[18] = { TYPE_NA, TYPE_R, TYPE_I, TYPE_R, TYPE_I, TYPE_R,
			TYPE_I, TYPE_I, TYPE_I, TYPE_I, TYPE_I, TYPE_I, TYPE_I, TYPE_I,
			TYPE_I, TYPE_J, TYPE_R, TYPE_J };
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
	char labelValue[];
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


	char delims[] = " ,.-\t";
	char * tokenField;
	printf("Splitting line \"%s\" into tokens:\n", str);



	tokenField = strtok(str, delims);
	printf ("%s\n",tokenField);
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
		printf ("2nd %s\n",tokenField);
	}
	//next thing HAS TO BE an opcode
	token->opcode = op_char_to_int(tokenField);
	token->type = op_to_type(token->opcode);

//	token->opcode = 2;
//	token->type = 2;

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
		printf ("%s\n",tokenField);
		token->r2 = reg_char_to_int(tokenField);
		//next thing will be an immediatevalue/labelvalue

		//jump!
		tokenField = strtok(NULL, delims);
		printf ("%s\n",tokenField);
		if (isalpha(tokenField[0])) {
			//this is a label
			for (i = 0; i < 16; i++) {
				token->labelValue[i] = tokenField[i];
			}
		} else {
			//this is a constantvalue/address
			token->constantValue = atoi(tokenField);
		}

	} else if (registersNumber == 0) {
		if (isalpha(tokenField[0])) {
			//this is a label
			for (i = 0; i < 16; i++) {
				token->labelValue[i] = tokenField[i];
			}
		} else {
			//this is a constantvalue/address
			token->constantValue = atoi(tokenField);
		}
	}

	puts("puts działa");
	printf("a printf nie");
	
	printf("type: %d opcode: %d r1: $%d r2: $%d r3: %d immValu: %d immLabel %s",token->type,token->opcode,token->r1,token->r2,token->r3,token->constantValue,token->labelValue);

	//now we have a complete token.
	return *token;
}

//-------------PB

/*
 * I don't like this function's name :P, and I moved the creation of a labelTree `up`.
 */
void assemblerPass1(struct map_node * labelTree, struct command **commandArray, int size){ 
	int i;
	for(i = 0; i < size; i++){
		if(commandArray[i]->label != NULL){
			map_put(labelTree, commandArray[i]->label, 4*i);
		}
	}
}

int * assemblerPass2(struct command **commandArray, struct map_node *labelTree, struct map_node *op_codes_tree, int size){
	int *bitArray = (int *)malloc(size*sizeof(int));
	int i;
	for(i=0;i<size;i++){
		if(commandArray[i]->type==TYPE_J){
			bitArray[i]=(commandArray[i]->opcode << 26);
			if(commandArray[i]->labelValue!=NULL){
				bitArray[i] = bitArray[i] | (map_get(labelTree, commandArray[i]->labelValue));
			} else {
				bitArray[i] = bitArray[i] | commandArray[i]->constantValue;
			}
		} else if(commandArray[i]->type==TYPE_I){
			bitArray[i]=((commandArray[i]->opcode << 26) | (commandArray[i]->r1 << 21) | (commandArray[i]->r2 << 16));
			if(commandArray[i]->opcode<=14 && commandArray[i]->opcode>=9){
				if(commandArray[i]->labelValue!=NULL){
					bitArray[i] = bitArray[i] | ((map_get(labelTree, commandArray[i]->labelValue))-(i<<2))>>2;
				} else {
					bitArray[i] = bitArray[i] | commandArray[i]->constantValue;
				}
			} else if(commandArray[i]->labelValue!=NULL){
				bitArray[i] = bitArray[i] | (map_get(labelTree, commandArray[i]->labelValue));
			} else {
				bitArray[i] = bitArray[i] | commandArray[i]->constantValue;
			}
		} else if(commandArray[i]->type==TYPE_R){
			bitArray[i]=((commandArray[i]->opcode << 26) | (commandArray[i]->r1 << 21) | (commandArray[i]->r2 << 16) | (commandArray[i]->r2 << 11));
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
 * Converts srtuct into binary represantation; instruction.
 *
 * @author Lukasz Koprowski <azram19@gmail.com>
 */
int binary_converter(struct command c){
    int instr = 0;
    
    instr |= (c.opcode << 26);
    
    if(c.type == TYPE_J){
        instr |= c.constantValue;
    } else if(c.type == TYPE_R){
        instr |= (co.r1 << 21);
        instr |= (c.r2 << 16);
        instr |= (c.r3 << 22);
    } else if(c.type == TYPE_I){
        iinstr |= (c.r1 << 21);
        instr |= (c.r2 << 16);
        instr |= c.constantValue;
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
    
    if(c -> labelValue[0] != NULL && c -> constantValue == 0){
        addr = map_get(labels, c -> labelValue[0]);
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
        l << 8;
        l |= b%(1 << 7);
        b /= (1 << 7);
    }
    
    return l;
}

int main(int argc, char *argv[]) {
	struct map_node codes_tree;
	op_codes_tree = &codes_tree;

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
			const char EOC = '-'; //end of code;

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

			while ((x = fgetc(inputFile)) != EOF) {
				//read one line

				if (x == EOL || x == EOC) {
					//pass the line to tokeniser
					//readToken(str);

					//pass the token to the command Array.
					commandArray[line]=readToken();

					//empty the buffer
					for (j=i; j > 0; j--) {
						str[j] = '\0';
					}
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
			struct command **commandArrayptr = (struct command **)&commandArray[0];
			struct map_node * labelTree = assemblerPass1(commandArrayptr, line);
			int *bitArray = assemblerPass2(commandArrayptr, labelTree, op_codes_tree, line);
			char filename[100];
			strcpy(filename,argv[2]);
			binarywriter(filename, bitArray, line);		
			//-----------PB
		}
	}
	
	
	

    //TEST - map
    struct map_node map_tree;
	struct map_node * map = &map_tree;

	map -> key = NULL;
	map -> left = NULL;
	map -> right = NULL;

    map_put(map, "pies", 14);
    int m = map_get(map, "pies");

	printf("MAP %s : %d\n", "pies", m);

	return 0;
}
