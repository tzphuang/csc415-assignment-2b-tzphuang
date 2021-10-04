/**************************************************************
* Class:  CSC-415-0# Fall 2021
* Name:
* Student ID:
* GitHub UserID:
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
		
	}
	
void b_close (b_io_fd fd)
	{
	//*** TODO ***:  Release any resources
	}
