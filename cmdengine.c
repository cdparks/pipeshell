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
  See high-level description in cmdengine.h.
*/

/* USER INCLUDES */
#include "cmdengine.h"
#include "cmdgroup.h"	/* struct CmdGroup, struct CmdNode */
#include "pshutils.h"	/* generalerror, syntaxerror, cmderror, warning, printparse */

/* SYSTEM INCLUDES */
#include <stdlib.h>	/* malloc, free */
#include <unistd.h>	/* pipe, close, dup2, fork, execv */
#include <sys/wait.h>	/* waitpid */

/* USER DEFINES */
#define REND 0		/* Read end of a pipe */
#define WEND 1		/* Write end of a pipe */

#define GETRPIPE(n) ((n) % 2)		/* even n -> 0, odd n -> 1, for swapping pipes */
#define GETWPIPE(n) (((n) + 1) % 2)	/* even n -> 1, odd n -> 0, for swapping pipes */

/* FORWARD DECLARATIONS */
int fileno(FILE *);

/*
  Run all the commands in a command group and wait on
  them if 'wait' is True.
*/
void runcmdgroup(struct CmdGroup *cmdgroup){
	int p0[2];
	int p1[2];
	int *pipes[] = {p0, p1};
	int *writepipe;
	int *readpipe;

	pid_t pid;
	pid_t *pids = malloc(cmdgroup->length * sizeof(pid_t));

	struct CmdNode *node;
	int count;
	for(count = 0, node = cmdgroup->head; node != NULL; ++count, node = node->next){
		/* Swap read and write pipes every iteration */
		writepipe = pipes[GETWPIPE(count)];
		readpipe  = pipes[GETRPIPE(count)];

		/* First command */
		if(!node->prev){
			/* Followed by more commands */
			if(node->next){
				pid = firstcmd(cmdgroup, writepipe);
			}
			/* Only command */
			else{
				pid = singlecmd(cmdgroup);
			}
		}
		/* Middle command */
		else if(node->next){
			pid = middlecmd(cmdgroup, writepipe, readpipe);
		}
		/* Last command */
		else{
			pid = lastcmd(cmdgroup, readpipe);
		}
		/* ERR: couldn't fork or pipe didn't work */
		if(pid < 0){
			generalerror(NONE, "Could not fork process for '%s'\n", node->cmd);
			free(pids);
			close(readpipe[REND]);
			close(readpipe[WEND]);
			close(writepipe[WEND]);
			close(writepipe[REND]);
		}
		/* In child - run command */
		else if(pid == 0){
			pusharg(node, NULL);
			runcmd(node->cmd, node->fullpath, node->args->elements);
		}
		/* In parent - add child's pid to list */
		else{
			if(node->prev){
				close(readpipe[REND]);
				close(readpipe[WEND]);
			}
			pids[count] = pid;
			if(!cmdgroup->wait){
				printf("[%d] Sent to background\n", pid);
			}
		}
	}
	/* Started all commands successfully; now wait on them */
	if(cmdgroup->wait){
		waitforcmds(pids, count);
	}
	free(pids);
}

/*
  If the current command is the first command in series,
  we need to handle redirecting stdin and opening the
  first writepipe. Once all filehandles are set, we fork
  a new process and wire up the pipe and filein. Returns
  < 0 for error, 0 for child, and > for parent.
*/
pid_t firstcmd(struct CmdGroup *cmdgroup, int *writepipe){
	pid_t pid;
	if(pipe(writepipe) == 0){
		pid = fork();
		if(pid == 0){
			if(cmdgroup->filein){
				dup2(fileno(cmdgroup->filein), STDIN_FILENO);
				fclose(cmdgroup->filein);
				cmdgroup->filein = NULL;
			}
			dup2(writepipe[WEND], STDOUT_FILENO);
			close(writepipe[WEND]);
			close(writepipe[REND]);
		}
		return pid;
	}
	else{
		return -1;
	}
}

/*
  A single command with no predecessors or successors
  just checks file redirection. Forks and returns like
  firstcmd.
*/
pid_t singlecmd(struct CmdGroup *cmdgroup){
	pid_t pid = fork();
	if(pid == 0){
		if(cmdgroup->filein){
			dup2(fileno(cmdgroup->filein), STDIN_FILENO);
			fclose(cmdgroup->filein);
			cmdgroup->filein = NULL;
		}
		if(cmdgroup->fileout){
			dup2(fileno(cmdgroup->fileout), STDOUT_FILENO);
			fclose(cmdgroup->fileout);
			cmdgroup->fileout = NULL;
		}
	}
	return pid;
}

/*
  A command between two commands needs to read from the previous
  write pipe, and open a new pipe to write to. Forks and returns
  like firstcmdn.
*/
pid_t middlecmd(struct CmdGroup *cmdgroup, int *writepipe, int *readpipe){
	pid_t pid;
	if(pipe(writepipe) == 0){
		pid = fork();
		if(pid == 0){
			dup2(readpipe[REND], STDIN_FILENO);
			close(readpipe[REND]);
			close(readpipe[WEND]);
			dup2(writepipe[WEND], STDOUT_FILENO);
			close(writepipe[WEND]);
			close(writepipe[REND]);
		}
		return pid;
	}
	else{
		return -1;
	}
}

/*
  Last command in a series of commands must read
  from the previous write pipe and check for file
  redirection of stdout. Forks and returns like
  firstcmd
*/
pid_t lastcmd(struct CmdGroup *cmdgroup, int *readpipe){
	pid_t pid = fork();
	if(pid == 0){
		if(cmdgroup->fileout){
			dup2(fileno(cmdgroup->fileout), STDOUT_FILENO);
			fclose(cmdgroup->fileout);
			cmdgroup->fileout = NULL;
		}
		dup2(readpipe[REND], STDIN_FILENO);
		close(readpipe[REND]);
		close(readpipe[WEND]);
	}
	return pid;
}

/*
  Once a new process has forked, exec the command
  here.
*/
void runcmd(char *shortcmd, char *fullcmd, char **argv)
{
	int status = execv(fullcmd, argv);
	if(status < 0)
		perror(shortcmd);
	_exit(1);
}

/*
  If cmdgroup->wait is True, wait for each
  pid in pids.
*/
void waitforcmds(pid_t *pids, int count){
	int i, status;
	pid_t pid;
	for(i = 0; i < count; ++i){
		pid = waitpid(pids[i], &status, 0);
		if(pid < 0){
			printf("Error waiting for %d\n", pids[i]);
		}
	}
}
