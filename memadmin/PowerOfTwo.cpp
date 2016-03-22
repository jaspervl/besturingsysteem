//#ident	"@(#)PowerOfTwo.cc	2.2	AKK	20140130"

/** @file PowerOfTwo.cc
 *  @brief Implementation of the PowerOfTwo algorihm.
 *  @author Jasper v. Lierop & Niels Jan van de Pol
 *  @version 3.0	2016/03/22
 */

#include "PowerOfTwo.h"
#include "ansi.h"
#include <math.h>
#include <vector>
#include <iostream>

/// Clean up dead stuff.
PowerOfTwo::~PowerOfTwo()
{
	// TODO : delete elements
}

/// Initializes how much memory we own.
void  PowerOfTwo::setSize(int new_size)
{
    // Determine the maximum possible 'power of 2' for the given new size.
    // Combined with MIN_SIZE (minimum needed 'power of 2'
	// this gives the number of indexes inside available_areas.
	int base = (01 << MIN_SIZE);
    int index;
    for (index = MIN_SIZE ; base < new_size ; ++index) {
        base <<= 1; // Bitshift
    }

    // Create all vectors inside the available_areas to hold certain sized Area pointers.
    --index;
    if(index >= MIN_SIZE){
        int arrSize = index - MIN_SIZE + 1;
        for(int i = 0; i < arrSize;i++){
            available_areas.push_back(*new std::vector<Area*>);
        }
    }
	// The given new_size does not fit inside our MIN_SIZE, thus a exception is thrown.
    else {
        throw 1;
    }

	// Eat up all available space by filling the space with blocks of the power of 2, starting from the biggest power.
    int available_size = new_size;
	for (int i = index; i >= MIN_SIZE;i--) {
        int block = pow(2,i);
		if(available_size - block >= 0){
            available_areas.at(i - MIN_SIZE).push_back(new Area(available_size,block));
            available_size -= block;
		}

	}

	// Set the size of the super class attribute.
	Allocator::setSize(new_size - available_size);
}

/// Print the current vectors with free Area pointers for debugging.
void	PowerOfTwo::dump()
{
    int i = 0;
    std::cout << "Available area's:" << std::endl;
    for(auto it : available_areas){
		if (it.size() > 0) {
            std::cout << "Index #" << i << ": [";
			for (int ind = 0 ; ind < it.size() -1 ; ind++) {
				std::cout << *it[ind] << ", ";
			}
			std::cout << *it[it.size() -1] << "]";
		} else {
            std::cout << "Index #" << i << ": [ ]";
		}
		std::cout << std::endl;
		i++;
    }
}


// Application wants 'wanted' memory
Area  *PowerOfTwo::alloc(int wanted)
{
    require(wanted > 0);
    require(wanted <= size);
    std::cout << size << ":" << wanted << std::endl;
    int index = 0;
    int value = (01 << MIN_SIZE);
    while(value < wanted){
        value <<= 1;
        ++index;
    }
    for(int i = index;i != available_areas.size();i++){
        if(available_areas[i].size() > 0){
            Area* area = available_areas[i].back();
            available_areas[i].pop_back();
            // 32
            // 1024* -> 512 / 512* -> 512 / 256 / 256* -> 512 / 256 / 128 / 128* -> 128 / 64 / 64* -> 128 / 64 / 32 / 32*
            while(i > index){
                available_areas[i - 1].push_back(area -> split((01 << (MIN_SIZE + i)) / 2));
                --i;
            }
            dump();
            return area;
        }
    }
    return 0;
}


// Application returns an area no longer needed
void	PowerOfTwo::free(Area *ap)
{
    int index = log2(ap ->getSize()) - MIN_SIZE;
    for(auto a : available_areas.at(index)){
        if(a ->overlaps(ap)){
            throw 1;
        }
    }

   available_areas.at(log2(ap ->getSize()) - MIN_SIZE).push_back(ap);
   dump();
}


// ----- internal utilities -----

// Search for an area with at least 'wanted' memory
Area  *PowerOfTwo::searcher(int wanted)
{
	require(wanted > 0);		// has to be "something",
	require(wanted <= size);	// but not more than can exist,
	//require(!available_areas ->empty());	// provided we do have something to give

	// Search thru all available areas
//	for(ALiterator  i = areas.begin() ; i != areas.end() ; ++i) {
//		Area  *ap = *i;					// Candidate item
//		if(ap->getSize() >= wanted) {	// Large enough?
//			// Yes, use this area;
//			// The 'erase' operation below invalidates the 'i' iterator
//			// but it does return a valid iterator to the next element.
//			ALiterator  next = areas.erase(i);	// Remove this element from the freelist
//			if(ap->getSize() > wanted) {		// Larger than needed ?
//				Area  *rp = ap->split(wanted);	// Split into two parts (updating sizes)
//				areas.insert(next, rp);			// Insert remainder before "next" area
//			}
//			return  ap;
//		}
//	}
//	return  0; // report failure
}



// Update statistics
void	PowerOfTwo::updateStats()
{
//	++qcnt;									// number of 'alloc's
//	qsum  += areas.size();					// length of resource map
//	qsum2 += (areas.size() * areas.size());	// same: squared
}

void PowerOfTwo::report(){

}

// vim:sw=4:ai:aw:ts=4:
