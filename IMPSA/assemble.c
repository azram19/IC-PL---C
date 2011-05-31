#include<stdlib.h>
#include<stdio.h>
#include<string.h>

#define TYPE_NA 0
#define TYPE_J 1
#define TYPE_I 2
#define TYPE_R 3

#define SUCCESS 1
#define HALT 0
#define ERROR -1

char *outputPath;

struct map_node{
    char * key;
    int value;
    int colour;
    struct map_node * left;
    struct map_node * right;
    struct map_node * parent;
};


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

int rb_insert(struct map_node * root, char * key, struct map_node * node){
    //node -> colour = red;
    
    return SUCCESS;
}

int map_get(struct map_node * root, char * key){
    if(root -> key == NULL){
        return NULL;
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
    return SUCCESS;
}

int op_to_type(int op_code){
    int op_type[18] = {TYPE_NA, TYPE_R, TYPE_I, TYPE_R, TYPE_I, TYPE_R, TYPE_I, TYPE_I, TYPE_I, TYPE_I, TYPE_I, TYPE_I, TYPE_I, TYPE_I, TYPE_I, TYPE_J, TYPE_R, TYPE_J };
    return op_type[op_code];
}

//❤        L S  .  .  . f  r  o  m         h   e  r   e       ❤



struct command{
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
	int r3;
	int r2;

	int constantValue;
	char labelValue[];
};



int main(int argc, char *argv[]){
	if(argc!=2){
		printf("usage: %s filename", argv[0]);
	}
	else{
     		FILE *sFile;
			sFile = fopen(argv[1],"r");

			if(sFile==NULL){
				perror("error opening %s!", argv[1]);
				return 1;
			}
			else{
			 	outputPath=argv[2];
//❤        L S  .  .  . e   n   d                              ❤
				chuj();



			}

	}
    
    //Testing tree
    struct map_node map;
    
    map.left = NULL;
    map.right = NULL;

    map_put(&map, "pies", 4);
    printf("%d\n", map_get(&map, "pies"));
    
    return 0;
}
