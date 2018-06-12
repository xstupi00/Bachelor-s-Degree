/**************************************************************
 * File:		shr_mem.h
 * Author:		Šimon Stupinský
 * University: 		Brno University of Technology
 * Faculty: 		Faculty of Information Technology
 * Course:		Operating Systems
 * Date:		23.04.2017
 * Last change:		30.04.2017	
 *		
 * Subscribe:	 	Header file of shr_mem.c.
 *
 **************************************************************/

/**
 * @file shr_mem.h
 * @brief Header file of shr_mem.c
 */

#ifndef SHR_MEM_H
#define SHR_MEM_H

#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

/**
  @struct Counter struct

  @brief Declaration needed shared variables
 */
typedef struct counter {
	unsigned number; 	//< Number of counter action
	unsigned waiting;	//< Number of children waiting to enter
	unsigned leaving;	//< Number of adults waiting to leave
	unsigned adults;	//< Number of adults in the center
	unsigned children;	//< Number of children in the center
	unsigned sum_count;	//< Number of adults and children which have gone
	unsigned adult_count;	//< Number of adults which have gone
} str_counter;			//< Nameless struct variable

/**
  @brief Allocates shared memmory segment
  @param function without parameters
  @return If everything is all right returns 0, else returns -1
 */
int set_resources();

/**
  @brief Deallocates a shared memmory segment
  @param function without parameters
  @return function no return any value
 */
void free_resources();

// declaration counter of shared memmory
int shared;
str_counter *cptr;

#endif
