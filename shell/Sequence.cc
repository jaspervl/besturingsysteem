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
            int pid = fork();
            if(pid > 0)
                wait(0);
            else
                pp->execute();
        }
	}	// else ignore empty pipeline
}


// vim:ai:aw:ts=4:sw=4:

