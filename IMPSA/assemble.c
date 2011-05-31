#include<stdlib.h>
#include<stdio.h>
#include<string.h>

struct map_node{
    char * key;
    int value;
    map_node * left;
    map_node * right;
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


				chuj();



			}

	}


}
