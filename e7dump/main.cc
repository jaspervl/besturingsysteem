/** @file 1.main.cc
 *	Dump information about a file with the 7th edition of Unix filesystem.
 *  @author Jasper van Lierop
 *  @author Niels Jan van de Pol
 */
// Include streams.
#include <iostream>
#include <fstream>
#include <sstream>
// Include custom classes.
#include "ansi.h"
#include "asserts.h"
#include "unix_error.h"
#include "cstr.h"
#include "Device.h"
#include "Block.h"
// Include other libraries.
#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include "math.h"

// Constant spacer string.
const std::string spacer = "****************************************\n";

// Instance of attributes.
std::ofstream file;
int blocksize;

/// Define some methods for later use.
// Write a array to the instance file
template <typename T>
void writeArray(T, int);

// Retrieve the superblock and write information about all free blocks
void	retrieveSuperBlock(Device&);
// Write superblock specific info
void 	writeSuperBlockInfo(Block*);
// Write the information about inodes
void	writeInodeInfo(Device&);
// Write specific directory information about a single inode
void 	writeInodeDirectories(Device&, dinode*, std::vector<daddr_x>, ino_x);
// Write indirection block info
void    writeIndirectionBlock(Device&, Block*, int, int);



///****************************************************
/// Dump some information from the given "device",
/// Will be called first by main method.
///****************************************************
void	dump(const char* floppie)
{
	// Open output file.
	file.open("floppieDump.txt");

	// Write first line to file.
	file << "Opening: \"" << floppie << "\"." << std::endl;
	file << spacer;

	// Init device with floppie file.
	Device device(floppie);
	// Retrieve information about the superblock with the floppie.
	retrieveSuperBlock(device);
	// Retrieve the information about the inodes with the floppie.
	writeInodeInfo(device);
	// Close and finish the output file.
	file.close();
}

///****************************************************
/// Get the superblock and print the information about it.
///	Also print information about the list of free blocks.
///****************************************************
void	retrieveSuperBlock(Device& device)
{
	// Fetch the block containing the super-block
	Block*  superblock = device.getBlock(SUPERB);

	// Save data from superblock.
	daddr_x isize  = superblock->u.fs.s_isize;
	daddr_x fchain = superblock->u.fs.s_free[0];

	// Print info about the superblock.
	writeSuperBlockInfo(superblock);

	file << spacer;
	file << "Free blocks list starts at block " <<  fchain << "." << std::endl;
	file << spacer;

	// Print all information about the free blocks
	while (fchain != 0) {
		Block* freeBlock = device.getBlock(fchain);
		file << "Free block: " << fchain << ": " << freeBlock->u.fb.df_nfree << std::endl;
		freeBlock->release();
		fchain = freeBlock->u.fs.s_free[-1];
		writeArray(freeBlock->u.fb.df_free, freeBlock->u.fb.df_nfree);
	}

	// Clean up 'connection' with superblock
	superblock->release();
}

///****************************************************
/// Print all the superblock specific information.
///****************************************************
void	writeSuperBlockInfo(Block* superblock)
{
	// Read write strings for more info ;-)
	file << "Dump of superblock on volume: " << cstr(superblock->u.fs.s_fname,6) << "." <<cstr(superblock->u.fs.s_fpack,6) << std::endl;
	file << spacer;
	file << "userdata area starts in block: " << superblock->u.fs.s_isize <<std::endl;
	file << "number of blocks on this volume is: " << superblock->u.fs.s_fsize <<std::endl;
	file << "Number of free blocks: " << superblock->u.fs.s_nfree << std::endl;
	writeArray(superblock->u.fs.s_free, superblock->u.fs.s_nfree);
	file << "Number of free inodes: " << superblock->u.fs.s_ninode << std::endl;
	writeArray(superblock->u.fs.s_inode, superblock->u.fs.s_ninode);
	file << "Freelist lock flag: " << superblock->u.fs.s_flock << std::endl;
	file << "ilist lock flag: " << superblock->u.fs.s_ilock << std::endl;
	file << "superblock-modified: " << superblock->u.fs.s_fmod << std::endl;
	file << "Read only flag: " << superblock->u.fs.s_ronly << std::endl;
	file << "Last update time was: " << ctime(&superblock->u.fs.s_time);
	file << "Total number of free blocks: " << superblock->u.fs.s_tfree << std::endl;
	file << "Total number of free inodes: " << superblock->u.fs.s_tinode << std::endl;
	file << "Interleave factors are: " << "m=" << superblock->u.fs.s_m << " , n=" << superblock->u.fs.s_n << std::endl;
	file << "File system name: " << cstr(superblock->u.fs.s_fname, 6) << std::endl;
	file << "File system pack: " << cstr(superblock->u.fs.s_fpack, 6) << std::endl;
}

///****************************************************
/// Write all info about the root inode and onwards.
///****************************************************
void	writeInodeInfo(Device& device)
{
	// Read inode from device
	Block*  inode_x = device.getBlock(SUPERB);

	// Write number of inodes
	file << spacer;
	ino_x	ninode = (inode_x->u.fs.s_isize - 2) * INOPB;
	file << "Examining " << ninode << " inodes" << std::endl;
	file << spacer;

	// Iterate through all inodes
	for(ino_x  inum = 1; inum < ninode; ++inum) {
		// retrieve the inode
		Block*  inode = device.getBlock( itod(inum) );

		// Using the u.dino view on a block
		// Determine where that inode lives in this bl
		dinode*	dp = &inode->u.dino[ itoo(inum) ];

		if(dp->di_mode != 0) {	// Check if node is being used
			// Check for datablocks
			bool hasData =
				(((dp->di_mode & X_IFMT) == X_IFREG) ||		// a regular file
				 ((dp->di_mode & X_IFMT) == X_IFDIR));		// a directory

			// Data found, write info
			if(hasData) {
				file << std::endl << "Reading inode " << inum << std::endl;
				file << "mode=0" << std::oct << dp->di_mode << std::dec << std::endl;
				file << "nlink=" << dp->di_nlink << " uid=" << dp->di_uid << " gid=" << dp->di_gid << std::endl;
				file << "atime=" << ctime(&dp->di_atime);
				file << "ctime=" << ctime(&dp->di_ctime);
				file << "mtime=" << ctime(&dp->di_mtime);
				blocksize = ceil(dp->di_size / (double) DBLKSIZ);
				file << "size=" << dp->di_size << "(this block uses at most "  <<  blocksize << ")" << std::endl;

				// Convert the 13, 24-bits, blocknumbers in the inode
				// to normal 32-bit daddr_x values (only valid for DIR or REG type!)
				daddr_x  diskaddrs[NADDR];
				Block::l3tol(diskaddrs, dp->di_addr);

				// Local saved addresses of inode (to be build).
				std::vector<daddr_x> addresses;

				// Write all addresses
				file << "address: ";
				for(int i= 0; i < NADDR; ++i) {
					daddr_x address = diskaddrs[i];
					file << address << " ";

					// Save address if not null
					if(address != 0) {
						addresses.push_back(address);
					}
				}
				file << std::endl;
				// Call help function to write directories
				writeInodeDirectories(device, dp, addresses, inum);
			}
		}
		inode->release();
	}
	inode_x->release();
}

///****************************************************
/// Write address information about a certain inode.
///****************************************************
void 	writeInodeDirectories(Device& device, dinode* dp,  std::vector<daddr_x> addresses, ino_x inum)
{
	// TODO: Hergebruik
	daddr_x  diskaddrs[NADDR];
	Block::l3tol(diskaddrs, dp->di_addr);

	int level = 0;
	// Check if dp implicates a directory
	bool isDirectory = (dp->di_mode & X_IFMT) == X_IFDIR;

	// It is a directory, print contents
	if (isDirectory) {
		file << "Direct blocks in inode: ";
		for(daddr_x address : addresses) {
			file << address << " ";
		}
		file << std::endl;
		file << "Contents of directory: " << std::endl;
		for(daddr_x address : addresses) {
			Block* block = device.getBlock(address);
			for(int i =0; i < NDIRENT; ++i) {
				if(block->u.dir[i].d_ino != 0 ) {
					if (isDirectory) {
						file << block->u.dir[i].d_ino << " \t'"  << block->u.dir[i].d_name <<  "'" << std::endl;
					}
				}
			}
			block->release();
		}

	// Not a directory, print direct blocks
	} else {
		if(inum != 1) {
			file << "Direct blocks in inode: ";
			for(int i = 0; i < NADDR; ++i) {
				if(i <=  9) {
					file << diskaddrs[i] << " ";
					blocksize--;
				} else {
					if(diskaddrs[i] != 0) {
						file << std::endl;
						file << "Block number in level " << ++level << " indirection block " << diskaddrs[i] << ": ";
						Block* block = device.getBlock(diskaddrs[i]);
						writeIndirectionBlock(device, block, level-1, blocksize);
						block->release();
					}
				}
			}
		}
		file << std::endl;
	}
}

///****************************************************
/// Write indirection blocks
///****************************************************
void writeIndirectionBlock(Device& device, Block* block, int lvl, int size)
{
	for (int i = 0; i < NINDIR; ++i) {
        if (blocksize ==0) return;
        for (int currLvl = 0; currLvl < lvl; ++currLvl) file << "[";
        int blockNr = block->u.bno[i];
        file << blockNr;
        for (int currLvl = 0; currLvl < lvl; ++currLvl) file << "]";
        file << " ";

        if (lvl > 0 && blockNr == 0) blocksize = blocksize - pow(128, lvl);
        else if (lvl == 0) blocksize--;
		else {
			Block* newBlock = device.getBlock(blockNr);
			writeIndirectionBlock(device, newBlock, lvl-1, blocksize);
			newBlock -> release();
        }
	}
}

///****************************************************
/// Print a array.
///****************************************************
template <typename T>
void  writeArray(T tArray, int end)
{
	for (int i = 0; i < end; i++) {
		file << tArray[i] << ", ";
	}
	file << tArray[end-1] << "." << std::endl;
}

// ====================

// Main is just the TUI
int  main(int argc, const char* argv[])
{
	// This tells cout to add a 0 or 0x prefix
	// when printing numbers in non decimal notation
	std::cout << std::showbase;

	try {
		// Pass a given parameter or use the default ?
		dump((argc > 1) ? argv[1] : "floppie.img");
		return EXIT_SUCCESS;
	} catch(const std::exception& e) {
		std::cerr << AC_RED "OOPS: " << e.what() << AA_RESET << std::endl;
		return EXIT_FAILURE;
	} catch(...) {
		std::cerr << AC_RED "OOPS: something went wrong" AA_RESET << std::endl;
		return EXIT_FAILURE;
	}
}
