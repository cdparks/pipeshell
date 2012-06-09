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
  argv.h/c: Dynamic array with limited interface.
  We only really need to be able to push strings
  on the end, so that's all it does. Reallocates
  char **elements when length == capacity. Initial
  capacity is 10, which should be more than sufficient
  for most user commands.
*/

#ifndef ARGV_H
#define ARGV_H

struct Argv{
	int length;
	int capacity;
	char **elements;
};

struct Argv *newargv(void);
char *push(struct Argv *argv, char *string);
void freeargv(struct Argv *argv);
#endif
