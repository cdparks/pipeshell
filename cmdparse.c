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
  See high-level description in cmdparser.h.
*/

/* USER INCLUDES */
#include "cmdparse.h"
#include "pshutils.h"	/* generalerror, syntaxerror, cmderror, warning, printparse */
#include "cmdgroup.h"	/* strcut CmdGroup, struct CmdNode */
#include "hashtable.h"	/* struct Hashtable */

/* SYSTEM INCLUDES */
#include <stdlib.h>	/* malloc, free */
#include <string.h>	/* strlen, strtok, strcat, strcpy */
#include <unistd.h>	/* access */


/* FORWARD DECLARATIONS */
int yylex(void);
char *strdup(char *);

/*
  Commands look like this:
  cmd [arg]* [| cmd [arg]* ]* [< filein] [> filein] [&]

  parsecmd -> parseargs
*/
struct CmdGroup *parsecmd(struct CmdGroup *cmdgroup){
	struct CmdNode *cmdnode;
	char *fullpath;
	if((fullpath = searchpath(tokentext))){
		cmdnode = pushcmd(cmdgroup, tokentext, fullpath);
		free(fullpath);
		if(cmdnode){
			return parseargs(cmdgroup, cmdnode);
		}
		else{
			return cmderror(NONE, cmdgroup, "Could not allocate memory for command '%s'\n", tokentext);
		}
	}
	else{
		return cmderror(NONE, cmdgroup, "Could not find '%s'\n", tokentext);
	}
}

/*
  parseargs -> parseargs	// Implicit, actually a while loop
		| pipestate
		| redirinstate
		| rediroutstate
		| waitstate
		| NULL
*/
struct CmdGroup *parseargs(struct CmdGroup *cmdgroup, struct CmdNode *cmdnode){
	int type;
	struct Pair *entry;
	char argname[10];
	int argno = 0;
	while(TRUE){
		switch(type = yylex()){
			case EXIT:
			case CD:
			case SETVAR:
				type = WORD;
			case STRING:
			case WORD:
				sprintf(argname, "arg %d", ++argno);
				printparse(type, argname);
				if(pusharg(cmdnode, tokentext)){
					break;
				}
				else{
					/* ERR: memory allocation */
					return cmderror(NONE, cmdgroup, "Could not allocate memory for argument '%s'\n", tokentext);
				}
			case VARIABLE:
				if((entry = find(vartable, tokentext))){
					tokentext = entry->value;
					sprintf(argname, "arg %d", ++argno);
					printparse(WORD, argname);
					if(pusharg(cmdnode, tokentext)){
						break;
					}
					else{
						/* ERR: memory allocation */
						return cmderror(NONE, cmdgroup, "Could not allocate memory for argument '%s'\n", tokentext);
					}
				}
				else{
					/* ERR: missing variable */
					return cmderror(type, cmdgroup, "Missing variable %s\n", tokentext);
				}
			case EOL:
				printparse(type, "EOL");
				return cmdgroup;
			case PIPE:
				printparse(type, "pipe");
				return pipestate(cmdgroup);
			case WAIT:
				printparse(type, "wait");
				return waitstate(cmdgroup);
			case REDIRIN:
				printparse(type, "redirect stdin");
				return redirinstate(cmdgroup);
			case REDIROUT:
				printparse(type, "redirect stdout");
				return rediroutstate(cmdgroup);
			default:
				return cmderror(type, cmdgroup, "'%s' not allowed mid-command\n", tokentext);
		}
	}
}

/*
  pipestate -> parsecmd
*/
struct CmdGroup *pipestate(struct CmdGroup *cmdgroup){
	int type;
	switch(type = yylex()){
		case WORD:
			printparse(type, "command");
			return parsecmd(cmdgroup);
		default:
			return cmderror(type, cmdgroup, "Pipe cannot be followed by '%s'\n", tokentext);
	}
}

/*
  waitstate -> NULL
*/
struct CmdGroup *waitstate(struct CmdGroup *cmdgroup){
	int type;
	switch(type = yylex()){
		case EOL:
			printparse(type, "EOL");
			cmdgroup->wait = FALSE;
			return cmdgroup;
		default:
			return cmderror(type, cmdgroup, "Wait must be final token in command\n");
	}
}

/*
  redirinstate -> rediroutstate
		| waitstate
		| NULL
*/
struct CmdGroup *redirinstate(struct CmdGroup *cmdgroup){
	int type;
	FILE *filein;
	struct Pair *entry;
	switch(type = yylex()){
		case VARIABLE:
			if((entry = find(vartable, tokentext))){
				tokentext = entry->value;
				type = STRING;
			}
			else{
				return cmderror(type, cmdgroup, "Missing variable %s\n", tokentext);
			}
		case STRING:
		case WORD:
			printparse(type, "filename");
			filein = fopen(tokentext, "r");
			if(filein){
				cmdgroup->filein = filein;
				break;
			}
			else{
				return cmderror(NONE, cmdgroup, "File '%s' could not be opened for reading\n", tokentext);
			}
		default:
			return cmderror(type, cmdgroup, "< must be followed by a filename\n");
	}
	switch(type = yylex()){
		case EOL:
			printparse(type, "EOL");
			return cmdgroup;
		case WAIT:
			printparse(type, "wait");
			return waitstate(cmdgroup);
		case REDIROUT:
			printparse(type, "redirect stdout");
			return rediroutstate(cmdgroup);
		default:
			return cmderror(type, cmdgroup, "'%s' cannot follow file redirection\n", tokentext);
	}
}

/*
  rediroutstate -> waitstate
		| NULL
*/
struct CmdGroup *rediroutstate(struct CmdGroup *cmdgroup){
	int type;
	FILE *fileout;
	struct Pair *entry;
	switch(type = yylex()){
		case VARIABLE:
			if((entry = find(vartable, tokentext))){
				tokentext = entry->value;
				type = STRING;
			}
			else{
				return cmderror(type, cmdgroup, "Missing variable %s\n", tokentext);
			}
		case STRING:
		case WORD:
			printparse(type, "filename");
			fileout = fopen(tokentext, "w");
			if(fileout){
				cmdgroup->fileout = fileout;
				break;
			}
			else{
				return cmderror(NONE, cmdgroup, "File '%s' could not be opened for writing\n", tokentext);
			}
		default:
			return cmderror(type, cmdgroup, "> must be followed by a filename\n");
	}
	switch(type = yylex()){
		case EOL:
			printparse(type, "EOL");
			return cmdgroup;
		case WAIT:
			printparse(type, "wait");
			return waitstate(cmdgroup);
		default:
			return cmderror(type, cmdgroup, "'%s' cannot follow file redirection\n", tokentext);
	}
}

/*
  If passed an absolue path or one relative
  to ./, just return it if readable, else NULL.
  Otherwise, search $PATH for a readable command,
  again else NULL.
*/
char *searchpath(char *cmd)
{
	int cmdlen = strlen(cmd);
	char *path;
	char *newpath;
	char *current;
	int pathlen;
	struct Pair *pair;
	/* command is absolute path or relative to ../ or ./ */
	if(cmdlen > 0 && (cmd[0] == '.' || cmd[0] == '/'))
	{
		if(access(cmd, R_OK) == 0){
			return strdup(cmd);
		}
		else{
			return NULL;
		}
	}
	if((pair = find(vartable, "$PATH")) != NULL){
		path = strdup(pair->value);
	}
	else{
		return NULL;
	}
	for(current = strtok(path, ":"); current != NULL; current = strtok(NULL, ":"))
	{
		pathlen = strlen(current);
		newpath = malloc(pathlen + cmdlen + 2);
		strcpy(newpath, current);
		newpath[pathlen++] = '/';
		newpath[pathlen] = '\0';
		strcat(newpath, cmd);
		if(access(newpath, R_OK) == 0){
			free(path);
			return newpath;
		}
		free(newpath);
	}
	free(path);
	return NULL;
}
