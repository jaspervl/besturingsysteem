#pragma once
#ifndef POWEROFTWO_H
#define POWEROFTWO_H

/** @file FirstFit.h
 *  @brief The class that implements the power of two algoritme
 *  @author Jasper v. Lierop & Niels Jan van de Pol
 *  @version 2.2	2014/01/30
 */

#include "Allocator.h"


typedef vector<stack<Area*>*> MemoryStack;
typedef	MemoryStack::iterator	MemoryIterator;

/// @class PowerOfTwo

class PowerOfTwo : public Allocator
{
    public:
        explicit
        /// Ensuring compatibility with the main
        /// @param cflag	initial status of check-mode
        /// @param type		name of this algorithm (default=PowerOfTwo)
        FirstFit(bool cflag=false, const char *type = "PowerOfTwo")
		: Fitter(cflag, type) {}
        ~PowerOfTwo();


        void	 setSize(int new_size);	///< initialize memory size

        /// Ask for an area of at least 'wanted' units
        /// @returns	An area or 0 if not enough freespace available
        Area	*alloc(int wanted);

        /// The application returns an area to freespace
        /// @param ap	The area returned to free space
        void	free(Area *ap);

    private:
        /// Define the minimal size of the area ( ex : 1  is 2^1 = 2  || 2 is  2 ^ 2 = 4 ...)
        const int  MIN_SIZE = 10;
        /// List of all the available free areas, define as nul pointers for initialization and safety precautions
        MemoryStack *available_areas = 0;
        MemoryStack *allocated_areas = 0;

        /// For debugging this function shows the free area list
        void	dump();

        /// This is the actual function that searches for space.
        /// @returns	An area or 0 if not enough freespace available
        Area 	*searcher(int);

        /// This function is called when the searcher can not find space.
        /// It tries to reclaim fragmented space by merging adjacent free areas.
        /// @returns true if free areas could be merged, false if no adjacent areas exist
        bool	  reclaim();
        void	updateStats();	///< update resource map statistics
};

#endif // POWEROFTWO_H
