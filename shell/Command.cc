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

///AKK: Wat bepaalt deze functie???
///		De opdrachten die deze shell zelf doet?
///		Want dan horen ls, rm, clear, echo etc er niet bij!
bool    Command::hasDirectCommand()
{
	///AKK: En waarom bekijken jullie alle woorden?
	///		Leuk als je "print this is an exit" als opdracht intikt
    for (int i = 0 ; i < words.size() ; ++i) {
        std::string w = words[i];
        if(     w == "cd"
            ||  w == "exit"
            ||  w == "ls"
            ||  w == "pwd"
            ||  w == "rm"
            ||  w == "clear"
            ||  w == "echo")
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

    // Output aanwezig
    if(hasOutput()) {

        // Stackoverflow example. Licht aangepast.
        char *fileName = (char*)output.c_str();
        int pfd;

        if(append)				///AKK: Niet RDWR (update) maar WRONLY (write-only) !
								///		(Overigens, dat is typisch gedrag in een windows omgeving)
            pfd = open(fileName, O_APPEND | O_RDWR, S_IWUSR | S_IRUSR);
        else
            pfd = open(fileName, O_RDWR | O_CREAT, S_IWUSR | S_IRUSR);

        // http://codewiki.wikidot.com/c:system-calls:dup2
        dup2(pfd, PIPE_READ);		///AKK: naar 0 ??? Dit was geen "<input_file" opdracht!
        dup2(pfd, PIPE_WRITE);
        close(pfd);
        execvp(args[0], args);		///AKK: nu al ? dus " a <b >c " kan niet

    // Input aanwezig
	} else if (hasInput()) {

        char *fileName = (char*) input.c_str();
				///AKK: Eh? RDWR|CREAT maw "for update" en "create_if_needed" ??
        int pfd = open(fileName, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
        dup2(pfd, PIPE_READ);
        close(pfd);
        execvp(args[0], args);		///AKK: nu al? dus " a <b >c " kan niet

    // Perform a direct command by delegating it to the appropreate shell ;-)
    /// TODO: Werkt niet goed
	} else if(hasDirectCommand()) {

        if (words[0] == "exit") {
            cerr << "Exiting." << endl;
            exit(0);    // perform exit		///AKK: tja, dat heeft geen effect op het oorspronkelijke proces
			return;	///AKK: return naar waar? wat ga je nu doen?
        }

        int cid = fork();			///AKK: eh? Je bent toch al degene die dit moet doen?
        if (cid > 0) {
            if (words[0] == "cd") {
                if (chdir(args[1]) < 0) {	///AKK: tja, heeft geen effect voor het oorspronkelijke proces
					perror(args[1]);		///AKK: addded, properly inform user about problems
                    cerr << "> File or directory not recognized;" << endl;
				}
                return;	///AKK: return naar waar? wat ga je nu doen?
            }
            execvp(args[0], args);
			perror(args[0]);		///AKK: added
            cerr << "Failed executing direct command." << endl;
            exit(0);
        } else {
            wait(0);
			///AKK: en wat wou je hierna gaan doen ?
        }

    // Open a program	///AKK: Je "opent" geen programma, je "executeer" een programma
    } else {

		cerr << "PATH=" << getenv("PATH") << endl;	///AKK: added
        // www.cplusplus.com/reference/cstdlib/getenv/
        char* path = getenv("PATH");
        strtok(path, ":");
        strcat(path, "/");
        strcat(path, args[0]);
		cerr << "PATH=" << getenv("PATH") << endl;	///AKK: added, want:
					/// A: deze code maak PATH kapot!
					/// B: deze code kijkt alleen naar het 1e element van PATH
					/// C: beter nadenken over de slides! 6.processes: #107
					/// D: wat gebeurt er als je de opdracht: /bin/xyz in tikt? of: ./bin/main ?
					///			m.a.w. de "pathname" is al compleet?

        execv(path, args);
        perror("Failed executing command.\n");
        exit(0);
    }
	notreached(); ///AKK: added
}


// vim:ai:aw:ts=4:sw=4:

