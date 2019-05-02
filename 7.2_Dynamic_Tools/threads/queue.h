////////////////////////////////////////////////////////////////////////////////
//
// This File:        queue.h
// Other Files:      queue.c
// Semester:         CS 537 Fall 2018
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _QUEUE_H_
#define _QUEUE_H_

/*
 * queue structure
 */
typedef struct Queue Queue;

/*
 * Allocates memory for Queue structure and queue array.
 * Instantiates all int variables to zero.
 *
 * Returns a pointer to the new Queue structure.
 */
Queue* CreateStringQueue(int size);

/*
 * Places the pointer to the string at the end of the queue.
 * If the queue is full, then the function blocks until there
 * is space available.
 */
void EnqueueString(Queue *q, char *string);

/*
 * Removes a pointer to a string from the beginning of the queue.
 * If the queue is empty, then the function blocks until there is a
 * string placed into the queue.
 *
 * Returns the pointer that was removed from the queue.
 */
char* DequeueString(Queue *q);

/*
 * Prints out the statistics the the queue.
 */
void PrintQueueStats(Queue *q);

#endif
