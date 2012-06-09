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
  pshlex.l: Defines a scanner that tokenizes the
  input for the var shell. yylex() is defined here
  and called by pshparse() for each atomic token
  in the input. Note that yylex relies on tokentext,
  which must be supplied elsewhere. Note that all
  tokens (including 'static' ones like exit) are
  strdup'ed. This means everything can be freed
  safely.
*/

%{
/* USER INCLUDES */
#include "pshutils.h"

/* SYSTEM INCLUDES */
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* FORWARD DECLARATIONS */
char *strdup(char *);
int fileno(FILE *);

/*
  Called when we run into EOF. No EOL,
  so print one to make output line up.
*/
int yywrap(void){
	return 1;
}

/* Current token, declared in vshparse.h */
extern char *tokentext;

/* Is stdin a tty or redir/pipe? */
extern int interactive;
%}

/*
  Ideally, the meta characters and word rules
  could have been prettied up using definitions
  here, but using :thing: character classes
  seems to break everything.

  Note that tokentext gets the raw pointer to
  yytext. If the parser needs to "keep" the
  char string around, it needes to strdup and
  then free it.
*/

%option noinput nounput
%%

exit {
	tokentext = yytext;
	return EXIT;
}

cd {
	tokentext = yytext;
	return CD;
}

setvar {
	tokentext = yytext;
	return SETVAR;
}

[|] {
	tokentext = yytext;
	return PIPE;
}

[&] {
	tokentext = yytext;
	return WAIT;
}

[<] {
	tokentext = yytext;
	return REDIRIN;
}

[>] {
	tokentext = yytext;
	return REDIROUT;
}

[%].* {
	tokentext = yytext;
	return COMMENT;
}

"\""[^"\""\n]*"\"" {
	/* Quoted string, no nesting */
	++yytext;
	yytext[strlen(yytext) - 1] = '\0';
	tokentext = yytext;
	return STRING;
}

"$"[[:^space:]]{-}[|%<>&]+ {
	/* shell vars look like $words */
	tokentext = yytext;
	return VARIABLE;
}

[[:^space:]]{-}[|%<>&]+ {
	/* words: contiguous nonspace characters minus metachars */
	tokentext = yytext;
	return WORD;
}

[ \t]+ /* Extra whitespace */;

<<EOF>> {
	/* End-Of-Line */
	tokentext = "EOL";
	return EOL;
}

. /* Anything else */;
%%
