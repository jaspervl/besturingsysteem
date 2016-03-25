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
int blockSize;

// Define some methods for later use.
std::string arrayToStringNAAMVERANDEREN(int[], int);
std::string unsignedarrayToStringNAAMVERANDEREN(uint16_t[], int);
void	retrieveSuperBlock(Device&);
void	retrieveRootInode(Device&);
void    nogGoedeNaamGeven(Device&, Block*, int, int);

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
	retrieveRootInode(device);
	// Close and finish the output file.
	file.close();
}

///****************************************************
/// TODO
///
///****************************************************
void	retrieveSuperBlock(Device& device)
{
    // Fetch the block containing the super-block
	Block*  sp = device.getBlock(SUPERB);

	file << "Dump of superblock on volume: " << cstr(sp->u.fs.s_fname,6) << "." <<cstr(sp->u.fs.s_fpack,6) << std::endl;
	file << spacer;

    file << "userdata area starts in block: " << sp->u.fs.s_isize <<std::endl;
    file << "number of blocks on this volume is: " << sp->u.fs.s_fsize <<std::endl;

    file << "Number of free blocks: " << sp->u.fs.s_nfree << std::endl;
    file << arrayToStringNAAMVERANDEREN(sp->u.fs.s_free, sp->u.fs.s_nfree);

    //TODO hulpmethode voor for loop
    file << "Number of free inodes: " << sp->u.fs.s_ninode << std::endl;
    file << unsignedarrayToStringNAAMVERANDEREN(sp->u.fs.s_inode, sp->u.fs.s_ninode);



    //TODO hulpmethode om 0 om te zetten naar set
	file << "Freelist lock flag: " << sp->u.fs.s_flock << std::endl;
	file << "ilist lock flag: " << sp->u.fs.s_ilock << std::endl;
	file << "superblock-modified: " << sp->u.fs.s_fmod << std::endl;
	file << "Read only flag: " << sp->u.fs.s_ronly << std::endl;

	file << "Last update time was: " << ctime(&sp->u.fs.s_time);
	file << "Total number of free blocks: " << sp->u.fs.s_tfree << std::endl;
	file << "Total number of free inodes: " << sp->u.fs.s_tinode << std::endl;
	file << "Interleave factors are: " << "m=" << sp->u.fs.s_m << " , n=" << sp->u.fs.s_n << std::endl;
	file << "File system name: " << cstr(sp->u.fs.s_fname, 6) << std::endl;
	file << "File system pack: " << cstr(sp->u.fs.s_fpack, 6) << std::endl;

	file << spacer;
	file << "Rest of free list continues in block " <<  sp->u.fs.s_free[0]<< std::endl;
	file << spacer;

	Block* bl = device.getBlock(sp->u.fs.s_free[0]);

	file << "Freeblock " << sp->u.fs.s_free[0] << ": " << bl->u.fb.df_nfree << std::endl;
	file << arrayToStringNAAMVERANDEREN(bl->u.fb.df_free, bl->u.fb.df_nfree) << std::endl;

    while (bl->u.fs.s_free[-1] != 0){
        file << "Freeblock " << bl->u.fs.s_free[-1] << ": " << bl->u.fb.df_nfree << std::endl;
        bl->release();
        bl = device.getBlock(bl->u.fs.s_free[-1]);
        file << arrayToStringNAAMVERANDEREN(bl->u.fb.df_free, bl->u.fb.df_nfree) << std::endl;
    }

    bl->release();
    sp->release();	// We no longer need the super block
}

///****************************************************
/// TODO
///
///****************************************************
std::string  arrayToStringNAAMVERANDEREN(int array[], int stopCondition)
{
    std::stringstream ss;
    for (int i = 0; i < stopCondition; i++)
    {
        ss << array[i] << ' ';
    }
    ss << std::endl;

    return ss.str();
}

///****************************************************
/// TODO
///
///****************************************************
std::string  unsignedarrayToStringNAAMVERANDEREN(uint16_t array[], int stopCondition)
{
    std::stringstream ss;
    for (int i = 0; i < stopCondition; i++)
    {
        ss << array[i] << ' ';
    }
    ss << std::endl;

    return ss.str();

}

///****************************************************
/// TODO
///
///****************************************************
void	retrieveRootInode(Device& device)
{
	// - - - - - - - - - - - - -
	// read INODE's from disk
	// Also see: e7ino.h
	// - - - - - - - - - - - - -
    Block*  ip_x = device.getBlock( SUPERB );
    file << spacer;

    ino_x	ninode = (ip_x->u.fs.s_isize - 2) * INOPB;
	file << "Examining " << ninode << " inodes" << std::endl;

    file << spacer;

    for(ino_x  inum = 1; inum < ninode; ++inum) {
        // Fetch the block containing the root inode
        Block*  ip = device.getBlock( itod(inum) );
        // Going to use the u.dino[] view on the block.
        // Use a pointer variable as a shortcut to the wanted dinode in this block
        dinode*	dp = &ip->u.dino[ itoo(inum) ];



        if(dp->di_mode != 0)	// Is this inode being used ?
        {
            // Does this kind of inode have datablocks?
            bool hasData = (   ((dp->di_mode & X_IFMT) == X_IFREG)	// a regular file
                            || ((dp->di_mode & X_IFMT) == X_IFDIR)	// a directory
                           );
            if(hasData) {
                file << std::endl << "Reading inode " << inum << std::endl;
                file << "mode=0" << std::oct << dp->di_mode << std::dec << std::endl;
                file << "nlink=" << dp->di_nlink << " uid=" << dp->di_uid << " gid=" << dp->di_gid << std::endl;
                file << "atime=" << ctime(&dp->di_atime);
                file << "ctime=" << ctime(&dp->di_ctime);
                file << "mtime=" << ctime(&dp->di_mtime);
                blockSize = ceil(dp->di_size / (double) DBLKSIZ);
                file << "size=" << dp->di_size << "(this block uses at most "  <<  blockSize << ")" << std::endl;

                // Convert the 13, 24-bits, blocknumbers in the inode
                // to normal 32-bit daddr_x values (only valid for DIR or REG type!)
                daddr_x  diskaddrs[NADDR];				// provide some room to store those 13 blocknumbers
                Block::l3tol(diskaddrs, dp->di_addr);	// the data convertion routine


                std::vector<daddr_x> adresses;
                file << "addr: ";
                for(int i= 0; i < NADDR; ++i){
                    daddr_x adres = diskaddrs[i];

                    file << adres << " ";
                    if(adres != 0){
                        adresses.push_back(adres);
                    }
                }
                int level = 0;
                bool isDirectory = (dp->di_mode & X_IFMT) == X_IFDIR;
                file << std::endl;
                if (isDirectory) {
                    file << "Direct blocks in inode: ";
                    for(daddr_x adres : adresses){
                        file << adres << " ";
                    }
                    file << std::endl;
                    file << "Contents of directory: " << std::endl;
                    for(daddr_x adres : adresses){
                    Block* bl = device.getBlock(adres);
                        for(int i =0; i < NDIRENT; ++i){
                            if(bl->u.dir[i].d_ino != 0 ){
                                if (isDirectory){
                                    file << bl->u.dir[i].d_ino << " \t'"  << bl->u.dir[i].d_name <<  "'" << std::endl;
                                }
                            }
                        }
                        bl->release();
                    }
                } else {
                    if(inum != 1){
                        file << "Direct blocks in inode: ";
                        for(int i = 0; i < NADDR; ++i){
                            if(i <=  9){
                                file << diskaddrs[i] << " ";
                                blockSize--;
                            } else {
                                if(diskaddrs[i] != 0){
                                    file << std::endl;
                                    file << "Block number in level " << ++level << " indirection block " << diskaddrs[i] << ": ";
                                    Block* bl = device.getBlock(diskaddrs[i]);
                                    nogGoedeNaamGeven(device, bl, level-1, blockSize);
                                    bl->release();
                                }
                            }
                        }
                    }
                    file << std::endl;
                }
            }
        }

        ip->release();
	}
	ip_x->release();	// We no longer need this inode block
}

///****************************************************
/// TODO
///
///****************************************************
void nogGoedeNaamGeven(Device& device, Block* bl, int lvl, int size)
{

    for(int j = 0; j < NINDIR; ++j){
        if(blockSize == 0){
            //file << "Gaat eruit" << std::endl;
            return;
        } else {
            int temp = bl->u.bno[j];

            for(int i = 0; i < lvl; ++i){
                file << "[";
            }
            file << temp ;
            for(int i = 0; i < lvl; ++i){
                file << "]";
            }
            file << " ";

            if(temp == 0 && lvl > 0){
                blockSize = blockSize-pow(128, lvl);
            } else if(temp != 0 && lvl > 0){
                Block* block = device.getBlock(temp);
                nogGoedeNaamGeven(device, block , lvl -1, blockSize);
                block->release();
            } else {
                --blockSize;
            }


        }

    }
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
