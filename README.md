# psh - "the pipe shell"
------------------------

## Invocation
    [$ make]
	$ ./psh

## Built-in commands
The pipe shell, or `psh`, is a simplified shell for use on UNIX-like systems. The following are built-in commands:

### exit - Halt the shell
	psh $ exit
	psh $ CTRL-D

### setvar - Set key to value
	psh $ setvar $SOMEVAR "somestring"
	psh $ setvar $SOMEVAR $SOMEOTHERVAR

### cd - Change directory
	psh $ cd path/to/directory
	psh $ cd $SOME_DIR_VAR

Note that the pipe shell displays the current leaf directory to the user like so:

	[currentdir] psh $

This has been omitted from the documentation to reduce clutter.

## User commands
Users may use an absolute path to the command, a path relative to ./ or ../, or a command that can be found by searching through directories in the `$PATH` variable. The default `$PATH` is:

    /bin
    /usr/bin
    /usr/local/bin
    /usr/local/gnu/bin
    /usr/local/sbin
    /usr/sbin
    /sbin
    /usr/games

The user, of course, can set his or her own path using setvar. Another built-in shell variable is `$ShowParseOutput`, which allows diagnostic information to be printed about parsing when set to `"1"`. Initially, this is set to `"0"`.

User Commands may provide arguments in the form of strings, quoted strings, and shell variables.

Multiple user commands can be chained together using pipes like so:

    psh $ cmd1 | cm2 | cmd3

In this case, the output of cmd1 would be piped as the input of cmd2, whose output would be piped to cmd3.

`Stdin` and `stdout` can be redirected to other files like so:

    psh $ cmd < filein > fileout

Note that one or both can be used, but if both are used, redirecting `stdin` must precede redirecting `stdout`. Filenames can be strings, quoted strings, or variables.

Finally, commands can be sent to the background by typing an ampersand at the end of the command like so:

    psh $ cmd &
    [1354] Sent to background

To summarize:

    usage - psh $ cmd [arg]* [| cmd [arg]* ]* [< filein] [> fileout] [&]

## Pitfalls
Commands cannot start with shell variables. Shell variables must have whitespace on either side. Each of the built-in commands takes an exact number of arguments - no fewer, no more. Strings packed into shell variables are not unpacked as multiple arguments to commands.

## Examples
### Saving arguments in a variable
	psh $ setvar $LSFLAGS "-laG"
	psh $ ls $LSFLAGS

### Storing a directory in a variable
	psh $ setvar $HOME "/mounts/u-zon-d2/ugrad/somestudent"
	psh $ cd $HOME

### Running a command found on the $PATH
	psh $ python -c "print 'hello world!'"

### Running a user command relative to ./
	psh $ ./psh

### Piping data through user commands
	psh $ ps -e | sort | grep -in "psh"

### Redirecting stdin
	psh $ wc < somefile.txt

### Letting a long job complete in the background
	psh $ ls -R / > bigfile.txt &


## See also
`DEVNOTES` has information for those interested in the implementation of the pipe shell.

