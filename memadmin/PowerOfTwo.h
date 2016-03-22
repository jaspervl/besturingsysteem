#pragma once
#ifndef POWEROFTWO_H
#define POWEROFTWO_H

/** @file PowerOfTwo.h
 *  @brief The class that defines the power of two algoritme.
 *  @author Jasper v. Lierop & Niels Jan van de Pol
 *  @version 3.0	2016/03/22
 */

#include "Allocator.h"
#include <vector>


/// @class PowerOfTwo
class PowerOfTwo : public Allocator
{
    public:
        explicit
        /// Ensuring compatibility with the main
        /// @param cflag	initial status of check-mode
        /// @param type		name of this algorithm (default=PowerOfTwo)
        PowerOfTwo(bool cflag=false, const char *type = "PowerOfTwo")
		: Allocator(cflag, type) {}
        ~PowerOfTwo();

		/// initialize memory size
        void	 setSize(int new_size);

        /// Ask for an area of at least 'wanted' units
        /// @returns	An area or 0 if not enough freespace available
        Area	*alloc(int wanted);

        /// The application returns an area to freespace
        /// @param ap	The area returned to free space
        void	free(Area *ap);
        void    report();

    private:

        /// List of all the available free areas, define as nul pointers for initialization and safety precautions
        std::vector<std::vector<Area*>> available_areas;

        /// Define the minimal size of the area ( ex : 1  is 2^1 = 2  || 2 is  2 ^ 2 = 4 ...)
        const int  MIN_SIZE = 5;

        /// For debugging this function shows the free area list
        void	dump();

        /// This is the actual function that searches for space.
        /// @returns	An area or 0 if not enough freespace available
        Area 	*searcher(int);

		/// update resource map statistics
        void	updateStats();
};

#endif // POWEROFTWO_H
