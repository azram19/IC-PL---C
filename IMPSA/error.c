#include<stdio.h>
#include<stdlib.h>
#include "error.h"

/*
 * Handles error codes.
 *
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


