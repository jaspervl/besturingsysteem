//#ident	"@(#)PowerOfTwo.cc	2.2	AKK	20140130"
/** @file PowerOfTwo.cc
 * De implementatie van PowerOfTwo.
 */

#include "PowerOfTwo.h"
#include "ansi.h"
#include <math.h>
#include <vector>

// Clean up dead stuff
PowerOfTwo::~PowerOfTwo()
{
	// TODO : delete elements
}

// Initializes how much memory we own
void  PowerOfTwo::setSize(int new_size)
{
	require(available_areas);					// prevent changing the size when the arrays are initialized
	Allocator::setSize(0);
	int base = (01 << MIN_SIZE);
    int index;
    for (index = MIN_SIZE ; base < new_size ; ++index) {
        base <<= 1; // Bitshift
        /// 0 = 1  1 = 2  2 = 4  3 = 8 4 = 16 5 = 32
    }
    --index;
    if(index >= MIN_SIZE){
        /// vector<Area*> available_areas = new Vector<Area*>[index - MIN_SIZE + 1];
        available_areas = new std::vector<Area*>[index - MIN_SIZE + 1];
    }
    else {
        /// If we can't contain the requested size in our minimal area size,throw an exception
        throw 1;
    }

    int available_size = 0;
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
		if(new_size - block >= 0){
             available_areas[i].push_back(new Area(available_size,block));
            available_size += block;

		}

	}
}

// Print the current freelist for debugging
void	PowerOfTwo::dump()
{

}


// Application wants 'wanted' memory
Area  *PowerOfTwo::alloc(int wanted)
{
	require(wanted > 0);		// has to be "something",
	require(wanted <= size);	// but not more than can exist

	updateStats();				// update resource map statistics

	if(available_areas ->empty()) {		// iff we have nothing
		return 0;    			// give up immediately
	}

	// Search thru all available free areas
	Area  *ap = 0;
	ap = searcher(wanted);		// first attempt
	if(ap) {					// success ?
		return ap;
	}
//	if(reclaim()) {			// could we reclaim fragmented freespace ?
//		ap = searcher(wanted);	// then make a second attempt
//		if(ap) {				// success ?
//			return ap;
//		}
//	}
	// Alas, failed to allocate anything
	//dump();//DEBUG
	return 0;					// inform caller we failed
}


// Application returns an area no longer needed
void	PowerOfTwo::free(Area *ap)
{
//	require(ap != 0);
//	if (cflag) {
//		// EXPENSIVE: check for overlap with all registered free areas
//		for(ALiterator  i = areas.begin() ; i != areas.end() ; ++i) {
//			check(!ap->overlaps(*i));    // the sanity check
//		}
//	}
//	areas.push_back(ap);	// add discarded "old" object to the end of free list
}


// ----- internal utilities -----

// Search for an area with at least 'wanted' memory
Area  *PowerOfTwo::searcher(int wanted)
{
	require(wanted > 0);		// has to be "something",
	require(wanted <= size);	// but not more than can exist,
	require(!available_areas ->empty());	// provided we do have something to give

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
