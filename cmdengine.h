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
  cmdengine.h/c: Interprets a CmdGroup and executes
  its individual commands. Also handles wiring up
  anonymous pipes and file redirection. Entrypoint is
  runcmdgroup.
*/

#ifndef CMDENGINE_H
#define CMDENGINE_H

/* USER INCLUDES */
#include "cmdgroup.h" 	/* struct CmdGroup */

/* SYSTEM INCLUDES */
#include <sys/types.h>	/* pid_t */

/* Interpret a CmdGroup and execute its commands */
void runcmdgroup(struct CmdGroup *cmdgroup);

/* The first command must open a write pipe and check filein */
pid_t firstcmd(struct CmdGroup *cmdgroup, int *writepipe);

/* A single command just checks file redirection */
pid_t singlecmd(struct CmdGroup *cmdgroup);

/* A middle command must read from read pipe and open a new write pipe */
pid_t middlecmd(struct CmdGroup *cmdgroup, int *writepipe, int *readpipe);

/* The last command must read from the readpipe and check fileout */
pid_t lastcmd(struct CmdGroup *cmdgroup, int *readpipe);

/* Exec the command and replace the process */
void runcmd(char *shortcmd, char *fullcmd, char **argv);

/* Wait for each pid unless CmdGroup->wait is False */
void waitforcmds(pid_t *pids, int count);

#endif
