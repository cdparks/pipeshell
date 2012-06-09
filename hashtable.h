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
  hashtable.h/c: Simple implementation of a hashtable mapping
  strings to strings. Collisions are resolved using chaining.
  Not an especially general implementation, but good enough
  for our purposes. Borrowed most ideas from K&R 2nd Edition,
  p144, though with my preferred interface and style conventions.

  Table looks sort of like this:
  array:    [0, 0, key, 0, key, 0, 0, key, 0, 0 ...]
                    |       |          |
  level 1:         val     val        val
                    |
  level 2:         val

  ...

*/

#ifndef HASH_H
#define HASH_H

struct Pair{
	struct Pair *next;
	char *key;
	char *value;
};

struct Hashtable{
	struct Pair **entries;
};

struct Hashtable *newhashtable();
void printtable(struct Hashtable *table);
unsigned hash(char *key);
struct Pair *find(struct Hashtable *table, char *key);
struct Pair *add(struct Hashtable *table, char *key, char *value);

#endif
