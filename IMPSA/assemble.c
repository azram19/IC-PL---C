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


struct map_node{
    char * key;
    int value;
    int colour;
    struct map_node * left;
    struct map_node * right;
    struct map_node * parent;
};

struct map_map{
    struct map_node * root;
};


struct map_node * op_codes_tree = NULL;
char *outputPath;
struct command *commandArray;

int map_put(struct map_node * root, char * key, int value){
    struct map_node node;
    struct map_node * node_ptr = &node;
    
    node.key = key;
    node.value = value;
    node.left = NULL;
    node.right = NULL; 
    
    tree_insert(root, key, node_ptr);
    //rb_insert(root, key, node_ptr);
    
    return SUCCESS;
}

int tree_insert(struct map_node * root, char * key, struct map_node * node){
    if(root -> key == NULL){
        root -> key = node -> key;
        root -> value = node -> value;
    } else if(strcmp(root -> key, key) > 0){
        if(root -> right == NULL){
            root -> right = node;
            node -> parent = node -> right;
        } else {
            tree_insert(root -> right, key, node);
        }
    } else {
        if(root -> left == NULL){
            root -> left = node;
            node -> parent = node -> left;
        } else {
            tree_insert(root -> left, key, node);
        }
    }
    
    return SUCCESS;
}

void tree_rotate_right(struct map_map * map){
    struct map_node * root = map -> root
    map -> root = map -> root -> left;
    root -> left = map -> root -> right;
    map -> root -> right = root; 
}

void tree_rotate_left(struct map_map * map){
    struct map_node * root = map -> root;
    map -> root = map -> root -> right;
    root -> right = map -> root -> left;
    map -> root -> left = root;
}

int rb_insert(struct map_node * root, char * key, struct map_node * node){
    //node -> colour = red;
    rb_insert_case1(struct map_node * node)
    return SUCCESS;
}

int rb_insert_case1(struct map_node * node){
    if(node -> parent == NULL){
        node -> colour = 1
    } else {
        rb_insert_case2(node);
    }
}
int rb_insert_case2(struct map_node * node){
    if(node -> parent -> color == 1){
        return;
    } else {
        rb_insert_case3(node);
    }
}
int rb_insert_case3(struct map_node * node){
    struct map_node * u = uncle(node), * g;
    
    if( u != NULL && u -> colour == 2){
        node -> parent -> colour = 1;
        u -> colour = 1;
        g = grandparent(node);
        g -> colour = 2;
    } else {
        insert_case4(node);
    }
}
int rb_insert_case4(struct map_node * node){
    struct map_node * g = grandparent;
    struct map_map m;
    struct map_map * m_ptr = &m;
    m.root = node -> parent;
    //TODO
}
int rb_insert_case5(struct map_node * node){

}

struct map_node * grandparent(struct map_node * node){
    if(node != NULL && node -> parent != NULL){
        return node -> parent -> parent;
    } else {
        return NULL;
    }
}

struct map_node * uncle(struct map_node * node){
    struct map_node * g = grandparent(node);
    
    if(g == NULL){
        return NULL;
    } else if(node -> parent == g -> left){
        return g -> right;
    } else {
        return g -> left;
    }
}

int map_get(struct map_node * root, char * key){
    if(root -> key == NULL){
        return ERROR;
    }
    
    if(strcmp(root -> key, key) == 0){
        return root -> value;
    } else if(strcmp(root -> key, key) > 0){
        return map_get(root -> left, key);
    } else {
        return map_get(root -> right, key);
    }
}

int op_char_to_int(char * op_code){
    return map_get(op_codes_tree, op_code);
}

int op_to_type(int op_code) {
	int op_type[18] = { TYPE_NA, TYPE_R, TYPE_I, TYPE_R, TYPE_I, TYPE_R,
			TYPE_I, TYPE_I, TYPE_I, TYPE_I, TYPE_I, TYPE_I, TYPE_I, TYPE_I,
			TYPE_I, TYPE_J, TYPE_R, TYPE_J };
	return op_type[op_code];
}

//❤        L S  .  .  . f  r  o  m         h   e  r   e       ❤


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
    
	if (argc != 2) {
		printf("usage: %s filename", argv[0]);
	} else {
		FILE *sFile;
		sFile = fopen(argv[1], "r");

		if (sFile == NULL) {
			perror("error opening %s!", argv[1]);
			return 1;
		} else {
			outputPath = argv[2];
			int x;

			/*Zjebane, trzeba:
			 *
			 * Brac linijka po linijce, ladowac do zmodyfikowanej funkcji readToken ktora bierze stringa jako argument
			 * Do zrobienia 1.06.2011
			 *
			 * imsorrythankyou
			 *
			 */



			while ((x = fgetc(inputFile)) != EOF) {
				//start reading file
			}

			fclose(file);
			//❤        L S  .  .  . e   n   d                              ❤

		}
    }

	return 0;
}

void readToken(FILE *inputFile) {
	int x;
	char *inputString;
	int i = 0;
	struct command token;

	if ((x = fgetc(inputFile)) != EOL) {
		//look for label/or maybe opcode?
		if (isalpha((char)x)) {
			while ((x = fgetc(inputFile)) != EOL && (char) x != " ") {
				if (((char) x) == ":") {
					token.label=inputString;

					//reset input string, and counter
					inputString=NULL;
					i=0;
					//waiting for next token
				}
				inputString[i] = x;
				i++;
			}
		}

	}

	commandArray=token;
	commandArray++;

}

