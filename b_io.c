/**************************************************************
* Class:  CSC-415-03 Fall 2021
* Name: Tony Huang
* Student ID: 915090688
* GitHub UserID: tzphuang
* Project: Assignment 2B – Buffered I/O
*
* File: b_io.c
*
* Description:
*
**************************************************************/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include "b_io.h"
#include "fsLowSmall.h"

#define MAXFCBS 20



typedef struct b_fcb
	{
	fileInfo * fi;;	//holds the low level systems file info
	// Add any other needed variables here to track the individual open file

	//internal file descriptor used to locate item in fcbArray[MAXFCBS]
	int my_internal_file_descriptor;

	//LBA count indicates which 512 block we will be refering to in the logical block array
	int my_LBA_count;

	//LBA position indicates position within the 512 block we want to seek to
	int my_LBA_position;

	//buffer used to store a chunk of data size equal to B_CHUNK_SIZE
	//so program doesnt have to constantly do a lba read for data
	//and grab from buffer instead, as long as it has enough info
	char * buffer;

	//numBytesAvaliablekeeps count of how many bytes that can be used inside the buffer
	uint64_t numBytesAvaliable;

	//bufferBookmark represents a bookmark for this specific file control block's buffer
	//so if we just used 200 bytes, numBytesAvaliable would be numBytesAvaliable - 200 
	//which is buffer[bufferBookmark] to buffer[bufferBookmark +200 -1] 
	//used and the next unused byte is buffer[bufferBookmark + 200]
	uint64_t bufferBookmark;

	} b_fcb;
	
b_fcb fcbArray[MAXFCBS];

int startup = 0;	//Indicates that this has not been initialized

//Method to initialize our file system
void b_init ()
	{
	//init fcbArray to all free
	for (int i = 0; i < MAXFCBS; i++)
		{
		fcbArray[i].fi = NULL; //indicates a free fcbArray
		}
		
	startup = 1;
	}

//Method to get a free FCB element
b_io_fd b_getFCB ()
	{
	for (int i = 0; i < MAXFCBS; i++)
		{
		if (fcbArray[i].fi == NULL)
			{
			return i;		//Not thread safe (but do not worry about this)
			}
		}
	return (-1);  //all in use
	}

// You will create your own file descriptor which is just an integer index into an array
// that you maintain for each open file.  


b_io_fd b_open (char * filename, int flags)
	{
	if (startup == 0) b_init();  //Initialize our system

	//*** TODO ***:  Write open function to return your file descriptor
	//				 You may want to allocate the buffer here as well
	//				 But make sure every file has its own buffer

	printf("beginning of b_open\n");
	//find a free fcb, if none avaliable in "fcbArray" return -1
	int my_curr_FD = b_getFCB();
	if(my_curr_FD == -1){
		return -1;
	}

	//we now have a uninitialized FCB, time create a FCB and initialize the struct elements
	//first malloc a new file control block
	//b_fcb *currentFCB = (struct b_fcb*) malloc(sizeof(struct b_fcb));
	b_fcb* currentFCB = &fcbArray[my_curr_FD];
	//setting fileInfo
	currentFCB->fi = GetFileInfo(filename);

	//setting file descriptor
	currentFCB->my_internal_file_descriptor = my_curr_FD;

	//setting LBA_count/LBA_position to 0 because we havent called LBAread() yet
	currentFCB->my_LBA_count = 0;
	currentFCB->my_LBA_position = 0;

	//setting buffer by mallocing a space of size B_CHUNK_SIZE
	currentFCB->buffer = malloc(B_CHUNK_SIZE);

	//setting numBytesUsed with LBAread
	//my_lba_count is a count of how many blocks I want to read
	//my_lba_position is which block I want to read
	//use file information (fi) to find the beginning position of lba position
	currentFCB->numBytesAvaliable = LBAread(currentFCB->buffer, currentFCB->my_LBA_count, currentFCB->my_LBA_position);
	printf("number of bytes avaliable is: %ld\n", currentFCB->numBytesAvaliable);

	//setting bufferBookmark to first avaliable element in buffer
	//which is 0 since this is a new file control block
	currentFCB->bufferBookmark = 0;

	//setting my_LBA_count to how many chunks I read with LBAread
	currentFCB->my_LBA_count += currentFCB->numBytesAvaliable / B_CHUNK_SIZE;

	//setting my_LBA_position to how far into the last chunk read by LBAread
	currentFCB->my_LBA_position = currentFCB->numBytesAvaliable % B_CHUNK_SIZE;

	//setting fcbArray to hold newly initialized fcb
	fcbArray[my_curr_FD] = *currentFCB;

	//return to user, not the actual file descriptor of the read file but
	//the file descriptor used to acess the fcbArray
	printf("ending of b_open\n");
	return my_curr_FD;
	
	}
	
int b_read (b_io_fd fd, char * buffer, int count)
	{
		//*** TODO ***:  Write buffered read function to return the data and # bytes read
		//               You must use the LBAread and you must buffer the data
		//				 in 512 byte chunks.
		
		if (startup == 0) b_init();  //Initialize our system

		// check that fd is between 0 and (MAXFCBS-1)
		if ((fd < 0) || (fd >= MAXFCBS))
			{
			return (-1); 					//invalid file descriptor
			}
			
		if (fcbArray[fd].fi == NULL)		//File not open for this descriptor
			{
			return -1;
			}	

		printf("beginning of b_read\n");

		int returnCount = 0;

		//check if count is not 0 or negative
		if(count < 0){
			return -1;						//return -1 for negative count
		}
		else if (count == 0)
		{
			return 0;						//return 0 if 0 count
		}
		//if we have more buffer bytes than the count provided, fill the passed in buffer
		//with what we have in our current FCB buffer
		//and then update our current fcb numBytesAvaliable and bufferBookmark
		else if (count > 0 &&  count < fcbArray[fd].numBytesAvaliable){
			printf("inside else if: fill user buffer from fcb buffer\n");
			memcpy(buffer, &fcbArray[fd].buffer[ fcbArray[fd].bufferBookmark ], count);
			fcbArray[fd].numBytesAvaliable -= count;
			fcbArray[fd].bufferBookmark += count;
			returnCount += count;
		}
		//if we have not enough bytes in current fcb buffer to fill user's buffer
		//and the count does not exceed fcb buffer + B_CHUNK_SIZE (this means only need 1 refill)
		//we must dump what we have in the buffer, refill the buffer, and then give remaining bytes
		//from new read in buffer, after we must increment lba_count
		else if (count >= fcbArray[fd].numBytesAvaliable && count < fcbArray[fd].numBytesAvaliable + B_CHUNK_SIZE){
			printf("inside else if: fill user buffer from fcb buffer and refill buffer once\n");
			//dumping what is in fcb buffer into user buffer
			memcpy(buffer, &fcbArray[fd].buffer[ fcbArray[fd].bufferBookmark ], fcbArray[fd].numBytesAvaliable);
			//updating returnCount / count
			returnCount += fcbArray[fd].numBytesAvaliable;
			count -= fcbArray[fd].numBytesAvaliable;
			//refilling fcb buffer
			fcbArray[fd].numBytesAvaliable = LBAread(fcbArray[fd].buffer, fcbArray[fd].my_LBA_count, fcbArray[fd].my_LBA_position);
			fcbArray[fd].my_LBA_count += 1; //updating lbacount as we just called it
			fcbArray[fd].bufferBookmark = 0; //resetting bufferBookmark 
			//using returnCount as a pseudo running bookmark for user's buffer we fill what is needed
			memcpy(&buffer[returnCount], &fcbArray[fd].buffer[ fcbArray[fd].bufferBookmark ], count);
			fcbArray[fd].numBytesAvaliable -= count; //updating avaliable bytes after memcpy
			fcbArray[fd].bufferBookmark += count; //updating bookmark position after memcpy
		}
		//if we need to fill the user's buffer with more than what we have in our current buffer
		//and the next buffer, we just skip filling our buffer and instead
		//we dump what we have, loop through and directly fill the user's buffer in 512 chunks
		//and finally for the last chunk we need to fill, we store in our fcb buffer
		//a new 512 buffer and give the user whatever leftover count they need
		else if (count > fcbArray[fd].numBytesAvaliable + B_CHUNK_SIZE){
			printf("inside else if: fill user buffer from fcb buffer and loop directly reading into user buffer\n");
			//dumping what is in fcb buffer into user buffer
			memcpy(buffer, &fcbArray[fd].buffer[ fcbArray[fd].bufferBookmark ], fcbArray[fd].numBytesAvaliable);
			//updating returnCount / count
			returnCount += fcbArray[fd].numBytesAvaliable;
			count -= fcbArray[fd].numBytesAvaliable;

			//after dumping fcb buffer and updating count to reflect that dump
			//we can find out how many loops we need to directly fill user's buffer
			//and the remainder count
			int loopsNeeded = count / B_CHUNK_SIZE;
			int remainingCountAfterLooping = count % B_CHUNK_SIZE;

			//loop and directly fill user's buffer
			for(int count = 0; count < loopsNeeded; count++){
				//returnCount is being used as a pseduo bookmark to keep position of user's buffer
				returnCount += LBAread(&buffer[returnCount], fcbArray[fd].my_LBA_count, fcbArray[fd].my_LBA_position);
				fcbArray[fd].my_LBA_count++; //updating lba_count since a LBAread was just called
			}

			//fill fcb buffer one last time and fill "count" number of bytes into user buffer
			fcbArray[fd].numBytesAvaliable = LBAread(fcbArray[fd].buffer, fcbArray[fd].my_LBA_count, fcbArray[fd].my_LBA_position);
			fcbArray[fd].bufferBookmark = 0; //resetting bufferBookmark to 0 since we just filled the buffer
			memcpy(&buffer[returnCount], &fcbArray[fd].buffer[ fcbArray[fd].bufferBookmark ], remainingCountAfterLooping);
			//updating bytes valiable/bookmark position/returnCount
			fcbArray[fd].numBytesAvaliable -= remainingCountAfterLooping;
			fcbArray[fd].bufferBookmark += remainingCountAfterLooping;
			returnCount += remainingCountAfterLooping;
		}
		//this should never be activated
		else{
			printf("ERROR: b_read inside else statement\n");
		}
		
		printf("ending of b_read\n");
		return returnCount;
	}
	
void b_close (b_io_fd fd)
	{
	//*** TODO ***:  Release any resources

	printf("beginning of b_close\n");
	//for every malloc there is a free
	//freeing buffer in our file control block, and setting to null
	free(fcbArray[fd].buffer);
	fcbArray[fd].buffer = NULL;

	//freeing file control block, and setting fcb file info to null
	free(&fcbArray[fd]);
	fcbArray[fd].fi = NULL;

	printf("ending of b_close\n");
	return;
	}
