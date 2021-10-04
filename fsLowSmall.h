/**************************************************************
* Class:  CSC-415
* Name: Professor Bierman
* Student ID: N/A
* Project: Basic File System
*
* File: fsLowSmall.h
*
* Description: This file provides the ability to read and write 
*   Logical Blocks and is the main interface for the file system
*   project.  
*   Note that the layer uses one more block than that presented
*   to the file organization module.  This block is used to 
*   hold the partition information and is not accessible from
*   any other layer.  But, when you use the hexdump utility
*   you will see that the first block is not part of the volume.
*
*   The file created by this layer represents the physical hard 
*	drive.  It presents to the logical layer (your layer) as just
*	a logical block array (a series of blocks - nominally 512 bytes,
*	that the logical layer can utilize). 
*
*	It is imperative that the logical layer (your layer) first
*	call startPartitionSystem before using any function and when
*	finished calls closePartitionSystem() to ensure that the 
*	file that represents the physical drive is properally closed.
*
**************************************************************/

#ifndef uint64_t
typedef u_int64_t uint64_t;
#endif
#ifndef uint32_t
typedef u_int32_t uint32_t;
#endif
typedef unsigned long long ull_t;


typedef struct fileInfo {
	char fileName[64];
	int fileSize;
	int location;
} fileInfo;

fileInfo * GetFileInfo (char * fname);


//uint64_t LBAwrite (void * buffer, uint64_t lbaCount, uint64_t lbaPosition);

uint64_t LBAread (void * buffer, uint64_t lbaCount, uint64_t lbaPosition);

