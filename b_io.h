/**************************************************************
* Class:  CSC-415-03 Fall 2021
* Name: Tony Huang
* Student ID: 915090688
* GitHub UserID: tzphuang
* Project: Assignment 2B – Buffered I/O
*
* File: b_io.h
*
* Description: Header file for b_io.c
*
**************************************************************/

#ifndef _B_IO_H
#define _B_IO_H

#define B_CHUNK_SIZE 512
typedef int b_io_fd;

b_io_fd b_open (char * filename, int flags);
int b_read (b_io_fd fd, char * buffer, int count);
void b_close (b_io_fd fd);

#endif

