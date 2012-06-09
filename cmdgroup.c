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
  See high-level description in cmdgroup.h
*/

/* USER INCLUDES */
#include "cmdgroup.h"
#include "argv.h"	/* struct Argv */

/* SYSTEM INCLUDES */
#include <stdio.h>	/* printf */
#include <stdlib.h>	/* malloc, free, NULL */
#include <string.h>	/* strdup */

/* FORWARD DECLARATIONS */
char *strdup(char *);

/*
  Return a new CmdGroup or NULL.
*/
struct CmdGroup *newcmdgroup(){
	struct CmdGroup *cmdgroup = malloc(sizeof(struct CmdGroup));
	if(cmdgroup){
		cmdgroup->filein = NULL;
		cmdgroup->fileout = NULL;;
		cmdgroup->head = NULL;
		cmdgroup->tail = NULL;
		cmdgroup->wait = 1;
		cmdgroup->length = 0;
	}
	return cmdgroup;
}

/*
  Return a new CmdNode or NULL.
*/
struct CmdNode *newcmdnode(){
	struct CmdNode *cmdnode = malloc(sizeof(struct CmdNode));
	if(cmdnode){
		cmdnode->next = NULL;
		cmdnode->prev = NULL;
		cmdnode->cmd = NULL;
		cmdnode->fullpath = NULL;
		cmdnode->args = newargv();
		if(!cmdnode->args){
			free(cmdnode);
			cmdnode = NULL;
		}
	}
	return cmdnode;
}

/*
  Get a new CmdNode, push it on the end of the CmdGroup's list,
  and fill node with provided data. Return CmdNode or NULL.
*/
struct CmdNode *pushcmd(struct CmdGroup *cmdgroup, char *cmd, char *fullpath){
	struct CmdNode *cmdnode = newcmdnode();
	if(cmdnode){
		if((cmdnode->cmd = strdup(cmd)) && (cmdnode->fullpath = strdup(fullpath)) && (cmdnode->args = newargv())){
			if(0 == cmdgroup->length++){
				cmdgroup->head = cmdnode;
				cmdgroup->tail = cmdnode;
			}
			else{
				cmdgroup->tail->next = cmdnode;
				cmdnode->prev = cmdgroup->tail;
				cmdgroup->tail = cmdnode;
			}
			pusharg(cmdnode, cmdnode->cmd);
		}
		else{
			freecmdnode(cmdnode);
			cmdnode = NULL;
		}
	}
	return cmdnode;
}

/*
  Push an argument onto the provided CmdNode's argument
  vector. Return argument or NULL.
*/
char *pusharg(struct CmdNode *cmdnode, char *arg){
	if(cmdnode){
		return(push(cmdnode->args, arg));
	}
	else{
		return NULL;
	}
}

/*
  Print each command and its arguments.
*/
void printcmdgroup(struct CmdGroup *cmdgroup){
	int i, j;
	struct CmdNode *node;
	puts(">>>");
	for(i = 1, node = cmdgroup->head; node != NULL; ++i, node = node->next){
		printf("%02d %s at %s\n", i, node->cmd, node->fullpath);
		if(node->args){
			for(j = 0; j < node->args->length; ++j){
				printf("  %s\n", node->args->elements[j]);
			}
		}
	}
	puts("<<<");
}

/*
  Free a CmdNode and its members.
*/
void freecmdnode(struct CmdNode *cmdnode){
	if(cmdnode){
		if(cmdnode->cmd) free(cmdnode->cmd);
		if(cmdnode->fullpath) free(cmdnode->fullpath);
		if(cmdnode->args) freeargv(cmdnode->args);
	}
	free(cmdnode);
}

/*
  Free a CmdGroup and its members.
*/
void freecmdgroup(struct CmdGroup *cmdgroup){
	if(cmdgroup){
		if(cmdgroup->filein){
			fclose(cmdgroup->filein);
			cmdgroup->filein = NULL;
		}
		if(cmdgroup->fileout){
			fclose(cmdgroup->fileout);
			cmdgroup->fileout = NULL;
		}
		struct CmdNode *delnode, *node = cmdgroup->head;
		while((delnode = node)){
			node = delnode->next;
			freecmdnode(delnode);
		}
		free(cmdgroup);
	}
}
