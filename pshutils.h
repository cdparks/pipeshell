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
  pshutils.h/c: Defines a group of functions, enums, and constants
  that are used by pshparse, pshlex, cmdparse, and cmdengine.
*/

#ifndef PSHUTILS_H
#define PSHUTILS_H

#define TRUE 1
#define FALSE 0

/*
  Pass NONE as type to error functions if you
  want to print a token
*/
#define NONE -1

#include "cmdgroup.h"	/* for struct CmdGroup */
#include <stdarg.h>	/* va_list, va_start, va_end */

typedef enum{
	EXIT, CD, SETVAR, WORD, VARIABLE, STRING, PIPE, WAIT, REDIRIN, REDIROUT, COMMENT, EOL
} tokentype;

/* PARSER HELPERS */
void printparse(int type, char *usage);			/* print Token Type, Token, Usage */
char *translate(int type);				/* Translate tokentype to printable string */

/* ERROR FUNCTIONS */
void _error(int type, char *prefix, char *format, va_list args);	/* Base error function. Don't call from user code. */
void warning(int type, char *format, ...);				/* Print warning to stderr */
void generalerror(int type, char *format, ...);				/* Print error to stderr */
void syntaxerror(int type, char *format, ...);				/* Print syntax error to stderr */

/* Print command group error and call freecmdgroup */
struct CmdGroup *cmderror(int type, struct CmdGroup *cmdgroup, char *format, ...);

/* GLOBAL VARIABLES */
char *tokentext;		/* Current token string, filled in by yylex() */
struct Hashtable *vartable;	/* Hashtable for shell variables*/
int interactive;		/* Is the shell interactive? */
int showparse;			/* Should we print parse output for this command? */

#endif
