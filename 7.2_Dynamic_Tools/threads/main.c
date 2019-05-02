////////////////////////////////////////////////////////////////////////////////
//
// This File:        main.c
// Other Files:      queue.c, queue.h
// Semester:         CS 537 Fall 2018
//
////////////////////////////////////////////////////////////////////////////////


#include "queue.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

const int BUFF = 1024;  // const buffer size for strings
const int QUEUE_SIZE_C = 10;  // const size for the queues

void *reader();
void *munch1();
void *munch2();
void *writer();

Queue *munch1_q;  // queue to be used from reader to munch1
Queue *munch2_q;  // queue to be used from munch1 to munch2
Queue *writer_q;  // queue to be used from munch2 to writer

int main() {
	// create queues
	munch1_q = CreateStringQueue(QUEUE_SIZE_C);
	munch2_q = CreateStringQueue(QUEUE_SIZE_C);
	writer_q = CreateStringQueue(QUEUE_SIZE_C);
	
	// declare threads
	pthread_t reader_t;
	pthread_t munch1_t;
	pthread_t munch2_t;
	pthread_t writer_t;

	// create threads and check return values
	if(pthread_create(&reader_t, NULL, reader, NULL)) {
		fprintf(stderr, "Error creating thread.");
		exit(1);
	}
	if(pthread_create(&munch1_t, NULL, munch1, NULL)) {
		fprintf(stderr, "Error creating thread.");
		exit(1);
	}
	if(pthread_create(&munch2_t, NULL, munch2, NULL)) {
		fprintf(stderr, "Error creating thread.");
		exit(1);
	}
	if(pthread_create(&writer_t, NULL, writer, NULL)) {
		fprintf(stderr, "Error creating thread.");
		exit(1);
	}

	// join threads and check return values
	if(pthread_join(reader_t, NULL)) {
		fprintf(stderr, "Error joining thread.");
		exit(1);
	}
	if(pthread_join(munch1_t, NULL)) {
		fprintf(stderr, "Error joining thread.");
		exit(1);
	}
	if(pthread_join(munch2_t, NULL)) {
		fprintf(stderr, "Error joining thread.");
		exit(1);
	}
	if(pthread_join(writer_t, NULL)) {
		fprintf(stderr, "Error joining thread.");
		exit(1);
	}

	// print stats of each queue
	printf("Statistics for queue between Reader and Munch1:\n");
        PrintQueueStats(munch1_q);
        printf("Statistics for queue between Munch1 and Munch2:\n");
        PrintQueueStats(munch2_q);
        printf("Statistics for queue between Munch2 and Writer:\n");
        PrintQueueStats(writer_q);

}

/*
 * This method will read a string and put it in the queue
 */
void *reader() {
	//current line that is being read from stdin
	//char *line = malloc(sizeof(char) * BUFF);
	char *line;
	char c = 1;  // current char that is being added to line
	int index;  // current index of line
	int nextline;  // true if nextline is ready to be processed

	// continue to get next char until EOF is reached
	while (c != EOF) {
		// initialize variables for new string
		line = malloc(sizeof(char) * BUFF);
		index = 0;
		nextline = 0;
		
		// read through entire line
		while (!nextline) {
			c = getc(stdin);

			if ( index >= BUFF - 1) {
				// line is too long for buffer
				fprintf(stderr, "Line length exceeds memory allocated.\n");
				// flush stdin
				while ( ((c = getc(stdin)) != '\n') && !feof(stdin) );

				// move on to next line
				nextline = 1;
				free(line);
				continue;
			} else if ( (c == '\n') || feof(stdin) ) {
				// at end of line null terminate the string
				line[index] = '\0';

				// enqueue the complete line
				EnqueueString(munch1_q, line);
				
				// move on to next line
				nextline = 1;
				continue;
			} else {
				// add char to line
				line[index] = c;
			}
			index++;
		}
	}
	
	// to keep track of end of file, add a NULL line to the end of the queue
	line = NULL;
	EnqueueString(munch1_q, line);

	// exit thread when complete
	pthread_exit(NULL);
}

/*
 * This method will dequeue a string and change each space character to
 * an asteriks before putting it into the next queue
 */
void *munch1() {
	// dequeue first line to be munched
	char *line = DequeueString(munch1_q);

	// continue to dequeue lines until NULL line is reached
	while(line != NULL) {
		// read through string and change space char to *
		for(int i = 0; i < BUFF; i++) {
			// break at end of line
			if (line[i] == '\0')
				break;

			if ( line[i] == ' ' )
				line[i] = '*';

		}

		// enqueue munched line
		EnqueueString(munch2_q, line);
		// dequeue next line
		line = DequeueString(munch1_q);
	}

	// enqueue the NULL line
	EnqueueString(munch2_q, line);
	// exit thread when complete
	pthread_exit(NULL);
}

/*
 * This method will dequeue a string and change all letters to uppercase
 * before putting it into the last queue
 */
void *munch2() {
	// dequeue first line to be munched
	char *line = DequeueString(munch2_q);

	// continue to dequeue until NULL line is reached
	while(line != NULL) {
		// read through string and change all letters to uppercase
		for(int i = 0; i < BUFF; i++) {
			// break at end of line
			if (line[i] == '\0')
				break;
			line[i] = toupper(line[i]);
		}

		// enqueue munched line
		EnqueueString(writer_q, line);
		// dequeue next line
		line = DequeueString(munch2_q);
	}

	// enqueue the NULL line
	EnqueueString(writer_q, line);
	// exit thread when complete
	pthread_exit(NULL);
}

/*
 * This method will dequeue a string and write it to stdout
 */
void *writer() {
	// dequeue first line to write to stdout
	char *line = DequeueString(writer_q);
	int count = 0;  // keep track of strings that are processed

	// continue to dequeue until NULL line is reached
	while(line != NULL) {
		// print line
		fprintf(stdout, "%s\n", line);
		//free line
		free(line);
		// dequeue next line
		line = DequeueString(writer_q);

		count++;
	}
	
	// print out number of strings processed	
	printf("Strings processed: %d\n", count);
	// exit thread when complete
	pthread_exit(NULL);
}
