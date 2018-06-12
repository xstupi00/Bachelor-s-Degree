/**************************************************************
 * File:		shr_mem.c
 * Author:		Šimon Stupinský
 * University: 		Brno University of Technology
 * Faculty: 		Faculty of Information Technology
 * Course:		Operating Systems
 * Date:		23.04.2017
 * Last change:		1.5.2017	
 *		
 * Subscribe:	 	Module implementing the function needed
 *			to work with shared memmory segment
 *
 **************************************************************/

/**
 * @file shr_mem.c
 * @brief Implementing function to work with shared memmory segment
 */

#include "shr_mem.h"

int set_resources() {
	
	// shared memory segment is allocating
	if( (shared = shmget(IPC_PRIVATE, sizeof(str_counter), IPC_CREAT | 0666)) == -1 )
        	return -1;
	
	if ( !(cptr = (str_counter*) shmat(shared, NULL, 0)) )
		return -1;

	// initializing shared variables
	cptr->number = 1;
	cptr->waiting = 0;
	cptr->leaving = 0;
	cptr->children = 0;
	cptr->adults = 0;
	cptr->adult_count = 0;
	
	return 0;
}


void free_resources() {

	// shared memmory segment is deallocating
    	shmctl(shared, IPC_RMID, NULL);
}
