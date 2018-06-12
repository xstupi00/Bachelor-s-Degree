/**************************************************************
 * File:		stack.h
 * Author:		Šimon Stupinský
 * University: 	Brno University of Technology
 * Faculty: 	Faculty of Information Technology
 * Course:	    Computer Communications and Networks
 * Date:		29.03.2018
 * Last change:	09.04.2018
 *
 * Subscribe:	The header module of the data structure Stack.
 *
 **************************************************************/

/**
 * @file    ipk-client.cpp
 * @brief   The header of the data structure Stack
 */

#ifndef STACK_H
#define STACK_H

#include <stdio.h>
#include <stdlib.h>

/**
 * @brief      This structure represents item of the stack
 */
typedef struct elem_t {
    void *data;					///< pointer to stack data
    struct elem_t *next_ptr;	///< pointer to next item
} elem_t;

/**
 * @brief      This structure represents the stack
 */
typedef struct stack_t {
    elem_t *top_ptr;			///< pointer to the stack top
    elem_t *save_ptr;
} stack_t;

/**
 * @brief      The function creates and initialize stack
 *
 * @return     Function returns pointer to newly created stack
 */
stack_t* S_Init ();

/**
 * @brief      The function creates new stack item set data
 * 			   and insert it to the stack
 *
 * @param      s     the stack where we want to store new item
 * @param      data  the data to be stored
 */
void S_Push (stack_t *s, void* data);

/**
 * @brief      The function removes the item from top of the stack
 *
 * @param      s     the stack from which we want to remove the item
 */
void S_Pop (stack_t *s);

/**
 * @brief      The function returns item from top of the stack (don't remove it)
 *
 * @param      s     the stack from which we want to get the item
 *
 * @return     the function returns pointer to data of the item on top of the stack
 */
void* S_Top (stack_t *s);

/**
 * @brief      The function tells us if the stack is empty
 *
 * @param      s     pointer to stack
 *
 * @return     the function returns -1 if stack is empty and 0 if not
 */
int S_Empty (stack_t *s);

/**
 * @brief      The function prints the stack's items
 *
 * @param      s     pointer to stack
 */
void S_Print (stack_t *s);

/**
 * @brief      The function copies data from src_stack to dst_stack
 *
 * @param      dst_stack  the destination stack
 * @param      src_stack  the source stack
 */
void S_Copy (stack_t *dst_stack, stack_t *src_stack);

/**
 * @brief       The function saved the pointer, which is actually
 *              on the top of the Stack.
 *
 * @param s     pointer to stack, which will be saved
 *
 * @return      the function returns pointer on the top of the stack, with saved
 *              position on the actual top
 */
stack_t* S_Save (stack_t *s);

/**
 * @brief       The function shift pointer on the top of the stack, back to
 *              saved position in its structure
 *
 * @param s     pointer to stack, in which will be apply the change
 *
 * @return      the function returns actualized stack
 */
stack_t* S_Comeback (stack_t *s);

#endif // STACK_H
