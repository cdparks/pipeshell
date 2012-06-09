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
  pshparse.h/c: Defines the parser 'void pshparse(void)' along
  with a host of functions that (loosely) represent productions
  from the starting symbol to a complete pipe shell command.
  Error handling is fairly reliable, and the syntax is strict;
  unlike in bash, for example, sending garbage as extra arguments
  to shell builtins will result in a syntax error.

  Global variables tokentext, vartable, interactive, and showparse
  are defined in pshutils.
*/

#ifndef PSHPARSE_H
#define PSHPARSE_H

/* PARSING FUNCTIONS */
void pshparse(void);
void start(void);		/*	pshparse: start			*/
void exitstate(void);		/*		exit -> exitstate	*/
void cdstate(void);		/*		cd -> cdstate		*/
void setvarstate(void);		/*		setvar -> setvarstate	*/
void cmdstate(void);		/*		word -> cmdstate	*/
				/*		comment ->		*/
				/*		other -> error		*/

/* HELPER FUNCTIONS */
void initparser(void);		/* set up hashtable, check interactivity */
char *getprompt(void);		/* print prompt if interactive */

#endif
