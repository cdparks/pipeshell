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
  cmdparser.h/c: User commands are more complex than the shell
  builtins, so this module defines a parser specifically for
  parsing them. Entrypoint is parsecmd, which returns a CmdGroup
  that can be interpreted by runcmdgroup in cmdengine.h/c.

  Global variables tokentext, vartable, interactive, and showparse
  are defined in pshutils.
*/

#ifndef CMDPARSER_H
#define CMDPARSER_H

/* USER INCLUDES */
#include "cmdgroup.h"	/* struct CmdGroup, struct CmdNode */

/*
  Commands look like this:
  cmd [arg]* [| cmd [arg]* ]* [< filein] [> filein] [&]

  parsecmd:
		parseargs

  parseargs:
		parseargs	// Implicit, actually a while loop
		| pipestate
		| redirinstate
		| rediroutstate
		| waitstate
		| NULL

  pipestate:
		parsecmd

  redirinstate:
		rediroutstate
		| waitstate
		| NULL

  rediroutstate:
		waitstate
		| NULL

*/

struct CmdGroup *parsecmd(struct CmdGroup *cmdgroup);
struct CmdGroup *parseargs(struct CmdGroup *cmdgroup, struct CmdNode *cmdnode);
struct CmdGroup *pipestate(struct CmdGroup *cmdgroup);
struct CmdGroup *redirinstate(struct CmdGroup *cmdgroup);
struct CmdGroup *rediroutstate(struct CmdGroup *cmdgroup);
struct CmdGroup *waitstate(struct CmdGroup *cmdgroup);
char *searchpath(char *cmd);

#endif

