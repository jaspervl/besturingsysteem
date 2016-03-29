/** @file Command.cc
 * Implementation of class Command.
 */
#include <iostream>
#include <cstdlib>		// for: getenv()
#include <unistd.h>		// for: getcwd(), close(), execv(), access()
#include <limits.h>		// for: PATH_MAX
#include <fcntl.h>		// for: O_RDONLY, O_CREAT, O_WRONLY, O_APPEND
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include "asserts.h"
#include "unix_error.h"
#include "Command.h"
using namespace std;


// Iff PATH_MAX is not defined in limits.h
#ifndef	PATH_MAX
# define	PATH_MAX (4096)	/* i.e. virtual pagesize */
#endif


Command::Command()
	: append(false)
{
}


void	Command::addWord(string& word)
{
	words.push_back(word);
}


void	Command::setInput(std::string& the_input)
{
	require(input.empty());		// catch multiple inputs
	input = the_input;
}

void	Command::setOutput(std::string& the_output)
{
	require(output.empty());	// catch multiple outputs
	output = the_output;
	append = false;
}

void	Command::setAppend(std::string& the_output)
{
	require(output.empty());	// catch multiple outputs
	output = the_output;
	append = true;
}

// A true "no-nothing" command?
bool	Command::isEmpty() const
{
	return input.empty() && output.empty() && words.empty();
}

// Check if the first word of the args got a certain string command enclosed.
bool    Command::hasCommand(std::string command)
{
    for (int i = 0; i < words.size(); ++i) {
        if (words[i] == command) return true;
    }
    return false;
}





// ===========================================================


// Execute a command
void	Command::execute()
{
    // Args opbouwen (uit eerste char van meegegeven woorden).
    char *args[words.size() + 1];
    for (int index = 0; index < words.size(); ++index)
        args[index] = &words[index][0];
    args[words.size()] = 0;





    /// TODO:
    ///close(2), open(2), getcwd(3), getenv(3), access(2), execv(2), exit(2)


    // TODO
    if (hasOutput()) {

        char *fileName = (char*)output.c_str();
        int fd;

        if(!append) {
            fd = open(fileName, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
        } else {
            fd = open(fileName, O_APPEND | O_RDWR, S_IRUSR | S_IWUSR);
        }

        dup2(fd, 1);
        dup2(fd, 2);
        close(fd);
        execvp(args[0], args);


    // TODO
	} else if (hasInput()) {

        char *fileName = (char*)input.c_str();
        int fd = open(fileName, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
        dup2(fd, 0);
        close(fd);
        execvp(args[0], args);


    /// Einde TODO





    // The cd command is performed
	} else if(hasCd()) {
        if (chdir(args[1]) < 0)
            cerr << "> File or directory not recognized" << endl;

    // The pwd command is performed
    } else if (hasPwd()) {
        char cwd[1024];
        getcwd(cwd, sizeof(cwd));
        cerr << "> Current working directory: " << cwd << endl;

    // The ls command is performed
    } else if (hasLs()) {
        DIR *dp;
        struct dirent *ep;

        dp = opendir("./");
        if (dp != 0) {
            while (ep = readdir(dp)) {
                cout << ep->d_name << endl;
            }
            closedir(dp);
        } else {
            cout << "> Something went wrong while reading the directory" << endl;
        }

    // The clear command is performed
    } else if (hasClear()) {
        system("clear");

    // The exit command is performed
    } else if (hasExit()) {
        exit(0);

    } else {
        cerr << "> Command not recognized, try again" << endl;
    }


	// TODO:	Handle I/O redirections.
	//			Don't blindly assume the open systemcall will always succeed!
	// TODO:	Convert the words vector<string> to: array of (char*) as expected by 'execv'.
	//			Note: In this case it safe to typecast from 'const char*' to 'char *'.
	//			Note: Make sure the last element of that array will be a 0 pointer!
	// TODO:	Determine the path of the program to be executed.
	// 			If the name contains a '/' it already is a path name,
	//				either absolute like "/bin/ls" or relative to the
	//				current directory like "sub/prog".
	// 			Otherwise it is the name of an executable file to be
	// 				searched for using the PATH environment variable.
	// TODO:	Execute the program passing the arguments array.
	// Also see: close(2), open(2), getcwd(3), getenv(3), access(2), execv(2), exit(2)

	// TODO: replace the code below with something that really works

#if 0	/* DEBUG code: Set to 0 to turn off the next block of code */
	cerr <<"Command::execute ";
	// Show the I/O redirections first ...
	if (!input.empty())
		cerr << " <"<< input;
	if (!output.empty()) {
		if (append)
			cerr << " >>"<< output;
		else
			cerr << " >"<< output;
	}
	// ... now show the command & parameters to execute
	if (words.empty())
		cerr << "\t(EMPTY_COMMAND)" << endl;
	else {
		cerr << "\t";
		for (vector<string>::iterator  i = words.begin() ; i != words.end() ; ++i)
			cerr << " " << *i;
		cerr << endl;
	}
#endif	/* end DEBUG code */
}


// vim:ai:aw:ts=4:sw=4:
