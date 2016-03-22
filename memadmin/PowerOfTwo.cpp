//#ident	"@(#)PowerOfTwo.cc	2.2	AKK	20140130"
/** @file PowerOfTwo.cc
 * De implementatie van PowerOfTwo.
 */

#include "PowerOfTwo.h"
#include "ansi.h"
#include <math.h>
#include <vector>
#include <iostream>

// Clean up dead stuff
PowerOfTwo::~PowerOfTwo()
{
	// TODO : delete elements
}

// Initializes how much memory we own
void  PowerOfTwo::setSize(int new_size)
{
    // prevent changing the size when the arrays are initialized
	int base = (01 << MIN_SIZE);
    int index;
    for (index = MIN_SIZE ; base < new_size ; ++index) {
        base <<= 1; // Bitshift
        /// 0 = 1  1 = 2  2 = 4  3 = 8 4 = 16 5 = 32
    }

    --index;
    if(index >= MIN_SIZE){
        /// vector<Area*> available_areas = new Vector<Area*>[index - MIN_SIZE + 1];
        int arrSize = index - MIN_SIZE + 1;
        for(int i = 0; i < arrSize;i++){
            available_areas.push_back(*new std::vector<Area*>);
        }

    }
    else {
        /// If we can't contain the requested size in our minimal area size,throw an exception
        throw 1;
    }

    int available_size = new_size;
//    int counter = index;
//	for ( = available_areas ->rbegin(); i != available_areas ->rend()  ; i++) {
//        int block = pow(2,counter--);
//		if(new_size - block >= 0){
//            i -> push_back(new Area(available_size,block));
//            available_size += block;
//		}
//
//	}

	for (int i = index; i >= MIN_SIZE;i--) {
        int block = pow(2,i);
		if(available_size - block >= 0){
            available_areas.at(i - MIN_SIZE).push_back(new Area(available_size,block));
            available_size -= block;
		}

	}

	dump();
	Allocator::setSize(new_size - available_size);
    std::cout << "WANTED : " << new_size << " ACTUAL : " << size << std::endl;

}

// Print the current freelist for debugging
void	PowerOfTwo::dump()
{
    int i = 0;
    std::cout << "ENTERING DUMP" << std::endl;
    for(auto it : available_areas){
            std::cout << "#" << i << ": ";
        for(auto x : it){
            std::cout << *x << ", ";
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
