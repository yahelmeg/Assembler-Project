/* creates the output files(.ob .ext .ent) for files that had no errors, starts with the code array and converts every line to the special base  */
/* after it the function does the same thing for the data array, after it the function checks if there are extern or entry labels */
/* if there are the function creates the matching output file */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "header.h"
#include "transitionsFunctions.c"


void createOutput(char filename[], int code[], int data[] , int ICF, int DCF , symbolTable *symboltable , externalsList *list ) {  /* creates .ob output file, also creates .ext and .ent files if required */ 
	FILE *fpw;  /* pointer to write to new file to create .ob file */
	char objectfile[MAX_FILE_NAME_LEN]; /* stores the name of the .ob file */
	char entfile[MAX_FILE_NAME_LEN]; /* stores the name of the .ent file */
	char extfile[MAX_FILE_NAME_LEN]; /* stores the name of the .ext file */
	int base;
	int offset;
	int value;
	int i = 100;
	int entryFlag = OFF;
	int codeLine = 100;
	int tempCode;
	label *temp;
	externalinfo *p; 
	temp = symboltable->start; /* pointer to the start of the symbol table , used to look for .entry labels in the table */
	p = list->start; /* pointer to the start of the externals list , used to print the .ext file if there are external labels used to write machine code  */
	strcpy(objectfile,filename);
	strcat(objectfile, ".ob");
	strcpy(entfile,filename);
	strcat(entfile, ".ent");
	strcpy(extfile,filename);
	strcat(extfile, ".ext");
	fpw = fopen( objectfile , "w");
	fprintf(fpw, "%d %d\n" , ICF-100, DCF); 
	while( code[i] != 0) {                        /* convert code array to the special base and write it to a .ob file */
		if(codeLine >= 1000) {
			fprintf(fpw,"%d  ", codeLine);
		} else {
			fprintf(fpw,"0%d  ", codeLine);
		}
		tempCode = code[i] >> 16;
		tempCode = tempCode & 15;
		fprintf(fpw,"A%x-",tempCode);
		tempCode = code[i] >> 12;
		tempCode = tempCode & 15;
		fprintf(fpw,"B%x-",tempCode);
		tempCode = code[i] >> 8;
		tempCode = tempCode & 15;
		fprintf(fpw,"C%x-",tempCode);
		tempCode = code[i] >> 4;
		tempCode = tempCode & 15;
		fprintf(fpw,"D%x-",tempCode);
		tempCode = code[i];
		tempCode = tempCode & 15;
		fprintf(fpw,"E%x\n",tempCode);
		i++;
		codeLine++;
	}
	i = 0;
	while( data[i] != 0) {					  /* convert data array to the special base and write it to a .ob file */
		if(codeLine >= 1000) {
			fprintf(fpw,"%d  ", codeLine);
		} else {
			fprintf(fpw,"0%d  ", codeLine);
		}
		tempCode = data[i] >> 16;
		tempCode = tempCode & 15;
		fprintf(fpw,"A%x-",tempCode);
		tempCode = data[i] >> 12;
		tempCode = tempCode & 15;
		fprintf(fpw,"B%x-",tempCode);
		tempCode = data[i] >> 8;
		tempCode = tempCode & 15;
		fprintf(fpw,"C%x-",tempCode);
		tempCode = data[i] >> 4;
		tempCode = tempCode & 15;
		fprintf(fpw,"D%x-",tempCode);
		tempCode = data[i];
		tempCode = tempCode & 15;
		fprintf(fpw,"E%x\n",tempCode);
		i++;
		codeLine++;
	}
	fclose(fpw);
	while(temp != NULL) {
		if(temp->entry_flag == ON) {
			entryFlag = ON;
			break;
		}
		temp = temp->next;
	}
	if( entryFlag == ON) {				/* write .ent file if .entry symbol was found */
		fpw = fopen( entfile , "w");
		while( temp != NULL ) {				
			if(temp->entry_flag == ON) {
				if(temp->data_flag == ON) {
					value = ICF + temp->value;
				} else {
					value = temp->value;
				}
				offset = value % 16;
				base = value - offset;
				fprintf(fpw,"%s,%d,%d\n", temp->labelname , base , offset);
			}
			temp = temp->next;
		}
		fclose(fpw);
	}
	if(p != NULL ) {        /* write .ext file if there are external labels used in the machine code(those label are found in the external list */
		fpw = fopen( extfile , "w");
		while( p != NULL) {
			if(p->externalBase >= 1000) {
				fprintf(fpw,"%s BASE %d\n", p->labelname , p->externalBase);
				fprintf(fpw,"%s OFFSET %d\n", p->labelname , p->externalBase+1);
			} else {
				fprintf(fpw,"%s BASE 0%d\n", p->labelname , p->externalBase);
				fprintf(fpw,"%s OFFSET 0%d\n", p->labelname , p->externalBase+1);
			}
			if(p->next != NULL) {
				fprintf(fpw,"\n");
			}
			p = p->next;
		}
	}
	freeSymboltable(symboltable);
	freeExternsList(list);
}
