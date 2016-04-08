/** @file Pipeline.cc
 * Implementation of class Pipeline.
 */
#include <iostream>
#include <unistd.h>		// for: pipe(), fork(), dup2(), close()
#include <fcntl.h>		// for: O_RDONLY, O_CREAT, O_WRONLY, O_APPEND
#include <signal.h>		// for: signal, SIG*
#include <stdlib.h>
#include <sys/wait.h>
#include "asserts.h"
#include "unix_error.h"
#include "Pipeline.h"
#include "stdio.h"
using namespace std;

const int PIPE_READ = 0;
const int PIPE_WRITE = 1;

Pipeline::Pipeline()
	: background(false)
{
}


void	Pipeline::addCommand(Command *cp)
{
	require(cp != 0);
	commands.push_back(cp);
}


Pipeline::~Pipeline()
{
	for (vector<Command*>::iterator  i = commands.begin() ; i != commands.end() ; ++i)
		delete  *i;
}


bool	Pipeline::isEmpty()	const
{
	return commands.empty();
}


// Execute the commands in this pipeline in parallel
void	Pipeline::execute()
{
	//cerr << "Pipeline::execute\n";//DEBUG

	// Because we want the shell to wait on the rightmost process only
	// we must created the various child processes from the right to the left.
	// Also see: pipe(2), fork(2), dup2(2), dup(2), close(2), open(2), signal(2).
	// Maybe also usefull for debugging: getpid(2), getppid(2).

	size_t	 j = commands.size();		// for count-down
    int p[2];

	for (vector<Command*>::reverse_iterator  i = commands.rbegin() ; i != commands.rend() ; ++i, --j)
	{
		Command  *cp = *i;
        // Last command
		if (j == 1) {
            cp->execute();
			notreached();	///AKK: added

        } else if (j > 1) {
            if(pipe(p) == -1) {
                perror("Pipe creation failed.");
				///AKK: En daarna ga je vrolijk verder alsof er niets is gebeurt....
			}

            int cid = fork();
            if (cid == 0) {     // Child process
                if (dup2(p[PIPE_WRITE],PIPE_WRITE) < 0)
                    perror("Child has a dup2 error.");
                close(p[PIPE_READ]);
                close(p[PIPE_WRITE]);
            } else if (cid > 0) {   // Parent process
                if (dup2(p[PIPE_READ],PIPE_READ) < 0)
                    perror("Parent has a dup2 error.");
                close(p[PIPE_READ]);
                close(p[PIPE_WRITE]);
                cp->execute();
				notreached(); ///AKK: added
            } else
                perror("Fork creation failed.");

        } else {
            cp->execute();
			notreached(); ///AKK: added
        }
	}
}

// vim:ai:aw:ts=4:sw=4:

