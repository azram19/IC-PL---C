#define EMPTY_KEY -1
#define TREE :)

#define SUCCESS 1
#define EMPTY_KEY -1

typedef struct map_node snode;

struct map_node {
	long long int key;
	int value;
	snode * left;
	snode * right;
};

long long int get_hashcode(char * s);

snode * map_find(snode *, long long int);
int map_put(snode * root, char * key, int value);
int map_get(snode * root, char * key);
int freeTheTree(snode * root);
