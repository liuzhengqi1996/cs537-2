////////////////////////////////////////////////////////////////////////////////
// Main File:        prodcomm.c
// This File:        prodcomm.c
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

void *Munch1(Queue **ptr);
void *Munch2(Queue **ptr);
void *Reader(Queue **ptr);
void *Writer(Queue **ptr);

/*
 * main - create the three queues and four pthread threads, wait for threads to
 * finish by calling pthread_join
 */
int main() {
	const int SIZE = 10;
	// Queue for Reader and Munch1
	Queue *q1 = CreateStringQueue(SIZE);
	// Queue for Munch1 and Munch2
	Queue *q2 = CreateStringQueue(SIZE);
	// Queue for Munch2 and Writer
	Queue *q3 = CreateStringQueue(SIZE);
	
	// Create 4 threads and pass each thread its queues to operate on
	pthread_t thread1;
	pthread_t thread2;
	pthread_t thread3;
	pthread_t thread4;
	
	struct transfer *w1 = (struct transfer*) malloc(sizeof(struct transfer));
	if (w1 == NULL) {
		fprintf(stderr, "%s\n", "Cannot to allocate memory for w1.");
		exit(1);
	}
	
	struct transfer *w2 = (struct transfer*) malloc(sizeof(struct transfer*));
	if (w2 == NULL) {
		fprintf(stderr, "%s\n", "Cannot to allocate memory for w2.");
		exit(1);
	}
	
	struct writertransfer *wt = (struct writertransfer*) malloc(sizeof(struct writertransfer));
	if (wt == NULL) {
		fprintf(stderr, "%s\n", "Cannot to allocate memory for w2.");
		exit(1);
	}
	
	w1 -> p1 = (Queue*) malloc(sizeof(Queue));
	if (w1 -> p1 == NULL) {
		fprintf(stderr, "%s\n", "Cannot to allocate memory for w1 -> p1.");
		exit(1);
	}
	
	w1 -> p2 = (Queue*) malloc(sizeof(Queue));
	if (w1 -> p2 == NULL) {
		fprintf(stderr, "%s\n", "Cannot to allocate memory for w1 -> p2.");
		exit(1);
	}
	
	w2 -> p1 = (Queue*) malloc(sizeof(Queue));
	if (w2 -> p1 == NULL) {
		fprintf(stderr, "%s\n", "Cannot to allocate memory for w2 -> p1.");
		exit(1);
	}
	
	w2 -> p2 = (Queue*) malloc(sizeof(Queue));
	if (w2 -> p2 == NULL) {
		fprintf(stderr, "%s\n", "Cannot to allocate memory for w2 -> p2.");
		exit(1);
	}
	

	Queue **Q = (Queue **) malloc(3 * sizeof(Queue*));
	Queue *Q1 = (Queue *) malloc(sizeof(Queue));
	Queue *Q2 = (Queue *) malloc(sizeof(Queue));
	Queue *Q3 = (Queue *) malloc(sizeof(Queue));
	Q[0] = &Q1;
	Q[1] = &Q2;
	Q[2] = &Q3;
	
	//int ret1 = pthread_create(&thread1, NULL, Reader, (void*)q1);
	int ret1 = pthread_create(&thread1, NULL, Reader, (Queue**)Q);
	if (ret1 != 0) {
		fprintf(stderr, "%s\n", "Fail to create thread1.");
		exit(1);
	}
	
	w1 -> p1 = q1;
	w1 -> p2 = q2;
	//int ret2 = pthread_create(&thread2, NULL, Munch1, (void*)w1);
	int ret2 = pthread_create(&thread2, NULL, Munch1, (Queue**) Q);
	if (ret2 != 0) {
		fprintf(stderr, "%s\n", "Fail to create thread2.");
		exit(1);
	}
	
	w2 -> p1 = q2;
	w2 -> p2 = q3;
	int ret3 = pthread_create(&thread3, NULL, Munch2, (Queue**) Q);
	if (ret3 != 0) {
		fprintf(stderr, "%s\n", "Fail to create thread3.");
		exit(1);
	}
	
	wt -> p1 = q1;
	wt -> p2 = q2;
	wt -> p3 = q3;
	int ret4 = pthread_create(&thread4, NULL, Writer, (Queue*) Q);
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
	printf("w1:p1: LINE 1 %s \n",w1->p1->buffer [0]);
	printf("w1:p1: LINE 2 %s \n",w1->p1->buffer [1]);
        printf("w1:p2: LINE 1 %s \n",w1->p2->buffer [0]);
	printf("w1:p2: LINE 2 %s \n",w1->p2->buffer [1]);
	return 0;
}

/*
 * Reader - read from standard input one line at a time, take each line of 
 * the input and pass it to thread Munch1 through a queue of character strings
 */
void *Reader(Queue **ptr) {
	//Queue *q1 = (Queue *) malloc(sizeof(Queue));
	//q1 = (Queue *) ptr;
	//Queue **q = (Queue **) malloc(sizeof(Queue*));
	//q = (Queue **) ptr;
	Queue *q1 = ptr[0];
	Queue *q2 = ptr[1];
	Queue *q3 = ptr[2];
	const int QUEUESIZE = 1024;
	char *buffer = (char*) malloc(QUEUESIZE * sizeof(char));
	if (buffer == NULL) {
		fprintf(stderr, "%s\n", "Cannot to allocate memory for buffer.");
		exit(1);
	}
	
	size_t len = 1024;
	int limit = 0;
	// If the input line exceeds the size of buffer, then reject that line
	// by printing out an error message to stderr and throw away any 
	// remaining characters on that line
	while ((limit = getline(&buffer, &len, stdin)) != -1) {
		if (limit >= 0 && limit <= 1024) {
			printf("%s %s\n", "Reader enqueue:", buffer);
			EnqueueString(q1, buffer);
		} else {
			fprintf(stderr, "%s\n", "Size of input exceeds buffer size.");
		}
	}
	/*
	while (getline(&buffer, &len, stdin) != -1) {
		printf("%s %s\n", "Reader:", buffer);
		EnqueueString(q1, buffer);
	}
	*/
	// When Reader detects end-of-file on its input, it then enqueues a NULL 
	// pointer in its output queue, and then does a pthread_exit to terminate
	// that thread.	
	EnqueueString(q1, NULL);
	printf("%s\n", "Reader enqueue: NULL FOR EXIT");
	int retVal = 0;
	pthread_exit(&retVal);
	
	return 0;
}

/*
 * Munch1 - scan the line and replace each space character with an asterisk 
 * ("*") character, then pass the line to thread Munch2 through a queue of 
 * character strings
 */
void *Munch1(Queue **ptr) {
	//struct transfer *we = (struct transfer*) malloc(sizeof(struct transfer));
	//we = (struct transfer*) ptr;
	//Queue *q1 = we -> p1;
	//Queue *q2 = we -> p2;
	//Queue **q = (Queue **) malloc(sizeof(Queue*));
	//q = (Queue **) ptr;
	Queue *q1 = ptr[0];
	Queue *q2 = ptr[1];
	Queue *q3 = ptr[2];

	char *string1 = DequeueString(q1);
	while (string1 != NULL) {
		printf("%s %s\n", "Munch1 dequeue:", string1);
		int n = strlen(string1);
		for (int i = 0; i < n; i++) {
			// Convert space character (not tabs or newlines) to asterisk character
			if (string1[i] == ' ' && string1[i] != '\t' && string1[i] != '\n') {
				string1[i] = '*';
			}
		}
		printf("%s %s\n", "Munch1 enqueue:", string1);
		EnqueueString(q2, string1);
		
		string1 = DequeueString(q1);
	}
	printf("%s %s\n", "Munch1 dequeue:", string1);
	
	// When Munch1 dequeues a NULL from its input queue, it then enqueues the 
	// NULL to its output queue and then does a pthread_exit
	if (string1 == NULL) {
		EnqueueString(q2, NULL);
		printf("%s\n", "Munch1 enqueue: NULL FOR EXIT");
		int retVal = 0;
		pthread_exit(&retVal);
	}
	
	return 0;
}

/*
 * Munch2 - scan the line and convert all lower case letters to upper case, 
 * then pass the line to thread Writer though a queue of character strings
 */
void *Munch2(Queue **ptr) {
	//struct transfer * we = (struct transfer*) malloc(sizeof(struct transfer));
	//we = (struct transfer*) ptr;
	//Queue *q2 = we -> p1;
	//Queue *q3 = we -> p2;	
	//Queue **q = (Queue **) malloc(sizeof(Queue*));
	//q = (Queue **) ptr;
	Queue *q1 = ptr[0];
	Queue *q2 = ptr[1];
	Queue *q3 = ptr[2];

	char *string2 = DequeueString(q2);
	while (string2 != NULL) {
		printf("%s %s\n", "Munch2 dequeue:", string2);
		int n = strlen(string2);
		// If there is a lower case letter, convert to its upper case form
		for (int i = 0; i < n; i++) {
			if (string2[i] >= 97 && string2[i] <= 122) {
				string2[i] = string2[i] - 32;
			}
		}
		printf("%s %s\n", "Munch2 enqueue:", string2);
		EnqueueString(q3, string2);
		
		string2 = DequeueString(q2);
	}
	printf("%s %s\n", "Munch2 dequeue:", string2);
	
	// When Munch2 dequeues a NULL from its input queue, it then enqueues the 
	// NULL to its output queue and then does a pthread_exit
	if (string2 == NULL) {
		EnqueueString(q3, NULL);
		printf("%s\n", "Munch2 dequeue: NULL FOR EXIT");
		int retVal = 0;
		pthread_exit(&retVal);
	}
	return 0;
}

/*
 * Writer - write the line to standard output
 */
void *Writer(Queue **ptr) {
	//struct writertransfer *we = (struct writertransfer *) malloc(sizeof(struct writertransfer));
	//we = (struct writertransfer*) ptr;
	//Queue *q1 = we -> p1;
	//Queue *q2 = we -> p2;
	//Queue *q3 = we -> p3;
	//Queue **q = (Queue **) malloc(sizeof(Queue*));
	//q = ptr;
	Queue *q1 = ptr[0];
	Queue *q2 = ptr[1];
	Queue *q3 = ptr[2];

	char *string = DequeueString(q3);
	while (string != NULL) {
		printf("%s %s\n", "Writer dequeue:", string);
		string = DequeueString(q3);
	}
	printf("%s %s\n", "Writer dequeue:", string);
	
	printf("%s", "q1 statistics: \n");
	PrintQueueStats(q1);
	printf("%s", "q2 statistics: \n");
	PrintQueueStats(q2);
	printf("%s", "q3 statistics: \n");
	PrintQueueStats(q3);
	
	// When Writer dequeues a NULL from its input queue, it then does a pthread_exit
	if (string == NULL) {
		int retVal = 0;
		pthread_exit(&retVal);
	}
	//printf("%s", string);
	
	//free(q1);
	//free(q2);
	//free(q3);
	return 0;
}
