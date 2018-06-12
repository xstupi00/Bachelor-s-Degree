/**************************************************************
 * File:		proj2.h
 * Author:		Šimon Stupinský
 * University: 		Brno University of Technology
 * Faculty: 		Faculty of Information Technology
 * Course:		Operating Systems
 * Date:		23.04.2017
 * Last change:		30.04.2017	
 *		
 * Subscribe:	 	Header file of proj2.c
 *
 **************************************************************/

/**
 * @file proj2.h
 * @brief Header file of proj2.c
 */

#ifndef PROJ2_H
#define PROJ2_H

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include "shr_mem.h"
#include "semaphores.h"
#include "proj2.h"

/**
  @brief Constants for retrieving arguments by 'strtoul'
 */
#define RADIX 10

/**
  @brief File descriptor for output file
 */
FILE *file;

/**
  @brief Variables for arguments assigned by users and identifier adults and children
 */
unsigned A, C, AGT, CGT, AWT, CWT, I_A, I_C;

/**
  @brief Function to working children -> sychronization process
  @param function without parameters
  @return function no return any value
 */
void child_work();

/**
  @brief Function to working adults -> sychronization process
  @param function without parameters
  @return function no return any value
 */
void adult_work();

#endif
