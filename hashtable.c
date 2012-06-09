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
  See high-level description in hashtable.h.
*/

/* USER INCLUDES */
#include "hashtable.h"

/* SYSTEM INCLUDES */
#include <stdlib.h>	/* malloc, free, NULL */
#include <string.h>	/* strdup, strcmp */
#include <stdio.h>	/* printf */

/* USER DEFINES */
#define HASHSIZE 199	/* Can be any prime; generally, larger == less chaining */

/* FORWARD DECLARATIONS */
char *strdup(char *);

/*
  Returns a pointer to a newly allocated
  Hashtable struct or NULL on failure.
  Initializes Pair **entries.
*/
struct Hashtable *newhashtable(){
	struct Hashtable *table = (struct Hashtable*)malloc(sizeof(struct Hashtable));
	if(table == NULL){
		return NULL;
	}
	else{
		table->entries = (struct Pair**)malloc(sizeof(struct Pair) * HASHSIZE);
		if(table->entries == NULL){
			return NULL;
		}
		else{
			return table;
		}
	}
}

/*
  Nice little debugging function. Prints
  each key-value pair along with the
  hash value of said key.
*/
void printtable(struct Hashtable *table){
	unsigned i;
	struct Pair *node;
	printf("Hashtable at %p:\n", table);
	for(i = 0; i < HASHSIZE; ++i){
		if(table->entries[i] != NULL){
			for(node = table->entries[i]; node != NULL; node = node->next){
				printf("[%03d] %s -> %s\n", i, node->key, node->value);
			}
		}
	}
}

/*
  Simple, fast hash. For this assignment,
  that's all that really matters. A few
  quick tests showed that collisions shouldn't
  be a huge problem.
*/
unsigned hash(char *key){
	unsigned hashval;
	for(hashval = 0; *key != '\0'; ++key){
		hashval = *key + 31 * hashval;
	}
	return hashval % HASHSIZE;
}

/*
  Returns the requested key-value pair or NULL if
  the key does not exist in the table.
*/
struct Pair *find(struct Hashtable *table, char *key){
	struct Pair *node;
	for(node = table->entries[hash(key)]; node != NULL; node = node->next){
		if(strcmp(key, node->key) == 0){
			return node;
		}
	}
	return NULL;
}

/*
  Returns the newly added key-value pair or NULL if
  for some reason no more memory could be allocated.
*/
struct Pair *add(struct Hashtable *table, char *key, char *value){
	unsigned hashval = hash(key);
	struct Pair *node;
	struct Pair *tail;
	if((node = find(table, key)) == NULL){
		/* Just walking the linked list if it exists... */;
		for(node = table->entries[hashval]; node != NULL; tail = node, node = node->next)
			/* Pointer to last node in tail */;
		if((node = (struct Pair*)malloc(sizeof(struct Pair))) == NULL){
			return NULL;
		}
		else{
			if(table->entries[hashval] == NULL){
				/* First entry at this hashval */
				table->entries[hashval] = node;
			}
			else{
				/* Collision - chain new node on the end of the list */
				tail->next = node;
			}
			if((node->key = strdup(key)) == NULL){
				return NULL;
			}
		}
	}
	else{
		/* Replace an existing key's value */
		free(node->value);
	}
	if((node->value = strdup(value)) == NULL){
		return NULL;
	}
	else{
		return node;
	}
}
