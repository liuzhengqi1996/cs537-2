////////////////////////////////////////////////////////////////////////////////
// Main File:        main.c
// This File:        main.c
// Other Files:      Queue.c, Queue.h, makefile
// Semester:         CS 537 Fall 2018
// Lecture:          Lec 001
// Authors:          Zhengqi Liu, Tian Zheng
// CS Login:         zhengqi, tian
// NetID:            mliu292, tzheng24
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "Queue.h"

void *Munch1(void *ptr);
void *Munch2(void *ptr);
void *Reader(void *ptr);
void *Writer(void *ptr);

/*
 * main - create three queues and four pthread threads, pass each thread its 
 * queues to operate on, and wait for threads to finish by calling pthread_join
 */
int main() {
	const int SIZE = 10;
	// Queue for Reader and Munch1
	Queue *q1 = CreateStringQueue(SIZE);
	// Queue for Munch1 and Munch2
	Queue *q2 = CreateStringQueue(SIZE);
	// Queue for Munch2 and Writer
	Queue *q3 = CreateStringQueue(SIZE);
	
	// Create structure to transfer queue 1 and 2 to Munch1
	struct transfer *w1 = (struct transfer*) malloc(sizeof(struct transfer));
	if (w1 == NULL) {
		fprintf(stderr, "%s\n", "Cannot to allocate memory for w1.");
		exit(1);
	}
	
	// Pass in queue 1 and 2 to w1
	w1 -> p1 = q1;
	w1 -> p2 = q2;
	
	// Create structure to transfer queue 2 and 3 to Munch2
	struct transfer *w2 = (struct transfer*) malloc(sizeof(struct transfer));
	if (w2 == NULL) {
		fprintf(stderr, "%s\n", "Cannot to allocate memory for w2.");
		exit(1);
	}
	
	// Pass in queue 2 and 3 to w2
	w2 -> p1 = q2;
	w2 -> p2 = q3;
	
	// Create structure to transfer queue 1, 2 and 3 to Writer
	struct writertransfer *wt = (struct writertransfer*) malloc(sizeof(struct writertransfer));
	if (wt == NULL) {
		fprintf(stderr, "%s\n", "Cannot to allocate memory for w2.");
		exit(1);
	}
	
	// Pass in queue 1, 2 and 3 to wt
	wt -> p1 = q1;
	wt -> p2 = q2;
	wt -> p3 = q3;

	// Create 4 threads with queues passed in
	pthread_t thread1;
	pthread_t thread2;
	pthread_t thread3;
	pthread_t thread4;
	
	int ret1 = pthread_create(&thread1, NULL, Reader, (void*) q1);
	if (ret1 != 0) {
		fprintf(stderr, "%s\n", "Fail to create thread1.");
		exit(1);
	}
	
	int ret2 = pthread_create(&thread2, NULL, Munch1, (void*) w1);
	if (ret2 != 0) {
		fprintf(stderr, "%s\n", "Fail to create thread2.");
		exit(1);
	}
	
	int ret3 = pthread_create(&thread3, NULL, Munch2, (void*) w2);
	if (ret3 != 0) {
		fprintf(stderr, "%s\n", "Fail to create thread3.");
		exit(1);
	}
	
	int ret4 = pthread_create(&thread4, NULL, Writer, (void*) wt);
	if (ret4 != 0) {
		fprintf(stderr, "%s\n", "Fail to create thread4.");
		exit(1);
	}
	
	// Wait for thread termination
	int ret6 = pthread_join(thread1, NULL);
	if (ret6 != 0) {
		fprintf(stderr, "%s\n", "Fail to join with a terminated thread1.");
		exit(1);
	}
	
	int ret7 = pthread_join(thread2, NULL);
	if (ret7 != 0) {
		fprintf(stderr, "%s\n", "Fail to join with a terminated thread2.");
		exit(1);
	}
	
	int ret8 = pthread_join(thread3, NULL);
	if (ret8 != 0) {
		fprintf(stderr, "%s\n", "Fail to join with a terminated thread3.");
		exit(1);
	}
	
	int ret9 = pthread_join(thread4, NULL);
	if (ret9 != 0) {
		fprintf(stderr, "%s\n", "Fail to join with a terminated thread4.");
		exit(1);
	}
	return 0;
}

/*
 * Reader - read from standard input one line at a time, take each line of 
 * the input and pass it to thread Munch1 through a queue of character strings
 */
void *Reader(void *ptr) {
	// Create buffer as an array of char* with constant size
	const int QUEUESIZE = 1024;
	size_t len = 1;
	char *buffer = (char*) calloc(len, QUEUESIZE * sizeof(char));
	if (buffer == NULL) {
		fprintf(stderr, "%s\n", "Cannot to allocate memory for buffer.");
		exit(1);
	}
	
	// Read standard input into buffer with limited length
	int limit = 0;
	while ((limit = getline(&buffer, &len, stdin)) != -1) {
		// If the input line is within the limit, enqueue the buffer to queue 1
		if (limit >= 0 && limit <= 1024) {
			EnqueueString(ptr, buffer);
			buffer = (char*) calloc(len, QUEUESIZE * sizeof(char));
		}
		// If the input line exceeds the size of buffer, reject that line by 
		// printing out an error message to stderr and not enqueue the line
		else {
			fprintf(stderr, "%s\n", "Size of input exceeds buffer size, skip a line.");
			buffer = (char*) calloc(len, QUEUESIZE * sizeof(char));
		}
	}

	// When Reader detects end-of-file on its input (get out of while loop), 
	// it then enqueues a NULL pointer in its output queue, queue 1, and 
	// then does a pthread_exit to terminate the thread
	EnqueueString(ptr, NULL);
	int retVal = 0;
	pthread_exit(&retVal);
	return 0;
}

/*
 * Munch1 - scan the line and replace each space character with an asterisk 
 * ("*") character, then pass the line to thread Munch2 through a queue of 
 * character strings
 */
void *Munch1(void *ptr) {
	// Get queue 1 and queue 2 from input transfer structure
	struct transfer * we = (struct transfer*) ptr;
	Queue *q1 = we -> p1;
	Queue *q2 = we -> p2;
	
	// Dequeue string from queue 1
	char *string1 = DequeueString(q1);
	// When Munch1 dequeues a non-null string, it processes the line to convert
	// it and enqueue the string to queue 2
	while (string1 != NULL) {
		int n = strlen(string1);
		for (int i = 0; i < n; i++) {
			// Convert space character (not tabs or newlines) to asterisk character
			if (string1[i] == ' ' && string1[i] != '\t' && string1[i] != '\n') {
				string1[i] = '*';
			}
		}
		EnqueueString(q2, string1);
		string1 = DequeueString(q1);
	}
	
	// When Munch1 dequeues a NULL (get out of while loop) from its input queue,
	// queue 1, it then enqueues the NULL to its output queue, queue 2, 
	// and then does a pthread_exit
	EnqueueString(q2, NULL);
	free(we);
	int retVal = 0;
	pthread_exit(&retVal);
	return 0;
}

/*
 * Munch2 - scan the line and convert all lower case letters to upper case, 
 * then pass the line to thread Writer though a queue of character strings
 */
void *Munch2(void *ptr) {
	// Get queue 2 and queue 3 from input transfer structure
	struct transfer * we = (struct transfer*) ptr;
	Queue *q2 = we -> p1;
	Queue *q3 = we -> p2;	

	// Dequeue string from queue 2
	char *string2 = DequeueString(q2);
	// When Munch2 dequeues a non-null string, it processes the line to convert
	// it and enqueue the string to queue 3
	while (string2 != NULL) {
		int n = strlen(string2);
		// If there is a lower case letter, convert to its upper case form
		for (int i = 0; i < n; i++) {
			if (string2[i] >= 97 && string2[i] <= 122) {
				string2[i] = string2[i] - 32;
			}
		}
		EnqueueString(q3, string2);
		string2 = DequeueString(q2);
	}
	
	// When Munch2 dequeues a NULL (get out of while loop) from its input queue,
	// queue 2, it then enqueues the NULL to its output queue, queue 3, and then
	// does a pthread_exit
	EnqueueString(q3, NULL);
	free(we);
	int retVal = 0;
	pthread_exit(&retVal);
	return 0;
}

/*
 * Writer - write the line to standard output
 */
void *Writer(void *ptr) {
	// Get queue 1, 2 and queue 3 from input transfer structure
	struct writertransfer * we = (struct writertransfer*) ptr;
	Queue *q1 = we -> p1;
	Queue *q2 = we -> p2;
	Queue *q3 = we -> p3;
	
	// Dequeue string from queue 2, write all not-null strings to standard output
	char *string = NULL;
	while (1) {
		string = DequeueString(q3);
		if (string == NULL) {
			break;
			
		}
		printf("%s", string);
		free(string);
	}
	
	// Print the queue statistics
	printf("\n%s", "Queue 1 statistics: \n");
	PrintQueueStats(q1);
	printf("%s", "Queue statistics: \n");
	PrintQueueStats(q2);
	printf("%s", "Queue statistics: \n");
	PrintQueueStats(q3);
	
	// When Writer dequeues a NULL from its input queue (get out of while loop), 
	// it then does a pthread_exit
	free(we);
	free(q1);
	free(q2);
	free(q3);
	int retVal = 0;
	pthread_exit(&retVal);
	return 0;
}
