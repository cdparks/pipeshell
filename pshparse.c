/*
  Copyright 2011 Christopher D. Parks

  This file is part of pipe shell.

  Pipe shell is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Pipe shell is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with pipe shell.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
  See high-level description in pshparse.h.
*/

/* USER INCLUDES */
#include "pshparse.h"
#include "pshutils.h"	/* generalerror, syntaxerror, cmderror, warning, printparse */
#include "pshlex.h"	/* YY_BUFFER_STATE, yy_scan_string, yy_delete_buffer */
#include "cmdgroup.h"	/* struct CmdGroup */
#include "cmdparse.h"	/* parsecmd */
#include "cmdengine.h"  /* runcmdgroup */
#include "hashtable.h"	/* struct Hashtable */

/* SYSTEM INCLUDES */
#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>	/* exit */
#include <stdio.h>	/* printf */
#include <string.h>	/* strlen, strcmp */
#include <unistd.h>	/* chdir, isatty */
#include <sys/wait.h>	/* waitpid */

/* USER DEFINES */
#define DIRSIZE 1024

/* FORWARD DECLARATIONS */
int yylex(void);
int fileno(FILE *);
char *strdup(char *);

void pshparse(void){
	char *line = NULL;
	char *prompt = NULL;
	YY_BUFFER_STATE buffer = NULL;
	initparser();
	while(TRUE){
		if(line){
			free(line);
		}
		if(interactive){
			prompt = getprompt();
			line = readline(prompt);
			add_history(line);
			free(prompt);
		}
		else{
			line = readline(NULL);
		}
		if(line){
			buffer = yy_scan_string(line);
			start();
			yy_delete_buffer(buffer);
		}
		else
		{
			printf("\n");
			break;
		}
	}
}

/*
  Something vaguely like a recursive descent parser.
  Calls yylex to get tokens and generates productions
  by calling related functions. yylex returns the
  tokentype and sets tokentext to the current token
  char string. tokentext = yytext, so duplicating and
  freeing tokentext must happen here.
*/
void start(void){
	int type, pid;
	struct Pair *entry;
	showparse = ((entry = find(vartable, "$ShowParseOutput")) != NULL && strcmp("1", entry->value) == 0);
	switch(type = yylex()){
		case EXIT:
			printparse(type, "exit");
			exitstate();
			break;
		case CD:
			printparse(type, "cd");
			cdstate();
			break;
		case SETVAR:
			printparse(type, "setvar");
			setvarstate();
			break;
		case WORD:
			printparse(type, "command");
			cmdstate();
			break;
		case EOL:
			printparse(type, "EOL");
			break;
		case COMMENT:
			printparse(type, "comment");
			break;
		default:
			syntaxerror(type, "Cannot begin command with '%s'\n", tokentext);
			break;
	}
	/* 'Wait' on background processes */
	while((pid = waitpid(-1, NULL, WNOHANG)) > 0){
		printf("[%d] Finished\n", pid);
	}
	/* I like whitespace */
	if(interactive){
		printf("\n");
	}
}


/*
  start -> exit EOL | ^D

  Exits the var shell.
*/
void exitstate(void){
	/*
	if(strcmp(tokentext, "^D") == 0){
		exit(0);
	}
	*/
	int type;
	char *usage = "exit usage - psh $ exit\n";
	switch(type = yylex()){
		case EOL:
			printparse(type, "EOL");
			exit(0);
			break;
		default:
			syntaxerror(type, usage);
			break;
	}
}

/*
  start -> cd word EOL

  Changes the current directory to word if
  it exists as a directory.
*/
void cdstate(void){
	int type;
	char *usage = "cd usage - psh $ cd path/to/directory/\n";
	char *path = NULL;
	struct Pair *entry;
	switch(type = yylex()){
		case WORD:
		case STRING:
			path = strdup(tokentext);
			printparse(type, "directory_name");
			break;
		case VARIABLE:
			if((entry = find(vartable, tokentext)) != NULL){
				path = tokentext = strdup(entry->value);
				printparse(STRING, "directory_name");
				break;
			}
			else{
				/* ERR: missing variable */
				generalerror(type, "Missing variable %s\n", tokentext);
				return;
			}
		default:
			/* ERR: non-word/non-var/missing argument */
			syntaxerror(type, usage);
			return;
	}
	switch(type = yylex()){
		case EOL:
			/* successful parse */
			printparse(type, "EOL");
			if(chdir(path) != 0){
				perror(path);
			}
			if(path) free(path);
			break;
		default:
			syntaxerror(type, usage);
			return;
	}
}

/*
  start -> setvar variable string EOL

  Adds/updates key (variable) to value (string).
*/
void setvarstate(void){
	int type;
	char *usage = "setvar usage - psh $ setvar $VAR \"some string\"\n";
	char *key = NULL;
	char *value = NULL;
	struct Pair *entry;
	switch(type = yylex()){
		case VARIABLE:
			printparse(type, "variable_name");
			key = strdup(tokentext);
			break;
		default:
			syntaxerror(type, usage);
			return;
	}
	switch(type = yylex()){
		case STRING:
			value = strdup(tokentext);
			printparse(type, "string");
			break;
		case VARIABLE:
			if((entry = find(vartable, tokentext)) != NULL){
				value = tokentext = strdup(entry->value);
				printparse(STRING, "directory_name");
				break;
			}
			else{
				/* ERR: missing variable */
				generalerror(type, "Missing variable %s\n", tokentext);
				return;
			}
		default:
			syntaxerror(type, usage);
			return;
	}
	switch(type = yylex()){
		case EOL:
			/* successful parse */
			printparse(type, "EOL");
			if(!add(vartable, key, value)){
				generalerror(NONE, "Could not set '%s' to '%s'", key, value);
			}
			if(key) free(key);
			if(value) free(value);
			break;
		default:
			syntaxerror(type, usage);
			return;
	}
}

/*
  Parse the user command using parscmd and
  run the resuling CmdGroup using runcmdgroup.
*/
void cmdstate(void){
	struct CmdGroup *cmdgroup = newcmdgroup();
	if(cmdgroup){
		cmdgroup = parsecmd(cmdgroup);
		if(cmdgroup){
			runcmdgroup(cmdgroup);
			freecmdgroup(cmdgroup);
		}
		/* Something failed, parsecmd will print an error */
	}
	else{
		generalerror(NONE, "Could not allocate memory for command structure\n");
	}
}

/*
  Creates shell variable hashtable, adds initial
  $PATH and $ShowParseOutput variables, and detects
  whether the input is a tty or a pipe/file/other
  redirection.
*/
void initparser(){
	vartable = newhashtable();
	if(!vartable){
		/* Only fatal error. We're pretty hosed if we can't even make the hashtable */
		generalerror(NONE, "Could not allocate shell variable table. Aborting...\n");
		exit(1);
	}
	if(!add(vartable, "$PATH", "/bin:/usr/bin:/usr/local/bin:/usr/local/gnu/bin:/usr/local/sbin:/usr/sbin:/sbin:/usr/games")){
		/* Can continue, won't be able to search for user programs */
		warning(NONE, "Could not allocate space for $PATH variable\n");
	}
	if(!add(vartable, "$ShowParseOutput", "0")){
		warning(NONE, "Could not allocate space for the $ShowParseOutput variable\n");
	}
	interactive = isatty(fileno(stdin));
}

/*
  Only print the prompt if we're in interactive
  mode.
*/
char *getprompt(){
	int len;
	char *prompt = NULL;
	char *leafdir = NULL;
	char *format = "[%s] psh $ ";
	char dir[DIRSIZE];
	getcwd(dir, DIRSIZE);
	if(strlen(dir) > 1){
		leafdir = strrchr(dir, '/') + 1;
		len = strlen(format) + strlen(leafdir) - 1;
		prompt = malloc(len);
		sprintf(prompt, format, leafdir);
		return prompt;
	}
	else{
		prompt = malloc(strlen(format));
		sprintf(prompt, format, "/");
		return prompt;
	}
}

