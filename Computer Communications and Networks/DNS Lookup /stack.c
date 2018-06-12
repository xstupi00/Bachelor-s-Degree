/**************************************************************
 * File:		stack.c
 * Author:		Šimon Stupinský
 * University: 	Brno University of Technology
 * Faculty: 	Faculty of Information Technology
 * Course:	    Computer Communications and Networks
 * Date:		29.03.2018
 * Last change:	09.04.2018
 *
 * Subscribe:	The main module of the data structure Stack.
 *
 **************************************************************/

/**
 * @file    ipk-client.cpp
 * @brief   The main of the data structure Stack
 */

#include "stack.h"

stack_t* S_Init () {

    stack_t* s;
    if( !(s = (stack_t*)malloc(sizeof(stack_t))) )
        exit(EXIT_FAILURE);
    s->top_ptr = NULL;
    s->save_ptr = NULL;
    return s;
}

void S_Push (stack_t *s, void* d) {

    elem_t *new_element; 
    if ( (new_element = (elem_t *) malloc(sizeof(struct elem_t))) == NULL )
        exit(EXIT_FAILURE);

    /// insert new element on top of stack
    new_element->data = d;
    new_element->next_ptr = s->top_ptr;
    s->top_ptr = new_element;
}

void S_Pop (stack_t *s) {

    if ( s->top_ptr != NULL )
        s->top_ptr = s->top_ptr->next_ptr;
}

void* S_Top (stack_t *s) {

    if ( s->top_ptr != NULL )
        return s->top_ptr->data;
    else
        return NULL;
}

int S_Empty (stack_t *s) {

    return ( s->top_ptr != NULL ? 0 : -1 );
}

void S_Copy (stack_t *dst_stack, stack_t *src_stack) {

    while ( !S_Empty(src_stack) ) {
        S_Push(dst_stack, S_Top(src_stack));
        S_Pop(src_stack);    
    }
}

stack_t* S_Save(stack_t *s) {

    s->save_ptr = s->top_ptr;
    return s;
}

stack_t* S_Comeback (stack_t *s) {

    s->top_ptr = s->save_ptr;
    return s;
}