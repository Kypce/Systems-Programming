# Unix Custom LS Command

This project implements the Unix LS command with custom flags and formatting.

### Overview
- The LS command in Unix systems is used to display all the available files/directories in your current file location
- This project copies the original function with a custom formatting and three flags.
	- a_flag => also shows hidden files
	- F_flag => shows types of files
	- s_flag => sorts by name

### How to Run
- The program can be placed in any directory though for easy testing, place in a location with other files/directories.
- Compile the code with the command 'gcc -o myls myls.c' (where 'myls' is whatever name you would like the exectuable to be).
- Run the executable by typing 'myls [flags] [directory]'.
	- [flags] == add any flags you would like (i.e., a, F, or s) with a '-' in front.
	- [directory] == write the full path of the directory, for easy testing you can type '`pwd`' to run the program in your current directory.

- Example: myls -aF `pwd`
	- This will display hidden files and the types of files in my current working directory
