#  Copyright 2011 Christopher D. Parks
#
#  This file is part of pipe shell.
#
#  Pipe shell is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  Pipe shell is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with pipe shell.  If not, see <http://www.gnu.org/licenses/>.



CC = gcc
CFLAGS = -Wall -std=c89
LEX = flex
TS = Tests/success
TE = Tests/error

psh: psh.c pshutils.o pshparse.o pshlex.o cmdgroup.o cmdparse.o cmdengine.o hashtable.o argv.o
	$(CC) $(CFLAGS) psh.c pshutils.o pshparse.o pshlex.o cmdgroup.o cmdparse.o cmdengine.o hashtable.o argv.o -lreadline -o psh

pshparse.o: pshparse.c pshparse.h pshlex.o
	$(CC) $(CFLAGS) -c pshparse.c

cmdengine.o: cmdengine.c cmdengine.h
	$(CC) $(CFLAGS) -c cmdengine.c	

cmdparse.o: cmdparse.c cmdparse.h pshlex.o
	$(CC) $(CFLAGS) -c cmdparse.c

pshutils.o: pshutils.c pshutils.h
	$(CC) $(CFLAGS) -c pshutils.c

pshlex.o: pshlex.c pshlex.h
	$(CC) $(CFLAGS) -c pshlex.c

pshlex.c: pshlex.l
	$(LEX) -o pshlex.c --header-file=pshlex.h pshlex.l

cmdgroup.o: cmdgroup.c cmdgroup.h argv.h
	$(CC) $(CFLAGS) -c cmdgroup.c

hashtable.o: hashtable.c hashtable.h
	$(CC) $(CFLAGS) -c hashtable.c

argv.o: argv.c argv.h
	$(CC) $(CFLAGS) -c argv.c

test:
	@echo "Testing good input"
	rm -f $(TS).out
	./psh < $(TS).in > $(TS).out
	diff $(TS).out $(TS).base
	@echo

clean:
	rm -f *.o pshlex.c pshlex.h psh
