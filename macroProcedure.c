/* contains the macro expansion procedure functions , reads through the file and stores macro lines into a linked list,replaces the macro with the lines stored */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "header.h"


typedef struct Macro{
	struct Macro *next;
	char *macroname;
	char *macrolines[MAX_MACRO_LINES];
	int linecount;
	} macro;

typedef struct MacroList {
	macro *start;
	} macroList;

void initList(macroList *list) /* initializes a new macro list */
{
    list->start = NULL;
}

void add(macroList *list, char macroname[]) /* adds new macro to the  macro list */
{
    macro *p;
    p = malloc(sizeof(macro));
	if (p == NULL) {
        fprintf(stderr, "Fatal: failed to allocate required space");
        exit(1);
    }
	p->macroname = malloc(MAX_MACRO_NAME_LEN);
	if (p->macroname == NULL) {
        fprintf(stderr, "Fatal: failed to allocate required space");
        exit(1);
    }
    strcpy(p->macroname,macroname);
    p->next = list->start;
    list->start = p;
}

void addCommand(macroList *list, char command[], int index)  /* add command to macro at the head of the list */
{
	list->start->macrolines[index] = malloc(MAX_LINE_LEN);
	if (list->start->macrolines[index] == NULL) {
        fprintf(stderr, "Fatal: failed to allocate required space");
        exit(1);
    }
    strcpy(list->start->macrolines[index],command);
	list->start->linecount = index+1;
}

macro *searchMacro(macroList *list, char macroname[]) {  /* search macro by name, returns pointer to it if it exists or null if it doesn't */
	macro *temp;
	temp = list->start;
	while(temp != NULL) {
		if(strcmp(temp->macroname,macroname) == 0) { 
			return temp;
		} else {
			temp = temp->next;
		}
	}
	return NULL;
}

void getFirstWord(char line[],char word[]) {  /* gets the first word in the current line */
	int i;
	int j = 0;
	int spaceFlag = OFF;
	int charFlag  = OFF;
	for(i = 0; i<strlen(line) && spaceFlag == OFF ; i++) {
		if(isspace(line[i]) == 0) {
			word[j] = line[i];
			j++;
			charFlag = ON;
		} else {
			if(charFlag == ON) {
				spaceFlag = ON;
			}
		}
	}
	word[j] = '\0';
}	

int getSecondWordIndex(char line[]) { /* gets the index of the second word */
	int i;
	int spaceFlag = OFF;
	int charFlag  = OFF;
	for(i = 0; i<strlen(line) && spaceFlag == OFF ; i++) {
		if(isspace(line[i]) == 0) {
			charFlag = ON;
		} else {
			if(charFlag == ON) {
				spaceFlag = ON;
			}
		}
	}
	for(; i<strlen(line); i++) {
		if(isspace(line[i]) == 0) {
			return i;	
		}
	}
	return i;
}	

void getSecondWord(char line[],char macroname[],int secondWordIndex) {   /* gets the second work in a code line */
	int i = secondWordIndex;
	int j = 0;
	while(j < MAX_MACRO_NAME_LEN && isspace(line[i]) == 0) {
		macroname[j] = line[i];
		i++;
		j++;
	}
	macroname[j] = '\0';
}

void freeMacroList(macroList *list) {    /* frees dynamic memory of the macro list */
	int i;
	macro *p;
	macro *temp;
	temp = list->start;
	while(temp != NULL) {
		p = temp;
		for(i = 0; i <= p->linecount ; i++) {
			free(p->macrolines[i]);
		}
		free(p->macroname);
		free(p);
		temp = temp->next;
	}
}

void macroExpansion(char filename[]) {   /* reads macros and write them to new .am file */
	FILE *fpr;	/* pointer to read current file */
	FILE *fpw;  /* pointer to write to new file to create .am file */
	macroList macrolist; /* new macro list */
	macro *currentmacro; /* pointer to macro */
	char line[MAX_LINE_LEN];	/* stores current line */
	char firstword[MAX_LINE_LEN]; /* stores first word */
	char filewrite[MAX_FILE_NAME_LEN]; /* stores the name of the new file */
	char fileread[MAX_FILE_NAME_LEN]; /* stores the name of the new file */
	char macroname[MAX_MACRO_NAME_LEN]; /* stores the macro name */
	int macroline = 0; /* current line in the macro */
	int i = 0;
	int secondwordindex; /* stores the starting index of the second word */
	int macroFlag = OFF; 
    initList(&macrolist);
	strcpy(filewrite,filename);
	strcpy(fileread,filename);
	strcat(filewrite, ".am");
	strcat(fileread, ".as");
	fpr = fopen( fileread , "r" );
	fpw = fopen( filewrite , "w");
	while(fgets( line, MAX_LINE_LEN , fpr )) {
		getFirstWord(line,firstword);
		if(strcmp(firstword,"macro") == 0) {  
			macroFlag = ON;   
			secondwordindex = getSecondWordIndex(line);
			getSecondWord(line,macroname,secondwordindex);  
			add(&macrolist,macroname);
			while(fgets( line, MAX_LINE_LEN , fpr )) {
				getFirstWord(line,firstword);
				if(strcmp(firstword,"endm") == 0) { 	
					macroline = 0;  
					break;
				} else {
					addCommand(&macrolist,line,macroline);
					macroline++;
				}
			}
		} else {
			if(macroFlag == OFF) {   /* no macro found yet */
				fprintf(fpw, "%s" , line);
			} else {		/* after at least 1 macro was found */
				currentmacro = searchMacro(&macrolist,firstword);
				if(currentmacro == NULL) {
					fprintf(fpw, "%s" , line); /* current line doesn't contain a macro */
				} else {
					if(currentmacro->linecount > 0) { 
						for(i = 0; i < currentmacro->linecount ; i++) {
							fprintf(fpw, "%s" , currentmacro->macrolines[i]);
						}
					}
				}
			}
		}	
	}
	fclose(fpr);
	fclose(fpw);
	freeMacroList(&macrolist);
}


int startMacroProcedure( char filename[]) {  /* check if the lines in the file are longer than 80 characters, file is invalid if one of the lines is longer */
	FILE *fp;
	char line[MAX_LINE_LEN];
	char file[MAX_FILE_NAME_LEN];
	int linenumber = 1;
	int errorflag = NOERROR;
	strcpy(file,filename);
	strcat(file, ".as");
	fp = fopen( file , "r" );
	if( fp == NULL ) {
		perror(file);
		return ERROR;
	}
	while(fgets( line, MAX_LINE_LEN , fp )) {
		if(line[strlen(line)-1] != '\n') {
			printf("Error, line number %d in file %s is longer than 80 characters\n", linenumber , file);
			errorflag = ERROR;
		} else {
			linenumber++;
		}
	} 	
	if(errorflag == ERROR) {
		printf("file %s has an error, can't create .am file", file);
		fclose(fp);
	} else {
		fclose(fp);
		macroExpansion(filename);
	}
	return NOERROR;
}
