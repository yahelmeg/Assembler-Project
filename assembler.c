/* main file , calls a funcion to expand macro for each argument(file), if it's valid it starts the process of making the machine code by calling the */
/* first transition function.																														 */
	

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include "macroProcedure.c"
#include "firstTransition.c"

int startMacroProcedure(char filename[]);    /* starts macro expansion procedure */
int firstTransition(char filename[],int code[],int data[]); /* first transition */

int main( int argc, char *argv[] ) {
	int i;
	char file[MAX_FILE_NAME_LEN];
	int code[MAX_WORDS] = {0};
	int data[MAX_WORDS] = {0};
	if(argc == 1) {
		printf("Error, no file name.\n");
		return 0;
	}
	for(i = 1; i<=argc-1; i++) {					 
		startMacroProcedure(argv[i]);  /* macro expansion for each argument(file) */
	}
	for(i = 1; i<=argc-1; i++) {     /* starts the transitions for each argument(file) */                  
		strcpy(file,argv[i]);
		strcat(file, ".am");
		if( access( file, F_OK ) == 0 ) {
    		firstTransition(argv[i], code , data);
		}  
	}
	return 0;
}
