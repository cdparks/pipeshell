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
  See high-level description in pshutils.h.
*/

/* USER INCLUDES */
#include "pshutils.h"
#include "cmdgroup.h"	/* struct CmdGroup */

/* SYSTEM INCLUDES */
#include <stdlib.h>	/* free */
#include <stdio.h>	/* printf, fprintf, vfprintf */
#include <string.h>	/* strlen, strtok, strcmp, strcat, strcpy */
#include <stdarg.h>	/* va_list, va_start, va_end */

/* FORWARD DECLARATIONS */
char *strdup(char *);

/*
  Base error function. In general, user code should
  not call this. Called from other error functions.
*/
void _error(int type, char *prefix, char *format, va_list args){
	if(type >= 0){
		printparse(type, "error");
	}
	if(prefix){
		fprintf(stderr, "%s: ", prefix);
	}
	vfprintf(stderr, format, args);
}

/*
  Print a warning to stderr. Type is optional (can be NONE)
  and the remaining args are like printf.
*/
void warning(int type, char *format, ...){
	va_list args;
	va_start(args, format);
	_error(type, "Warning", format, args);
	va_end(args);
}

/*
  Print a syntax error to stderr. Type is optional (can be NONE)
  and the remaining args are like printf.
*/
void syntaxerror(int type, char *format, ...){
	va_list args;
	va_start(args, format);
	_error(type, "Synax Error", format, args);
	va_end(args);
}

/*
  Print a regular error to stderr. Type is optional (can be NONE)
  and the remaining args are like printf.
*/
void generalerror(int type, char *format, ...){
	va_list args;
	va_start(args, format);
	_error(type, "Error", format, args);
	va_end(args);
}

/*
  Print a command group error to stderr. Type is optional (can
  be none, cmdgroup will be freed, and the remaining args are
  like printf. Returns NULL for error state propagation.
*/
struct CmdGroup *cmderror(int type, struct CmdGroup *cmdgroup, char *format, ...){
	freecmdgroup(cmdgroup);
	va_list args;
	va_start(args, format);
	_error(type, "Error", format, args);
	va_end(args);
	return NULL;
}

/*
  Prints parse data if $ShowParseOutput is "1"
*/
void printparse(int type, char *usage){
	int len, i;
	char *shorter;
	if(showparse){
		len = strlen(tokentext);
		if(len <= 16){
			printf("Token Type: %-16s Token: %-16s Usage: %-16s\n", translate(type), tokentext, usage);
		}
		else
		{
			shorter = strdup(tokentext);
			for(i = 13; i < 16; ++i){
				shorter[i] = '.';
			}
			shorter[16] = '\0';
			printf("Token Type: %-16s Token: %-16s Usage: %-16s\n", translate(type), shorter, usage);
			free(shorter);
		}
	}
}

/*
  Helper function translates tokentype to a
  printable string.
*/
char *translate(int type){
	switch(type){
		case EXIT: 	return "exit";
		case CD:	return "cd";
		case SETVAR:	return "setvar";
		case PIPE:	return "pipe";
		case WAIT:	return "wait";
		case REDIRIN:	return "redirect stdin";
		case REDIROUT:	return "redirect stdout";
		case STRING:	return "string";
		case VARIABLE:	return "variable";
		case WORD:	return "word";
		case COMMENT:	return "comment";
		case EOL:	return "end-of-line";
		default:	return "None";
	}
}
