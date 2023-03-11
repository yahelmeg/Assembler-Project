/* first transition function, reads line by line and check for various errors, and add label to the symbol list if they are valid. it creates the machine */ 
/* code and stores it in the respective array while leaving room for lines that contain label, those lines would be filled in the second transition  */
/* after reading through the whole file the function checks if there are any errors, if it found error it stops here and if it didn't it continues to the */
/* second transition function */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "header.h"
#include "secondTransition.c"


int noOperandBeforeComma(char line[], int index) {   /* checks if there's a comma before an operand  */
	int i;
	int charFlag = OFF;
	for(i = index; i<strlen(line) && charFlag == OFF ; i++) {
		if(isspace(line[i]) == 0) {
			if(line[i] == ',') {
				return ERROR; /* no operand before comma */
			} else {
				charFlag = ON;
			}
		}
	}
	return NOERROR;
}

int checkInstructionLineValidity(char line[] , char filename[] , int linenumber) {  /* checks .string .entry and .extern lines validity , no misplaced commas , extraneous text , returns the number of operands or -1 for errors and prints the relevant error */
	int i;
	int result;
	int charFlag = OFF;
	int operands = 0;
	result = noOperandBeforeComma(line,0);
	if(result == ERROR) {
		printf("Syntax error found, misplaced comma in line %d of file '%s'\n",linenumber,filename);
		return ERROR;
	}
	for(i = 0; i<strlen(line) ; i++) {
		if(isspace(line[i]) == 0) {
			charFlag = ON;		
		} else {
			if(charFlag == ON) {
				charFlag = OFF;
				operands++;
			}
		}
	}
	if(charFlag == ON) {
		operands++;
	}
	return operands;
}

int checkCommandLineValidity(char line[], int index, char filename[], int linenumber) {  /* checks the line validity : right amount of operands , no extra commas , missing commads , extraneous text , returns the amount of operands or -1 for other errors also prints the relevant error */
	int i;
	int result;
	int commaFlag = OFF;
	int charFlag = OFF;
	int spaceFlag = OFF;
	int operands = 0;
	result = noOperandBeforeComma(line,index);
	if(result == ERROR) {
		printf("Syntax error found, no operand before comma in line %d of file '%s'\n",linenumber,filename);
		return ERROR;
	}
	for(i = index; i<strlen(line) ; i++) {
		if(isspace(line[i]) == 0) {
			if(line[i] == ',') {
				if(commaFlag == ON ) {
					printf("Syntax error found, multiple consecutive commas in line %d of file '%s'\n",linenumber,filename);
					return ERROR; /* extra commas */
				} else {
					spaceFlag = OFF;
					commaFlag = ON;
					charFlag = OFF;  /* found a comma separating operands */
					operands++;
				}
			} else {
				charFlag = ON;
				if(spaceFlag == ON) {
					printf("Syntax error found, missing comma between operands in line %d of file '%s'\n",linenumber,filename);
					return ERROR;
				}
				if(commaFlag == ON) {   
					commaFlag = OFF;
				}
			}
		} else {
		    if(charFlag == ON)
			    spaceFlag = ON;
		}
	}
	if(commaFlag == ON) {
	    printf("Syntax error found, extraneous text in line %d of file '%s'\n",linenumber,filename);
	    return ERROR; 
	}
	if(charFlag == ON) {
		operands++;
	}
	
	return operands;
}


int writeMachineCode(int code[], int IC, int commandArrayIndex, int srcReg , int srcAddMethod , int dstReg , int dstAddMethod , int srcNum , int dstNum) {
	int srcMethod;
	int dstMethod;
	int machineCode = 0;
	int tempCode = 0;
	int L = 0;   /* amount of words written or unknown words(from labels) waiting to be written */
	int opCode = commands_info_array[commandArrayIndex].opcode;
	int funct =	commands_info_array[commandArrayIndex].funct;
	if(funct == -1) { /* no funct */
		funct = 0;
	}
	if(srcAddMethod != NO_ADD_METHOD) {
		srcMethod = srcAddMethod;
	} else {
		srcMethod = 0;	
	}
	if(dstAddMethod != NO_ADD_METHOD) {
		dstMethod = dstAddMethod;
	} else {
		dstMethod = 0;	
	}
	tempCode = tempCode | 1;                /* write first line */
	tempCode = tempCode << ABSOLUTE_BIT; 
	machineCode = machineCode | tempCode; 
	tempCode = 0; 		
	tempCode = tempCode | 1;         
	tempCode = tempCode << opCode;
	machineCode = machineCode | tempCode;   
	tempCode = 0;
	code[IC] = machineCode;
	IC++;
 	L++;
	if( (strcmp(commands_info_array[commandArrayIndex].name, "stop") == 0) || (strcmp(commands_info_array[commandArrayIndex].name, "rts") == 0) ) {
		return L;
	}
	machineCode = 0;                   /* write second line if the command isn't stop or rts */
	tempCode = tempCode | 1;          
	tempCode = tempCode << ABSOLUTE_BIT; 
	machineCode = machineCode | tempCode;
	tempCode = 0;
	tempCode = tempCode | funct;       
	tempCode = tempCode << FUNCT_BIT; 
	machineCode = machineCode | tempCode;
	tempCode = 0;
	tempCode = tempCode | srcReg;          
	tempCode = tempCode << SOURCE_REG_BIT; 
	machineCode = machineCode | tempCode;
	tempCode = 0;
	tempCode = tempCode | srcMethod;          
	tempCode = tempCode << SOURCE_METHOD_BIT; 	
	machineCode = machineCode | tempCode;
	tempCode = 0;
	tempCode = tempCode | dstReg;          
	tempCode = tempCode << DST_REG_BIT; 
	machineCode = machineCode | tempCode;
	tempCode = 0;
	machineCode = machineCode | dstMethod;          
	code[IC] = machineCode;
	machineCode = 0;
	IC++;
	L++;
	if(srcAddMethod == 0) {
		if(srcNum >= 0) {
			tempCode = tempCode | 1;          
			tempCode = tempCode << ABSOLUTE_BIT; 
			machineCode = machineCode | tempCode;
			tempCode = 0;
			machineCode = machineCode | srcNum; 
		} else {
			tempCode = tempCode | 1;          
			tempCode = tempCode << ABSOLUTE_BIT; 
			tempCode = 0;
			srcNum = srcNum & 65535; /* turn off the 16th left most bits */
			machineCode = machineCode | srcNum;
		}
		code[IC] = machineCode;
		IC++;
		L++;
	}
	if(srcAddMethod == DIRECTADDRESSING || srcAddMethod == INDEXADDRESSING) {   /* save space for 2 words */
		L = L+2;
		IC = IC+2;
	}
	if(dstAddMethod == 0) {          /* write number line */
		if(dstNum >= 0) {
			tempCode = tempCode | 1;          
			tempCode = tempCode << ABSOLUTE_BIT; 
			machineCode = machineCode | tempCode;
			tempCode = 0;
			machineCode = machineCode | dstNum; 
		} else {
			tempCode = tempCode | 1;          
			tempCode = tempCode << ABSOLUTE_BIT; 
			machineCode = machineCode | tempCode;
			tempCode = 0;
			dstNum = dstNum & 65535; /* turn off the 16th left most bits */
			machineCode = machineCode | dstNum;
		}
		code[IC] = machineCode;
		IC++;
		L++;
	}
	if(dstAddMethod == DIRECTADDRESSING || dstAddMethod == INDEXADDRESSING) { /* save space for 2 words */
		L = L+2;
		IC = IC+2;
	}
	return L;
} 

int getNum(char operand[]) {    /* gets the number after # for addressing method 0 */
	int i;
	int j = 0;
	int sign = 1;
	char num[MAX_LINE_LEN];
	if(isdigit(operand[1]) == 0 ) {
		i = 2;
		if(operand[1] == '-') {
			sign = -1;
		}
	} else {
		i = 1;
	}
	while( i < strlen(operand) ) {
		num[j] = operand[i];
		i++;
		j++;
	}
	if(sign == 1) {
 		return atoi(num);
	} else {
		return -1*atoi(num);
	}
}   
                 

int getRegNum(char operand[]) {    /* gets register num for addressing methods 2 or 3 */
	int i;
	char regNum[3];
	for(i=0;i<strlen(operand);i++) {
		if(operand[i] == 'r') {
			regNum[0] = operand[i+1];
			regNum[1] = operand[i+2];
			regNum[2] = '\0';
		}
	}
	return atoi(regNum);
}

int checkAddressingMethod(char operand[] , char filename[] , int lineNumber) {
	int i;
	int reg;
	int validRegFlag = ON;
	char regis[MAX_LABEL_LEN];   /* contains the number found after 'r' to check if register number is valid */
	char regisIndexCheck[MAX_LINE_LEN];   /* check index in index addressing */
	if(strlen(operand) > MAX_LABEL_LEN) {
		printf("Error occured in line %d of file '%s' , label size is longer than 31\n", lineNumber,filename);
		return ERROR;
	}
	for (i = 0; i < NUMBER_OF_COMMANDS; i++) {
		if (strcmp(commands_info_array[i].name, operand) == 0) {
			printf("Error occured in line %d of file '%s', label name can't be the same as operand name\n", lineNumber,filename);
			return ERROR;
		}
	}
	if(operand[0] == '#') {  /* check if immediate addressing */
		if(operand[1] == '+' || operand[1] == '-') {   /* number might start with + or - */
			for(i=2; i < strlen(operand); i++) {
				if(isdigit(operand[i]) == 0) {
					printf("Error occured in line %d of file '%s', invalid label or operator\n",lineNumber,filename);
					return ERROR;
				}		
			}
			return IMMEDIATEADDRESSING;  /* addressing method 0  */
		} else {
			for(i=1; i < strlen(operand); i++) {	/* number might not start with a sign */
				if(isdigit(operand[i]) == 0) {
					printf("Error occured in line %d of file '%s', invalid label or operator\n",lineNumber,filename);
					return ERROR;
				}		
			}
			return IMMEDIATEADDRESSING;  /* addressing method 0 */
		}
	}
	if(operand[0] == 'r') {   /* check if register direct addressing*/
		for(i=1; i < strlen(operand); i++) {
				if(isdigit(operand[i]) == 0) {
					validRegFlag = OFF;
				}
		}
		if(validRegFlag == ON) {
			strcpy(regis,&operand[1]);
			reg = atoi(regis);
			if(reg >=1 && reg<=16) {
				return REGISTERADDRESSING;  /* addressing method 3 */
			}
		}
	}
	if(operand[strlen(operand)-1] == ']' && isalpha(operand[0])) {   /* check if index addressing */
		for(i = 1; i < strlen(operand); i++) {
			if(operand[i] == '[') {
				strcpy(regisIndexCheck,&operand[i+1]);
				regisIndexCheck[strlen(regisIndexCheck)-1] = '\0';
				if(strlen(regisIndexCheck) != 3 || regisIndexCheck[0] != 'r') {
					printf("Error occured in line %d of file '%s', invalid index for index addressing method\n",lineNumber,filename);
					return ERROR;
				} else {
					strcpy(regis,&regisIndexCheck[1]);
					reg = atoi(regis);
					if(reg >=10 && reg<=15) {
						return INDEXADDRESSING;  /* addressing method 2 */
					}
				}
			}
		}
	}
	if(isalpha(operand[0]) == 0) {    /* check if direct addressing */
		printf("Error occured in line %d of file '%s', label starts with an invalid character\n",lineNumber,filename);
		return ERROR;
	} else {	
		for(i = 1; i < strlen(operand); i++) {    
			if(isalnum(operand[i])==0) {
				printf("Error occured in line %d of file '%s', label contains an invalid character\n",lineNumber,filename);
				return ERROR;
			}
		}
		return DIRECTADDRESSING; /* addressing method 1 */
	}
	return ERROR;
}

int handleCommand(int commandArrayIndex, int numberOfOperands, char line[], int lineIndex, char filename[] , int lineNumber , int code[] , int IC ) { /* handle command line , calls a function to write the machine code according to the number of operands in the line */
	int L;
	int addressingMethod;
	char operand[MAX_LINE_LEN];
	int nextOperandStartIndex = lineIndex;
	int srcAddressMethod = NO_ADD_METHOD; 
	int dstAddressMethod = NO_ADD_METHOD;
	int srcReg = 0;
	int dstReg = 0;
	int srcNum = 0;
	int dstNum = 0;
	if(commands_info_array[commandArrayIndex].numberofoperands != numberOfOperands) {
		printf("Error in line %d of file '%s', operands number not matching current command\n",lineNumber,filename);
		return ERROR;
	}
	if(numberOfOperands == 2) {
		nextOperandStartIndex = getNextOperand(line,nextOperandStartIndex,operand);   /* first operand of commands with 1 operand , put into addressing source */
		addressingMethod = checkAddressingMethod(operand,filename,lineNumber);
		if(addressingMethod == ERROR) {
			return ERROR;
		} else {
			switch(addressingMethod) {
				case IMMEDIATEADDRESSING:
					if(commands_info_array[commandArrayIndex].src_address_methods.addressingmethod0 == 0) {
						printf("Error in line %d of file '%s', invalid source addressing method\n",lineNumber,filename);
						return ERROR;
					}
					srcAddressMethod = 0;
					srcNum = getNum(operand);
					break;
				case DIRECTADDRESSING:
					if(commands_info_array[commandArrayIndex].src_address_methods.addressingmethod1 == 0) {
						printf("Error in line %d of file '%s', invalid source addressing method\n",lineNumber,filename);
						return ERROR;
					}
					srcAddressMethod = 1;
					break;
				case INDEXADDRESSING:
					if(commands_info_array[commandArrayIndex].src_address_methods.addressingmethod2 == 0) {
						printf("Error in line %d of file '%s', invalid source addressing method\n",lineNumber,filename);
						return ERROR;
					}
					srcAddressMethod = 2;
					srcReg = getRegNum(operand);
					break;
				case REGISTERADDRESSING:
					if(commands_info_array[commandArrayIndex].src_address_methods.addressingmethod3 == 0) {
						printf("Error in line %d of file '%s', invalid source addressing method\n",lineNumber,filename);
						return ERROR;
					}
					srcAddressMethod = 3;
					srcReg = getRegNum(operand);
					break;
				default:
					break;
			}
		}
		nextOperandStartIndex = getNextOperand(line,nextOperandStartIndex,operand);   /* second operand of command with 2 operands , put into destination source */
		addressingMethod = checkAddressingMethod(operand,filename,lineNumber);
		if(addressingMethod == ERROR) {
			return ERROR;
		} else {
			switch(addressingMethod) {
				case IMMEDIATEADDRESSING:
					if(commands_info_array[commandArrayIndex].dst_address_methods.addressingmethod0 == 0) {
						printf("Error in line %d of file '%s', invalid destination addressing method\n",lineNumber,filename);
						return ERROR;
					}
					dstAddressMethod = 0;
					dstNum = getNum(operand);
					break;
				case DIRECTADDRESSING:
					if(commands_info_array[commandArrayIndex].dst_address_methods.addressingmethod1 == 0) {
						printf("Error in line %d of file '%s', invalid destination addressing method\n",lineNumber,filename);
						return ERROR;
					}
					dstAddressMethod = 1;
					break;
				case INDEXADDRESSING:
					if(commands_info_array[commandArrayIndex].dst_address_methods.addressingmethod2 == 0) {
						printf("Error in line %d of file '%s', invalid destination addressing method\n",lineNumber,filename);
						return ERROR;
					}
					dstAddressMethod = 2;
					dstReg = getRegNum(operand);
					break;
				case REGISTERADDRESSING:
					if(commands_info_array[commandArrayIndex].dst_address_methods.addressingmethod3 == 0) {
						printf("Error in line %d of file '%s', invalid destination addressing method\n",lineNumber,filename);
						return ERROR;
					}
					dstAddressMethod = 3;
					dstReg = getRegNum(operand);
					break;
				default:
					break;
			}
			L = writeMachineCode(code, IC, commandArrayIndex, srcReg , srcAddressMethod , dstReg , dstAddressMethod , srcNum , dstNum);
		}
	}
	if(numberOfOperands == 1) {
		nextOperandStartIndex = getNextOperand(line,nextOperandStartIndex,operand);   /* first operand of command with 1 operand , put into destination source */
		addressingMethod = checkAddressingMethod(operand,filename,lineNumber);
		if(addressingMethod == ERROR) {
			return ERROR;
		} else {
			switch(addressingMethod) {
				case IMMEDIATEADDRESSING:
					if(commands_info_array[commandArrayIndex].dst_address_methods.addressingmethod0 == 0) {
						printf("Error in line %d of file '%s', invalid destination addressing method\n",lineNumber,filename);
						return ERROR;
					}
					dstAddressMethod = 0;
					dstNum = getNum(operand);
					break;
				case DIRECTADDRESSING:
					if(commands_info_array[commandArrayIndex].dst_address_methods.addressingmethod1 == 0) {
						printf("Error in line %d of file '%s', invalid destination addressing method\n",lineNumber,filename);
						return ERROR;
					}
					dstAddressMethod = 1;
					break;
				case INDEXADDRESSING:
					if(commands_info_array[commandArrayIndex].dst_address_methods.addressingmethod2 == 0) {
						printf("Error in line %d of file '%s', invalid destination addressing method\n",lineNumber,filename);
						return ERROR;
					}
					dstAddressMethod = 2;
					dstReg = getRegNum(operand);
					break;
				case REGISTERADDRESSING:
					if(commands_info_array[commandArrayIndex].dst_address_methods.addressingmethod3 == 0) {
						printf("Error in line %d of file '%s', invalid destination addressing method\n",lineNumber,filename);
						return ERROR;
					}
					dstAddressMethod = 3;
					dstReg = getRegNum(operand);
					break;
				default:
					break;
			}
			L = writeMachineCode(code, IC, commandArrayIndex, srcReg , srcAddressMethod , dstReg , dstAddressMethod , srcNum , dstNum);
		}
	}
	if(numberOfOperands == 0) {
		L = writeMachineCode(code, IC, commandArrayIndex, srcReg , srcAddressMethod , dstReg , dstAddressMethod , srcNum , dstNum);	
	}
	return L;
}

int getNextNum(char line[],char num[] ,int index,char filename[],int lineNumber) {   /* stores next num from .data instruction in num array, return index of next num or ERROR for error*/
	int j = 0;
	int i = index;
	while(isspace(line[i]) != 0) {
		i++;
	}
	if(line[i] == '-') {
		num[j] = '-';
		j++;
		i++;
	} else {
		if( line[i] == '+') {
			num[j] = '+';
			j++;
			i++;
		} else {
			num[j] = '+';
			j++;
		}
	}
	while( i<strlen(line) && line[i] != ',') {
		if(isdigit(line[i] == 0)) {
			printf("Error in line %d of file '%s', .data instruction contains an invalid character",lineNumber,filename);
			return ERROR;
		}
		num[j] = line[i];
		j++;
		i++;
	}
	num[j] = '\0';
	return i+1;
} 

int handleDataInstructions(int amountOfNums,char line[],int lineIndex,char filename[],int lineNumber,int data[] ,int DC) { /* handle .data instructions, add nums to code array and returns error for error or the number of words added otherwise */
	int index = lineIndex;
	char num[MAX_LINE_LEN];
	int number;
	int i;
	int sign;
	int D = 0 ;
	int currDC = DC;
	int machineCode = 0;
	int tempCode = 0;
	for(i = 0; i<amountOfNums; i++) {
		index = getNextNum(line,num, index, filename,lineNumber);
		if(index == ERROR) {
			return ERROR;
		}
		if(num[0] == '+') {
			strcpy(num,&num[1]);
			number = atoi(num);
			sign = 1;
		} else {
			strcpy(num,&num[1]);
			number = atoi(num);
			sign = -1;
		}
		if(number < MIN_INT_VALUE || number > MAX_INT_VALUE ) {
			printf("Error in line %d of file '%s', number is too big there aren't enough bits to store it",lineNumber,filename);
			return ERROR;
		} else {
			tempCode = tempCode | 1;          
			tempCode = tempCode << ABSOLUTE_BIT;   	
			machineCode = machineCode | tempCode;
			tempCode = 0; 
			if(sign == 1) {
				machineCode = machineCode | number; 
			} else {
	        	number = (~number)+1;
				number = number & 65535; /* turn off the 16th left most bits */
				machineCode = machineCode | number;
			}	
		}
		data[currDC] = machineCode;
		machineCode = 0;
		currDC++;
		D++;	
	}
	return D;
}
int handleStringInstructions(char line[],int lineIndex,char filename[],int lineNumber,int data[] ,int DC) {   /* handle .string instructions, add chars to code array and returns error for error or the number of words added otherwise */
	int i;
	char string[MAX_LINE_LEN];
	int j = 0;
	int D = 0;
	int tempCode = 0;
	int machineCode = 0;
	for(i = lineIndex; i<strlen(line); i++) {
		if(isspace(line[i]) == 0) {
			string[j] = line[i];
			j++;
		}
	}
	string[j] = '\0';
	if(string[0] != '"' || string[strlen(string)-1] != '"') {
		printf("Error in line %d of file '%s', instruction string does not start and end with quotes \n",lineNumber,filename);
		return ERROR;
	} else {
		for( i = 1; i< strlen(string)-1; i++) {
			tempCode = tempCode | 1;          
			tempCode = tempCode << ABSOLUTE_BIT;   	
			machineCode = machineCode | tempCode;
			machineCode = machineCode | (int)string[i];
			data[DC] = machineCode;
			DC++;
			D++;
			tempCode = 0; 
			machineCode = 0;
		}
		machineCode = machineCode | 1;          
		machineCode = machineCode << ABSOLUTE_BIT;
		data[DC] = machineCode;
		D++; 
	}
	return D;
}


int checkLabelValidity(char label[],char filename[],int lineNumber) {
	int i;
	if(strlen(label) > MAX_LABEL_LEN) {
		printf("Error occured in line %d of file '%s' , label size is longer than 31\n", lineNumber,filename);
		return ERROR;
	}
	for (i = 0; i < NUMBER_OF_COMMANDS; i++) {
		if (strcmp(commands_info_array[i].name, label) == 0) {
			printf("Error occured in line %d of file '%s', label name can't be the same as operand name\n", lineNumber,filename);
			return ERROR;
		}
	} 
	if(isalpha(label[0]) == 0) {    /* check if direct addressing */
		printf("Error occured in line %d of file '%s', label starts with an invalid character\n",lineNumber,filename);
		return ERROR;
	} else {	
		for(i = 1; i < strlen(label); i++) {    
			if(isalnum(label[i])==0) {
				printf("Error occured in line %d of file '%s', label contains an invalid character\n",lineNumber,filename);
				return ERROR;
			}
		}
	}
	return NOERROR;
}


int firstTransition(char filename[], int code[], int data[]) {   /* first transition function , reads command line from .am file and calls relevant functions to check for errors and handle the code lines , calls second transition function if there are no errors and stops otherwise */ 
	FILE *fp;
	int i;		
	int numberOfOperands;/* counts number of operands to check if it matches the amount of operands required in a command line */
	int amountOfNums;   /* counts the number of nums in .data line */
	int lineIndex;		/* saves the index where the operand/command ends */
	label *currentlabel; /* pointer to label */
	char line[MAX_LINE_LEN];   /* stores the line read from file */
	char token[MAX_LINE_LEN];  /* stores the first word */
	char label[MAX_LABEL_LEN]; /* stores a label name */
	char currLine[MAX_LINE_LEN];   /* stores the code after a label */
	char file[MAX_FILE_NAME_LEN]; /* stores the file name */
	symbolTable symboltable;   
	int lineNumber = 0;
	int DC = 0;
	int IC = 100;
	int D = 0;      /* used to update DC based on the amount of lines added */
	int L = 0;      /* used to update IC based on the amount of lines added */
	int errorCounter = 0;  /* counts number of errors */
	initSymbolTable(&symboltable);  /* initializies symbol table head to be null */
	strcpy(file,filename);
	strcat(file, ".am");
	fp = fopen( file , "r" );
	if( fp == NULL ) {
		perror(file);
		return ERROR;
	}
	while(fgets( line, MAX_LINE_LEN , fp )) {
		lineNumber++;
		if(checkEmptyLine(line) == EMPTY) {  /* skip empty line */
			continue;
		}
		lineIndex = getToken(line,token);
		if(token[0] == ';') {				/* skips line that starts with ; */
			continue;
		} 
		if (token[strlen(token)-1] == ':') {       /* add new label to the label table if it's valid */
			token[strlen(token)-1] = '\0';
			strcpy(label,token);
			if(checkLabelValidity(label,file,lineNumber) == ERROR) {
				errorCounter++;
				continue;
			} else {
				currentlabel = searchLabel(&symboltable,label);
				if(currentlabel != NULL) {
				printf("Error in line %d of file %s, label %s already exists in the symbol table",lineNumber,filename, label);
					errorCounter++;
					continue;
				}
				strcpy(currLine,&line[lineIndex]);
				lineIndex = getToken(currLine,token);
				if(strcmp(token, ".data") == 0) {   /* check if .data instructions comes after label */
					amountOfNums = checkCommandLineValidity(currLine,lineIndex,filename,lineNumber);
					if(amountOfNums == ERROR) {
						errorCounter++;
						continue;
					}
					if(amountOfNums == 0) {
						printf("Error in line %d of file '%s', instruction data does not contain any numbers\n",lineNumber,filename);
						errorCounter++;
						continue;
					}
					D = handleDataInstructions(amountOfNums,currLine,lineIndex,filename,lineNumber, data , DC);
					if( D == ERROR) {
						errorCounter++;
						continue;
					}
					addSymbol(&symboltable,label,DC,OFF,ON,OFF);
				} else {
					if(strcmp(token, ".string") == 0) {  /* check if .string instruction comes after label */
						D = handleStringInstructions(currLine,lineIndex,filename,lineNumber,data,DC);
						if( D == ERROR) {
							errorCounter++;
							continue;
						}
						addSymbol(&symboltable,label,DC,OFF,ON,OFF);		
					} else {
						for (i = 0; i < NUMBER_OF_COMMANDS; i++) {
							if (strcmp(commands_info_array[i].name, token) == 0) {
								break;
							}
						}
						if (i == NUMBER_OF_COMMANDS) {   /* check if command comes after label */
							printf("Syntax error found in line %d of file '%s',invalid code after label\n",lineNumber,filename);
							errorCounter++;
							continue;
						}else {
							numberOfOperands = checkCommandLineValidity(currLine,lineIndex,filename,lineNumber);
							if(numberOfOperands == ERROR) {
								errorCounter++;
								continue;
							} else {
								L = handleCommand(i,numberOfOperands,currLine,lineIndex,filename,lineNumber, code , IC );
								if( L == ERROR) {
									errorCounter++;
									continue;
								}
								addSymbol(&symboltable,label,IC,ON,OFF,OFF);
							} 
						}
					}
				}
			}
		} else if(token[0] == '.') {            /* potentially found an instruction */
			if ((strcmp(token, ".string") != 0) && (strcmp(token, ".data") != 0) && (strcmp(token, ".entry") != 0) && (strcmp(token,".extern") != 0)) {
				printf("Syntax error found in line %d of file '%s', invalid instruction\n",lineNumber,file);
				errorCounter++;
				continue;
			} else {
				if(strcmp(token, ".data") == 0) {  /* handle .data instruction */
					amountOfNums = checkCommandLineValidity(line,lineIndex,file,lineNumber);
					if(amountOfNums == ERROR) {
						errorCounter++;
						continue;
					}
					if(amountOfNums == 0) {
						printf("Error in line %d of file '%s', instruction data does not contain any numbers\n",lineNumber,file);
						errorCounter++;
						continue;
					}
					D = handleDataInstructions(amountOfNums,line,lineIndex,file,lineNumber, data , DC);
					if( D == ERROR) {
						errorCounter++;
						continue;
					}	
				}
				if(strcmp(token, ".string") == 0) {  /* handle .string instruction */
					D = handleStringInstructions(line,lineIndex,file,lineNumber,data,DC);
					if( D == ERROR) {
						errorCounter++;
						continue;
					}
				}
				if(strcmp(token, ".entry") == 0) {    /* check .entry line validity */
					strcpy(currLine,&line[lineIndex]);
					numberOfOperands = checkInstructionLineValidity(currLine,file,lineNumber);
					if(numberOfOperands == ERROR) {
						errorCounter++;
						continue;
					}	
					if(numberOfOperands != 1) {
						printf("Error in line %d of file '%s' , extraneous text after .entry line\n", lineNumber , file);
						errorCounter++;
						continue;
					}
					lineIndex = getToken(currLine,label);
					if(checkLabelValidity(label,filename, lineNumber) == ERROR) {
						errorCounter++;
						continue;
					}
				}
				if(strcmp(token, ".extern") == 0) {  /* handle .extern instruction */
					strcpy(currLine,&line[lineIndex]);
					numberOfOperands = checkInstructionLineValidity(currLine,file,lineNumber);
					if(numberOfOperands == ERROR) {
						errorCounter++;
						continue;
					}	
					if(numberOfOperands != 1) {
						printf("Error in line %d of file '%s' , extraneous text after .extern line\n", lineNumber , file);
						errorCounter++;
						continue;
					}
					lineIndex = getToken(currLine,label);
					if(checkLabelValidity(label,filename, lineNumber) == ERROR) {
						errorCounter++;
						continue;
					}
					currentlabel = searchLabel(&symboltable,label);
					if(currentlabel == NULL) {
						addSymbol(&symboltable,label,0,OFF,OFF,ON);
					} else {
						if(currentlabel->external_flag == ON) {
							continue;
						} else {
							printf("Error in line %d of file '%s' , label %s already exists as non-external label\n" , lineNumber , file , label );
						}
					}
				}		
			}
		} else {        /* potenatially found a command line */
			for (i = 0; i < NUMBER_OF_COMMANDS; i++) {
				if (strcmp(commands_info_array[i].name, token) == 0) {
					break;
				}
			}
			if (i == NUMBER_OF_COMMANDS) {
				printf("Syntax error found in line %d of file '%s',invalid command name\n",lineNumber,file);
				errorCounter++;
				continue;
			}else {
				numberOfOperands = checkCommandLineValidity(line,lineIndex,file,lineNumber);
				if(numberOfOperands == ERROR) {
					errorCounter++;
					continue;
				} else {
					L = handleCommand(i,numberOfOperands,line,lineIndex,file,lineNumber, code , IC );
					if( L == ERROR) {
						errorCounter++;
						continue;
					}
				} 
			} 
		}
		DC = DC + D;
		D = 0;
		IC = IC + L; 
		L = 0;	
		if(IC+DC > 8024) {
			errorCounter++;
			printf("Error, number of words exceeds 8024 which is the amount of free space available for file '%s'", file);
			break;
		}
	}	
	fclose(fp);
	if(errorCounter == NOERROR) {
		secondTransition(filename, code , data , IC , DC , &symboltable); 
	} else {
		printf("Found errors in file '%s', can't write machine code\n",filename);
		freeSymboltable(&symboltable);
		return ERROR;
	}
	return NOERROR;
}
