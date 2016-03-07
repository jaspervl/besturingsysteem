//#ident	"@(#)PowerOfTwo.cc	2.2	AKK	20140130"
/** @file PowerOfTwo.cc
 * De implementatie van PowerOfTwo.
 */

#include "PowerOfTwo.h"
#include "ansi.h"

// Clean up dead stuff
PowerOfTwo::~PowerOfTwo()
{
	while (!areas.empty()) {
		Area  *ap = areas.back();
		areas.pop_back();
		delete ap;
	}
}

// Initializes how much memory we own
void  PowerOfTwo::setSize(int new_size)
{
	require(available_areas);					// prevent changing the size when the arrays are initialized
    require(allocated_areas);
	Allocator::setSize(new_size);
    int index;
    while (index = 0 ; size < new_size ; ++index) {
        size <<= 1; // Bitshift
        /// 0 = 1  1 = 2  2 = 4  3 = 8 4 = 16 5 = 32
    }
    if(index >= MIN_SIZE){

    }     index = index - MIN_SIZE + 1;
    else {
        /// If we can't contain the requested size in our minimal area size,throw an exception
        throw 1;
    }
	available_areas = new MemoryStack[index];
	allocated_areas = new MemoryStack[index];

	for (MemoryIterator  i = index; i != 0 ; --i) {
		std::cerr << ' ' << **i;
	}
}

// Print the current freelist for debugging
void	PowerOfTwo::dump()
{
	std::cerr << AC_BLUE << type << "::areas";
	for (ALiterator  i = areas.begin() ; i != areas.end() ; ++i) {
		std::cerr << ' ' << **i;
	}
	std::cerr << AA_RESET << std::endl;
}


// Application wants 'wanted' memory
Area  *PowerOfTwo::alloc(int wanted)
{
	require(wanted > 0);		// has to be "something",
	require(wanted <= size);	// but not more than can exist

	updateStats();				// update resource map statistics

	if(areas.empty()) {		// iff we have nothing
		return 0;    			// give up immediately
	}

	// Search thru all available free areas
	Area  *ap = 0;
	ap = searcher(wanted);		// first attempt
	if(ap) {					// success ?
		return ap;
	}
	if(reclaim()) {			// could we reclaim fragmented freespace ?
		ap = searcher(wanted);	// then make a second attempt
		if(ap) {				// success ?
			return ap;
		}
	}
	// Alas, failed to allocate anything
	//dump();//DEBUG
	return 0;					// inform caller we failed
}


// Application returns an area no longer needed
void	PowerOfTwo::free(Area *ap)
{
	require(ap != 0);
	if (cflag) {
		// EXPENSIVE: check for overlap with all registered free areas
		for(ALiterator  i = areas.begin() ; i != areas.end() ; ++i) {
			check(!ap->overlaps(*i));    // the sanity check
		}
	}
	areas.push_back(ap);	// add discarded "old" object to the end of free list
}


// ----- internal utilities -----

// Search for an area with at least 'wanted' memory
Area  *PowerOfTwo::searcher(int wanted)
{
	require(wanted > 0);		// has to be "something",
	require(wanted <= size);	// but not more than can exist,
	require(!areas.empty());	// provided we do have something to give

	// Search thru all available areas
	for(ALiterator  i = areas.begin() ; i != areas.end() ; ++i) {
		Area  *ap = *i;					// Candidate item
		if(ap->getSize() >= wanted) {	// Large enough?
			// Yes, use this area;
			// The 'erase' operation below invalidates the 'i' iterator
			// but it does return a valid iterator to the next element.
			ALiterator  next = areas.erase(i);	// Remove this element from the freelist
			if(ap->getSize() > wanted) {		// Larger than needed ?
				Area  *rp = ap->split(wanted);	// Split into two parts (updating sizes)
				areas.insert(next, rp);			// Insert remainder before "next" area
			}
			return  ap;
		}
	}
	return  0; // report failure
}


// We have run out of usefull areas;
// Try to reclaim space by joining fragmented freespace
bool	PowerOfTwo::reclaim()
{
	return false;
}

// Update statistics
void	PowerOfTwo::updateStats()
{
	++qcnt;									// number of 'alloc's
	qsum  += areas.size();					// length of resource map
	qsum2 += (areas.size() * areas.size());	// same: squared
}

// vim:sw=4:ai:aw:ts=4:
