/**************************************************************
 * File:		semaphores.h
 * Author:		Šimon Stupinský
 * University: 		Brno University of Technology
 * Faculty: 		Faculty of Information Technology
 * Course:		Operating Systems
 * Date:		23.04.2017
 * Last change:		30.04.2017	
 *		
 * Subscribe:	 	Header file of semaphores.c
 *
 **************************************************************/

/**
 * @file semaphores.h
 * @brief Header file of semaphores.c
 */

#ifndef SEMAPHORES_H
#define SEMAPHORES_H

#include <stdio.h>
#include <fcntl.h>
#include <semaphore.h>

sem_t *mutex, *adultQueue, *childQueue, *commonEnd;

/**
  @brief Creates semaphores needed to sychronization process 
  @param function without parameters
  @return If everything is all right returns 0, else returns -1
 */
int create_semaphores();

/**
  @brief Closes and unlinks all created semaphores
  @param function without parameters
  @return function no returns any value
 */					
void clear_semaphores();	  
					  
#endif
