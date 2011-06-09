#include<stdlib.h>
#include<stdio.h>

#ifndef ERROR
#include "error.h"
#endif

#include "tree.h"

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
 * Function frees the memory occupied by a binary search tree.
 *
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

