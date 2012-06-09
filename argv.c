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
  See high-level description in argv.h.
*/

/* USER INCLUDES */
#include "argv.h"

/* SYSTEM INCLUDES */
#include <stdlib.h>	/* malloc, realloc, NULL */
#include <string.h>	/* strdup */

/* USER DEFINES */
#define STARTSIZE 10

/* FORWARD DECLARATIONS */
char *strdup(char *);

/*
  Return pointer to a new struct Argv.
*/
struct Argv *newargv(void){
	struct Argv *temp = (struct Argv*)malloc(sizeof(struct Argv));
	temp->length = temp->capacity = 0;
	return temp;
}

/*
  Push char *string on the end of arg->elements.
  Returns string on success, NULL otherwise.
*/
char *push(struct Argv *argv, char *string){
	char *newstring;
	if(argv->capacity == argv->length){
		char **temp;
		if(argv->capacity == 0){
			/* initial allocation */
			temp = (char**)malloc((argv->capacity = STARTSIZE) * sizeof(char*));
		}
		else{
			/* reallocate, grew too large */
			temp = (char**)realloc(argv->elements, (argv->capacity *= 2) * sizeof(char*));
		}
		if(temp == NULL){
			return NULL;
		}
		else{
			argv->elements = temp;
		}
	}
	if(string == NULL){
		argv->elements[argv->length++] = (char *)NULL;
		return (char *)1;
	}
	else if((newstring = strdup(string)) != NULL){
		argv->elements[argv->length++] = newstring;
		return newstring;
	}
	else{
		return NULL;
	}
}

/*
  Frees each char *string pushed onto
  argv->elements
*/
void freeargv(struct Argv *argv)
{
	int i;
	for(i = 0; i < argv->length; i++){
		if(argv->elements[i]){
			free(argv->elements[i]);
		}
	}
	free(argv);
}
