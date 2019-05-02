////////////////////////////////////////////////////////////////////////////////
//
// This File:        queue.c
// Other Files:      queue.h
// Semester:         CS 537 Fall 2018
//
////////////////////////////////////////////////////////////////////////////////

#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

const int QUEUE_SIZE = 10;  // const queue size
/*
 * Structure representing the Queue.
 *
 * queue: array of pointers to char strings
 * length: amount of elements in queue array at a time
 * head: first element in queue
 * tail: last element in queue
 * enqueueCount: count of strings enqueued
 * dequeueCount: count of strings dequeued
 * enqueueBlockCount: count of times enqueue was attempted but blocked
 * dequeueBlockCount: count of times dequeue was attempted but blocked
 */
typedef struct Queue {
        char** array;
        int length;
        int head;
        int tail;
        int enqueueCount;
        int dequeueCount;
        int enqueueBlockCount;
        int dequeueBlockCount;

	sem_t full;
	sem_t empty;
	sem_t mutex;
} Queue;

/*
 * Allocates memory for Queue structure and queue array.
 * Instantiates all int variables to zero.
 *
 * Returns a pointer to the new Queue structure.
 */
Queue *CreateStringQueue(int size) {
	// allocate memory for Queue, if NULL then exit with error.
	Queue *new_Queue = malloc(sizeof(Queue));
	if ( new_Queue == NULL ) {
		fprintf(stderr, "Unable to allocate memory for Queue structure.");
		exit(1);
	}

	// allocate memory for array, if NULL then exit with error.
	char **new_array = malloc(sizeof(char*) * size);
	if ( new_array == NULL ) {
		fprintf(stderr, "Unable to allocate memory for Queue array.");
		exit(1);
	}

	// instantiate Queue struct parameters
	new_Queue->array = new_array;
	new_Queue->length = 0;
	new_Queue->head = 0;
	new_Queue->tail = -1;
	new_Queue->enqueueCount = 0;
	new_Queue->dequeueCount = 0;
	new_Queue->enqueueBlockCount = 0;
	new_Queue->dequeueBlockCount = 0;

	// initialize semaphores
	if(sem_init(&(new_Queue->full), 0, 0)) {
		fprintf(stderr, "Unable to initialize semaphore.");
		exit(1);
	}
	if(sem_init(&(new_Queue->empty), 0, QUEUE_SIZE)) {
		fprintf(stderr, "Unable to initialize semaphore.");
		exit(1);
	}
	if(sem_init(&(new_Queue->mutex), 0, 1)) {
		fprintf(stderr, "Unable to initialize semaphore.");
		exit(1);
	}

	return new_Queue;
}

/*
 * Places the pointer to the string at the end of the queue.
 * If the queue is full, then the function blocks until there
 * is space available.
 */
void EnqueueString(Queue *q, char *string) {
	// check if thread will be blocked based on curr queue size
	if(q->length > 9)
		q->enqueueBlockCount++;
	
	// block if there is no more room in the queue
	sem_wait(&(q->empty));
	// block if another thread is in the critical section
	sem_wait(&(q->mutex));
	
	// update queue
	q->length++;
	q->tail = (q->tail + 1) % 10;
	*(q->array + q->tail) = string;
	q->enqueueCount++;

	// unlock critical section when done
	sem_post(&(q->mutex));
	// allow queue to be dequeued if previously empty
	sem_post(&(q->full));
}

/*
 * Removes a pointer to a string from the beginning of the queue.
 * If the queue is empty, then the function blocks until there is a
 * string placed into the queue.
 *
 * Returns the pointer that was removed from the queue.
 */
char* DequeueString(Queue *q) {
	char *str;  // string to be dequeued
	
	// check if thread will be blocked based on curr queue size
	if(q->length < 1)
		q->dequeueBlockCount++;

	// block if there are no strings in the queue
	sem_wait(&(q->full));
	// block if another thread is in the critical section
	sem_wait(&(q->mutex));

	// update queue
	q->length--;
	str = *(q->array + q->head);
	q->head = (q->head + 1) % 10;
	q->dequeueCount++;

	// unlock critical section
	sem_post(&(q->mutex));
	// allow queue to be enqueued if previously full
	sem_post(&(q->empty));

	return str;

}

/*
 * Prints out the statistics the the queue.
 */
void PrintQueueStats(Queue *q) {
	printf("enqueueCount = %d\ndequeueCount = %d\nenqueueBlockCount = %d\ndequeueBlockCount = %d\n", q->enqueueCount, q->dequeueCount, q->enqueueBlockCount, q->dequeueBlockCount);
}
