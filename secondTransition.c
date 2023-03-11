/* contains second transition functions, handles .entry left and also fills the empty lines left in code array after the first transition */
/* by reading the lines that contain a macro and searchs in the symbol list for the respective label, continues to the output creation process */
/* if there are no errors otherwise, it stops here */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "header.h"
#include "createOutputFiles.c"

int checkOperandLabel(char operand[]) {   
	if(operand[0] == '#') {
		return IMMEDIATEADDRESSING; /* addressing method 0 */
	}
	if(operand[0] == 'r') {
		return REGISTERADDRESSING; /* addressing method 3 */
	}	
	if(operand[strlen(operand)-1] == ']') {
		return INDEXADDRESSING; /* addressing method 2 */
	}
	return DIRECTADDRESSING; /* addressing method 1 */
}

int writeLabelMachineCode(char currLabel[], int code[] , int IC, int ICF, symbolTable *symboltable, externalsList *externallist) {    /* completes the machine code missing after the first transition , fills every empty line left for labels , adds external labels base to a list if there are any */
	int i;
	int base;
	int offset;
	int value;
	label *currentlabel; /* pointer to label */
	int L = 0;
	int machineCode = 0;
	currentlabel = searchLabel(symboltable,currLabel);
	if(currentlabel == NULL) {
		return ERROR;
	} else {
		value = currentlabel->value;
		if(currentlabel->data_flag == ON) {
			value = value + ICF ;
		}
		if(currentlabel->external_flag == ON) {
			machineCode = machineCode | 1;         
			machineCode = machineCode << EXTERNAL_BIT;
		} else {
			machineCode = machineCode | 1;         
			machineCode = machineCode << RELOCATABLE_BIT;
		}
		offset = value % 16;
		base = value - offset;
		offset = offset | machineCode;
		base = base | machineCode;
		for( i = IC; i< MAX_WORDS; i++) {
			L++;
			if(code[i] == 0) {
				code[i] = base;
				code[i+1] = offset;
				break;
			}
		}
		L = L+2;
		if(currentlabel->external_flag == ON) {
			addExternal(externallist,currentlabel->labelname,i);
		}
	}
	return L;
}

int checkLabelLines(char line[], int lineIndex,int code[],int IC, int numberOfOperands,int lineNumber,char filename[],int ICF, symbolTable *symboltable, externalsList *externallist){   /* checks if a line contains a label and calls a function to fill the missing machine code , returns the lines added */
	int i;
	int currentIC = IC;
	char operand[MAX_LINE_LEN];	
	char label[MAX_LABEL_LEN];
	int addressingMethod;
	int nextOperandStartIndex = lineIndex;
	int L = 0;
	int j = 0;
	if(numberOfOperands == 2) {   /* for command lines with 2 operands */
		nextOperandStartIndex = getNextOperand(line,nextOperandStartIndex,operand);
		addressingMethod = checkOperandLabel(operand);  /* check if operand addressing method is 1 or 2 */
		if(addressingMethod == IMMEDIATEADDRESSING ||  addressingMethod == REGISTERADDRESSING) {
			L = 0;
		} else {
			if(addressingMethod == DIRECTADDRESSING ) {
				strcpy(label,operand);
				L = writeLabelMachineCode(label,code,currentIC,ICF,symboltable,externallist);
				if(L == ERROR) {
					printf("Error in line %d of file '%s', label %s doesn't exist in the symbol table\n",lineNumber,filename,label); 
					return ERROR;
				} else {
					currentIC = currentIC + L;
				}
			} else {    /* index addressing */
				for( i = 0; i<strlen(operand) && operand[i] != '['; i++) {
					label[j] = operand[i];
					j++;	
				}
				label[j] = '\0';
				L = writeLabelMachineCode(label,code,currentIC,ICF,symboltable,externallist);
				if(L == ERROR) {
					printf("Error in line %d of file '%s', label %s doesn't exist in the symbol table",lineNumber,filename,label); 
					return ERROR;
				} else {
					currentIC = currentIC + L;
				}
			} 
		}
		nextOperandStartIndex = getNextOperand(line,nextOperandStartIndex,operand);
		addressingMethod = checkOperandLabel(operand);  /* check if operand addressing method is 1 or 2 */
		if(addressingMethod == IMMEDIATEADDRESSING ||  addressingMethod == REGISTERADDRESSING) {
			L = 0;
		} else {
			if(addressingMethod == DIRECTADDRESSING ) {
				strcpy(label,operand);
				L = writeLabelMachineCode(label,code,currentIC,ICF,symboltable,externallist);
				if(L == ERROR) {
					printf("Error in line %d of file '%s', label %s doesn't exist in the symbol table\n",lineNumber,filename,label); 
					return ERROR;
				} else {
					currentIC = currentIC + L;
				}
			} else {    /* index addressing */
				for( i = 0; i<strlen(operand) && operand[i] != '['; i++) {
					label[j] = operand[i];
					j++;	
				}
				label[j] = '\0';
				L = writeLabelMachineCode(label,code,currentIC,ICF,symboltable,externallist);
				if(L == ERROR) {
					printf("Error in line %d of file '%s', label %s doesn't exist in the symbol table",lineNumber,filename,label); 
					return ERROR;
				} else {
					currentIC = currentIC + L;
				}
			} 
		}
	}
	if(numberOfOperands ==1) {    /* for command lines with 1 operands */
		nextOperandStartIndex = getNextOperand(line,nextOperandStartIndex,operand);
		addressingMethod = checkOperandLabel(operand);  /* check if operand addressing method is 1 or 2 */
		if(addressingMethod == IMMEDIATEADDRESSING ||  addressingMethod == REGISTERADDRESSING) {
			L = 0;
		} else {
			if(addressingMethod == DIRECTADDRESSING ) {
				strcpy(label,operand);
				L = writeLabelMachineCode(label,code,currentIC,ICF,symboltable,externallist);
				if(L == ERROR) {
					printf("Error in line %d of file '%s', label %s doesn't exist in the symbol table\n",lineNumber,filename,label); 
					return ERROR;
				} else {
					currentIC = currentIC + L;
				}
			} else {    /* index addressing */
				for( i = 0; i<strlen(operand) && operand[i] != '['; i++) {
					label[j] = operand[i];
					j++;	
				}
				label[j] = '\0';
				L = writeLabelMachineCode(label,code,currentIC,ICF,symboltable,externallist);
				if(L == ERROR) {
					printf("Error in line %d of file '%s', label %s doesn't exist in the symbol table\n",lineNumber,filename,label); 
					return ERROR;
				} else {
					currentIC = currentIC + L;
				}
			} 
		}
	}
	return L;  
}

int getNumberOfOperands(char line[], int lineIndex) {   /* gets the number of operands in a line , used after checking that the line has no errors */
	int i;
	int operands = 0;
	int charFlag = OFF;	
	for(i = lineIndex; i<strlen(line); i++) {
		if(isspace(line[i]) == 0) {
			charFlag = ON;
			if(line[i] == ',') {
				operands++;
			}
		}
	}
	if(charFlag == ON) {
		operands++;
	}
	return operands;
}

int secondTransition(char filename[], int code[], int data[] , int ICF , int DCF , symbolTable *symboltable ) {  /* second transition function , fills the empty lines left after the first transition ( lines with label ) and also handles .entry files , if there are no errors it calls the function to write the output files */
	FILE *fp;
	int i;	
	char line[MAX_LINE_LEN];   /* stores the line read from file */
	char file[MAX_FILE_NAME_LEN]; /* stores the file name */
	char token[MAX_LINE_LEN];  /* stores the first word */
	char currLine[MAX_LINE_LEN];   /* stores the code after a label */
	int lineIndex;		/* saves the index where the operand/command ends */
	int numberOfOperands;/* counts number of operands to check if it matches the amount of operands required in a command line */
	char labelStr[MAX_LABEL_LEN]; /* stores a label name */
	int lineNumber = 0;
	int L = 0;
	int IC = 100;
	int errorCounter = 0;
	label *currentlabel; /* pointer to label */
	externalsList externallist;
	initExternalsList(&externallist);    /* initializies externals list head to be null */
	strcpy(file,filename);
	strcat(file, ".am");
	fp = fopen( file , "r" );
	if( fp == NULL ) {
		perror(file);
		return ERROR;
	}
	while(fgets( line, MAX_LINE_LEN , fp )) {
		lineNumber++;
		lineIndex = getToken(line,token);	
		if(checkEmptyLine(line) == EMPTY) {  /* skip empty line */
			continue;
		}
		if(strcmp(token, ".entry") == 0) {   /* handle .entry instruction */
			strcpy(currLine,&line[lineIndex]);
			lineIndex = getToken(currLine,labelStr);
			currentlabel = searchLabel(symboltable,labelStr);
			if(currentlabel == NULL) {
				printf("Error in line %d of file %s, label %s doesn't exist in the symbol table, can't use .entry insturction",lineNumber,filename, labelStr);
				errorCounter++;
				continue;
			} else {
				if(currentlabel->external_flag == ON) {
					printf("Error in line %d of file '%s' , label %s already exists as an external label" , lineNumber , file , labelStr );
					errorCounter++;
					continue;
				} else {
					currentlabel->entry_flag = ON;
				}		
			} 
		} else {
			if (token[strlen(token)-1] == ':') {       /* check if line starts with label */
				strcpy(currLine,&line[lineIndex]);
				lineIndex = getToken(currLine,token);
				for (i = 0; i < NUMBER_OF_COMMANDS; i++) {    /* check if command comes after label if not continue */
					if (strcmp(commands_info_array[i].name, token) == 0) {
						break;
					}
				}
				if (i == NUMBER_OF_COMMANDS) {   
					continue;
				} else {				
					numberOfOperands = getNumberOfOperands(currLine,lineIndex);
					L = checkLabelLines(currLine,lineIndex,code,IC,numberOfOperands,lineNumber,filename,ICF,symboltable, &externallist);
					if( L == ERROR) {
						errorCounter++;
						break;
					}
				}
			} else {      /* check if line starts with a command if not continue */
				for (i = 0; i < NUMBER_OF_COMMANDS; i++) {
					if (strcmp(commands_info_array[i].name, token) == 0) {
						break;
					}
				}
				if (i == NUMBER_OF_COMMANDS) {   
					continue;
				} else {
					numberOfOperands = getNumberOfOperands(line,lineIndex);
					L = checkLabelLines(line,lineIndex,code,IC,numberOfOperands,lineNumber,filename,ICF,symboltable , &externallist);
					if( L == ERROR) {
						errorCounter++;
						break;
					} 
				}
			}
		}
		IC = IC + L;
		L = 0;
	}
	if(errorCounter == NOERROR) {
		createOutput(filename,code,data,ICF,DCF, symboltable ,&externallist);
		return NOERROR;
	} else {
		printf("Found errors in file '%s', can't create output files.\n",filename);
		freeSymboltable(symboltable);
		freeExternsList(&externallist);
	}
	return ERROR;
}
