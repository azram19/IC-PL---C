#include<stdlib.h>
#include<stdio.h>
#include<string.h>

struct map_node{
    char * key;
    int value;
    map_node * left;
    map_node * right;
};


char *outputPath;


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


//❤        L S  .  .  . f  r  o  m         h   e  r   e       ❤
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


}
