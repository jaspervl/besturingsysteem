/** @file Sequence.cc
 * Implementation of class Sequence.
 */
 #include <iostream>
#include <sys/wait.h>		// for: wait(), WIF...(status)
#include <unistd.h>			// for: fork(), nice()
#include <fcntl.h>			// for: O_RDONLY, O_CREAT, O_WRONLY, O_APPEND
#include <signal.h>			// for: signal(), SIG*
#include <cstring>			// for: strsignal()
#include "asserts.h"
#include "unix_error.h"
#include "Sequence.h"
using namespace std;


void  Sequence::addPipeline(Pipeline *pp)
{
	require(pp != 0);
	commands.push_back(pp);
}


Sequence::~Sequence()
{
	for (vector<Pipeline*>::iterator  i = commands.begin() ; i != commands.end() ; ++i)
		delete  *i;
}


bool	Sequence::isEmpty()	const
{
	return commands.empty();
}

// Execute the pipelines in this sequence one by one
void	Sequence::execute()
{
    int status;
	// Execute each pipeline in turn.
	// Also see: fork(2), nice(2), wait(2), WIF...(2), strsignal(3)
	size_t  j = commands.size();			// for count-down
	for (vector<Pipeline*>::iterator  i = commands.begin() ; i != commands.end() ; ++i, --j)
	{
        Pipeline  *pp = *i;
		if (!pp->isEmpty())
		{
            // Pipeline has exit command
            if (pp->hasExit()) {
                cout << "Exiting shell, goodbye." << endl;
                exit(EXIT_SUCCESS);

            // Pipeline has CD command
            } else if (pp->hasCD() != 0) {
                const char *directory = pp->getDirectory()->c_str();
                if (chdir(directory) < 0)
                    perror(directory);

            // Pipeline has any other command
            } else {
                int pid = fork();
                if (pid == 0) {
                    pp->execute();
                    notreached();
                } else if (pid > 0) {
                    if (!pp->isBackground() || j == 1) {
                        int xid = wait (0);
                        close(xid);
                    }
                }
            }
        }
	}	// else ignore empty pipeline
}


// vim:ai:aw:ts=4:sw=4:

