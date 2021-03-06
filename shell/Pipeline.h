#ifndef	_Pipeline_h_
#define	_Pipeline_h_
/** @file Pipeline.h
 * Definition of class Pipeline.
 */


#include <vector>		// std::vector
#include "Command.h"


/** @class Pipeline
 * A group of commands that are to be executed in parallel interconnected by
 * \"<a target='info' href='http://en.wikipedia.org/wiki/Pipeline_(Unix)'>pipes</a>".
 * For instance:
 * @code
     abc | def | ghi | ... | xyz
 * @endcode
 * The standard output of the first process (i.e. "abc")
 * will be connected to the standard input of the second process (i.e. "def").
 * The standard output of the second process (i.e. "def")
 * will be connected to the standard input of the third process (i.e. "ghi").
 * Etc.
 *
 * The shell waits until the rightmost process (i.e. "xyz") ends.
 * The exit status of a pipeline is the exit status
 * of the rightmost process (i.e. "xyz").
 */
class	Pipeline
{

	private:

		// The commands to be executed in parallel
		// interconnected by pipes.
		std::vector<Command*>	commands;

		// Set to true if this is a background pipeline
		bool					background;

	public:

		/// Initialize
		Pipeline();

		/// Cleanup
		~Pipeline();

		/// Append a Command to the pipeline
		/// @param cp The command to be added
		void	addCommand(Command *cp);

		/// Is the pipeline empty?
		/// @return true when empty
		bool	isEmpty()	const;

		/// Set background mode
		void	setBackground()	{ background = true; }

		/// Is this a background pipeline?
		/// @return true is background
		bool	isBackground()	const { return background; }

		/// Executes all the commands stored in 'commands'.
		/// Subsequent processes will be interconnected to each other
		/// via pipes (See: unix manual: pipe(2))
		void	execute();

		/// Check if one of the commands is exit
		inline bool hasExit() {
            for (std::vector<Command*>::iterator it = commands.begin() ; it != commands.end() ; ++it) {
                Command *cmd = *it;
                if (cmd->hasExit())
                    return true;
            }
            return false;
		}

        /// Check if one of the commands is cd
		inline bool hasCD() {
            for (std::vector<Command*>::iterator it = commands.begin() ; it != commands.end() ; ++it) {
                Command *cmd = *it;
                if (cmd->hasCD())
                    return true;
            }
            return false;
		}

		/// Get the directory of a command to complete a cd call
		inline std::string* getDirectory() {
            // Doing double work, but there are almost never many commands
            for (std::vector<Command*>::iterator it = commands.begin() ; it != commands.end() ; ++it) {
                Command *cmd = *it;
                if (cmd->hasCD())
                    return cmd->getDirectory();
            }
            return 0;
		}
};

// vim:ai:aw:ts=4:sw=4:
#endif	/*_Pipeline_h_*/
