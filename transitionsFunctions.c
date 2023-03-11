#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "header.h"


typedef struct valid_methods_of_addressing
{
	int addressingmethod0;
	int addressingmethod1;
	int addressingmethod2;
	int addressingmethod3;

}valid_methods_of_addressing;

typedef struct commands_info {
	char *name;
	int numberofoperands;
	int opcode;
	int funct;
	valid_methods_of_addressing src_address_methods;
	valid_methods_of_addressing dst_address_methods;
}commands_info;

struct commands_info commands_info_array[NUMBER_OF_COMMANDS]={

		{"mov", 2,  0, -1, {1,1,1,1}, {0,1,1,1}},
		{"cmp", 2,  1, -1, {1,1,1,1}, {1,1,1,1}},	
		{"add", 2,  2, 10, {1,1,1,1}, {0,1,1,1}},
		{"sub", 2,  2, 11, {1,1,1,1}, {0,1,1,1}},
		{"lea", 2,  4, -1, {0,1,1,0}, {0,1,1,1}},
		{"clr", 1,  5, 10, {0,0,0,0}, {0,1,1,1}},
		{"not", 1,  5, 11, {0,0,0,0}, {0,1,1,1}},
		{"inc", 1,  5, 12, {0,0,0,0}, {0,1,1,1}},
		{"dec", 1,  5, 13, {0,0,0,0}, {0,1,1,1}},
		{"jmp", 1,  9, 10, {0,0,0,0}, {0,1,1,0}},
		{"bne", 1,  9, 11, {0,0,0,0}, {0,1,1,0}},
		{"jsr", 1,  9, 12, {0,0,0,0}, {0,1,1,0}},
		{"red", 1, 12, -1, {0,0,0,0}, {0,1,1,1}},
		{"prn", 1, 13, -1, {0,0,0,0}, {1,1,1,1}},
		{"rts", 0, 14, -1, {0,0,0,0}, {0,0,0,0}},
		{"stop",0, 15, -1, {0,0,0,0}, {0,0,0,0}}
};

int getNextOperand(char line[],int lineIndex,char operand[]) {  /* stores next operand in the operand array and returns the starting index of the next operand in the line */
	int i;
	int j=0;
	for(i = lineIndex; i<strlen(line) && line[i] != ',' ; i++)	{
		if(isspace(line[i]) == 0) {	
			operand[j] = line[i];
			j++;
		}
	}
	operand[j] = '\0';
	return i+1;
}

int checkEmptyLine(char line[]) {   /* checks if a line is empty */
	int i;
	for(i=0;i<strlen(line);i++) {
		if(isspace(line[i]) == 0) {
			return NOTEMPTY;
		}
	}
	return EMPTY;
}

int getToken(char line[],char token[]) {  /* gets the first word(command,instruction,label) in the current line and returns the index where it ends */
	int i;
	int j = 0;
	int spaceFlag = OFF;
	int charFlag  = OFF;
	for(i = 0; i<strlen(line) && spaceFlag == OFF ; i++) {
		if(isspace(line[i]) == 0) {
			token[j] = line[i];
			j++;
			charFlag = ON;
		} else {
			if(charFlag == ON) {
				spaceFlag = ON;
			}
		}
	}
	token[j] = '\0';
	return i;
}	

typedef struct externalInfo{
	struct externalInfo *next;
	char *labelname;
	int externalBase;
} externalinfo;

typedef struct List {
	externalinfo *start;
	} externalsList;

void initExternalsList(externalsList *list) { /* initializes an externals list */

    list->start = NULL;
}

void addExternal(externalsList *list,char labelname[], int base) {   /* add an external label and its base to the list */
	externalinfo *p;
    p = malloc(sizeof(externalsList));
	if (p == NULL) {
        fprintf(stderr, "Fatal: failed to allocate required space");
        exit(1);
    }
	p->labelname = malloc(MAX_LABEL_LEN);
	if (p->labelname == NULL) {
        fprintf(stderr, "Fatal: failed to allocate required space");
        exit(1);
    }
	strcpy(p->labelname,labelname);
	p->externalBase = base;
	p->next = list->start;
    list->start = p;
}

typedef struct Label{
	struct Label *next;
	char *labelname;
	int value;
	int data_flag ;
	int code_flag ;
	int external_flag ;
	int entry_flag ;
	} label;

typedef struct SymbolTable {
	label *start;
	} symbolTable;

void initSymbolTable(symbolTable *table) { /* initializes a new symbol table list */

    table->start = NULL;
}


void addSymbol(symbolTable *list, char labelname[],int currValue,int codeFlag , int dataFlag,int externalFlag) /* adds new symbol to the symbol table */
{
    label *p;
    p = malloc(sizeof(label));
	if (p == NULL) {
        fprintf(stderr, "Fatal: failed to allocate required space");
        exit(1);
    }
	p->labelname = malloc(MAX_LABEL_LEN);
	if (p->labelname == NULL) {
        fprintf(stderr, "Fatal: failed to allocate required space");
        exit(1);
    }
	p->value = currValue;
	p->code_flag = codeFlag;
	p->data_flag = dataFlag;
	p->external_flag = externalFlag;
    strcpy(p->labelname,labelname);
    p->next = list->start;
    list->start = p;
}

void printSymbolTable(symbolTable *list) /* prints the symbol table */
{
    label *temp;
	temp = list->start;
	while(temp != NULL) {
		printf("%s ",temp->labelname);
		temp = temp->next;
	}
}

label *searchLabel(symbolTable *list, char currLabelname[]) {  /* search label by name, returns pointer to it if it exists or null if it doesn't */
	label *temp;
	temp = list->start;
	while(temp != NULL) {
		if(strcmp(temp->labelname,currLabelname) == 0) { 
			return temp;
		} else {
			temp = temp->next;
		}
	}
	return NULL;
}

void freeSymboltable(symbolTable *symboltable) {    /* frees dynamic memory of the symbol table */
	label *p;
	label *temp;
	temp = symboltable->start;
	while(temp != NULL) {
		p = temp;
		free(p->labelname);
		free(p);
		temp = temp->next;
	}
}

void freeExternsList(externalsList *list) {    /* frees dynamic memory of the externals list */
	externalinfo *p;
	externalinfo *temp;
	temp = list->start;
	while(temp != NULL) {
		p = temp;
		free(p->labelname);
		free(p);
		temp = temp->next;
	}
}


