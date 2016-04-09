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
#include "asserts.h"
#include "unix_error.h"
#include "Command.h"
using namespace std;


// Iff PATH_MAX is not defined in limits.h
#ifndef	PATH_MAX
# define	PATH_MAX (4096)	/* i.e. virtual pagesize */
#endif

const int PIPE_READ = 0;
const int PIPE_WRITE = 1;


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

// Check if a exit command is present.
bool    Command::hasExit()
{
    for (int i = 0 ; i < words.size() ; ++i) {
        std::string w = words[i];
        if(w == "exit")
            return true;
    }
    return false;
}

// Check if a cd command is present.
bool    Command::hasCD()
{
    for (int i = 0 ; i < words.size() ; ++i) {
        std::string w = words[i];
        if(w == "cd")
            return true;
    }
    return false;
}


// ===========================================================


// Execute a command
void	Command::execute()
{
    // args bepalen
    char *args[words.size() + 1];
    for (int index = 0; index < words.size(); ++index)
        args[index] = &words[index][0];
    args[words.size()] = 0;

    if (hasOutput() && hasInput()) {

        // Stackoverflow example. Licht aangepast.
        char *outputFile = (char*)output.c_str();
        int fdO;

        if(append)
            fdO = open(outputFile, O_APPEND | O_WRONLY, S_IWUSR | S_IRUSR);
        else
            fdO = open(outputFile, O_WRONLY | O_CREAT, S_IWUSR | S_IRUSR);

        dup2(fdO, PIPE_WRITE);
        close(fdO);

        char *inputFile = (char*) input.c_str();
				///AKK: Eh? RDWR|CREAT maw "for update" en "create_if_needed" ??
        int fdI = open(inputFile, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
        dup2(fdI, PIPE_READ);
        close(fdI);

        execvp(args[0], args);
        perror("Failed performing in and output");
        exit(EXIT_FAILURE);




    // Output aanwezig
    } else if(hasOutput()) {

        // Stackoverflow example. Licht aangepast.
        char *fileName = (char*)output.c_str();
        int pfd;

        if(append)
            pfd = open(fileName, O_APPEND | O_WRONLY, S_IWUSR | S_IRUSR);
        else
            pfd = open(fileName, O_CREAT | O_WRONLY, S_IWUSR | S_IRUSR);

        // http://codewiki.wikidot.com/c:system-calls:dup2
        dup2(pfd, PIPE_WRITE);
        close(pfd);
        execvp(args[0], args);
        perror("Failed performing output");
        exit(EXIT_FAILURE);

    // Input aanwezig
	} else if (hasInput()) {

        char *fileName = (char*) input.c_str();
				///AKK: Eh? RDWR|CREAT maw "for update" en "create_if_needed" ??
        int pfd = open(fileName, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
        dup2(pfd, PIPE_READ);
        close(pfd);
        execvp(args[0], args);
        perror("Failed writing input");
        exit(EXIT_FAILURE);

    } else if (hasCD()) {


    // Executeer een programma of commando
    } else {
        execvp(args[0], args);
        perror("Failed executing command");
        exit(EXIT_FAILURE);
    }
    cout << "end of command reached!" << endl;
	notreached();
}


// vim:ai:aw:ts=4:sw=4:

