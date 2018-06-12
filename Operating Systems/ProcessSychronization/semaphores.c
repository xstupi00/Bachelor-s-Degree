/**************************************************************
 * File:		semaphores.c
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
 * @file semaphores.c
 * @brief Implementing function to work with shared memmory segment
 */

#include "semaphores.h"

int create_semaphores() {
			
	// creating semaphores and control of success 
			
	if( (mutex = sem_open("/xstupi_proj2_mutex", O_CREAT, 0666, 1)) == SEM_FAILED )
		goto error1;

	if( (adultQueue = sem_open("/xstupi_proj2_adultQueue", O_CREAT, 0666, 0)) == SEM_FAILED )
		goto error2;
				
	if( (childQueue = sem_open("/xstupi_proj2_childQueue", O_CREAT, 0666, 0)) == SEM_FAILED )
		goto error3;
	
	if( (commonEnd = sem_open("/xstupi_proj2_commonEnd", O_CREAT, 0666, 0)) == SEM_FAILED )
		goto error4;

	return 0;
	
	// closing semaphores in the cases of unsuccessfully creating	
	
	error4:
		sem_close(commonEnd);
		sem_unlink("/xstupi_proj2_commonEnd");
	error3:
		sem_close(childQueue);
		sem_unlink("/xstupi_proj2_childQueue");
	error2:
		sem_close(adultQueue);
		sem_unlink("/xstupi_proj2_adultQueue");
	error1:
		sem_close(mutex);
		sem_unlink("/xstupi_proj2_mutex");
		
	return -1;						
}

// function closes and unlinks all semaphores
void clear_semaphores() {
	
	// closing and unlinks all semaphores and control of success	
	
	if (sem_close(mutex) || sem_close(adultQueue) ||
	    sem_close(childQueue) || sem_close(commonEnd) ||
	    sem_unlink("/xstupi_proj2_mutex") || sem_unlink("/xstupi_proj2_adultQueue") ||
	    sem_unlink("/xstupi_proj2_childQueue") || sem_unlink("/xstupi_proj2_commonEnd") )
		perror("Some errors have been caused with clearing semaphores");
						
}
