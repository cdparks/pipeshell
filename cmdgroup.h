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
  cmdgroup.h/c: cmdgroup defines a data structure CmdGroup
  which is made up of CmdNodes. Each node has a user command's
  short name, full path, and argument vector. A CmdGroup also
  defines filein and fileout in the case of redirection. A
  CmdGroup is built by cmdparse in parsecmd.h/c and interpreted
  by runcmdgroup in cmdengine.h/c.
*/

#ifndef CMDGROUP_H
#define CMDGROUP_H

#include "argv.h"	/* struct Argv */
#include <stdio.h>	/* struct FILE */

struct CmdNode{
	struct CmdNode *next;
	struct CmdNode *prev;
	char *cmd;
	char *fullpath;
	struct Argv *args;
};

struct CmdGroup{
	FILE *filein;
	FILE *fileout;
	struct CmdNode *head;
	struct CmdNode *tail;
	int wait;
	int length;
	char *error;
};

/* CONSTRUCT/DESTRUCT CMDGROUP */
struct CmdGroup *newcmdgroup();
void freecmdgroup(struct CmdGroup *cmdgroup);

/* CONSTRUCT/DESTRUCT CMDNODE */
struct CmdNode *newcmdnode();
void freecmdnode(struct CmdNode *cmdnode);

/* Push a new command on the CmdGroup list tail */
struct CmdNode *pushcmd(struct CmdGroup *cmdgroup, char *cmd, char *fullpath);

/* Push a new argument on the CmdNode argument vector */
char *pusharg(struct CmdNode *cmdnode, char *arg);

/* For debugging */
void printcmdgroup(struct CmdGroup *cmdgroup);

#endif
