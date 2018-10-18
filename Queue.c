////////////////////////////////////////////////////////////////////////////////
// Main File:        main.c
// This File:        Queue.c
// Other Files:      main.c, Queue.h, makefile
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

extern Queue *CreateStringQueue(int inputSize);
extern void EnqueueString(Queue *q, char *string);
extern char *DequeueString(Queue *q);
extern void PrintQueueStats(Queue *q);
extern int move(int v, int size);

/*
 * CreateStringQueue - dynamically allocate a new Queue structure, initialize 
 * it with an array of character points of length size, return a pointer to 
 * the new queue structure
 */
Queue *CreateStringQueue(int size) {
	// Create a queue with memory allocation
	Queue *Q = (Queue *) malloc(sizeof(Queue));
	if (Q == NULL) {
		fprintf(stderr, "%s\n", "Cannot to allocate memory for Q.");
		exit(1);
	}
	
	// Initialize mutex and condition variables
	int ret1 = pthread_mutex_init(&Q -> Queue_lock, NULL);
	if (ret1 != 0) {
		fprintf(stderr, "%s\n", "Fail to initialize Queue_lock.");
		exit(1);
	}
	
	int ret2 = pthread_cond_init(&Q -> cond_full, NULL);
	if (ret2 != 0) {
		fprintf(stderr, "%s\n", "Fail to initialize cond_full.");
		exit(1);
	}
	
	int ret3 = pthread_cond_init(&Q -> cond_empty, NULL);
	if (ret3 != 0) {
		fprintf(stderr, "%s\n", "Fail to initialize cond_empty.");
		exit(1);
	}
	
	// Initialize bufferSize to the input size and allocate memory for buffer
	Q -> bufferSize = size;
	Q -> buffer = (char **) malloc(sizeof(char *) * Q -> bufferSize);
	if (Q -> buffer == NULL) {
		fprintf(stderr, "%s\n", "Cannot to allocate memory for Q -> buffer[Q -> bufferSize].");
		exit(1);
	}
	
	// Initialize variables for buffer and queue count
	Q -> first = Q -> last = 0;
	Q -> enqueueCount = Q -> dequeueCount = 0;
	Q -> enqueueBlockCount = Q -> dequeueBlockCount = 0;
	return Q;
}

/*
 * move - move location inside queue
 */
int move(int v, int size) {
	return (v + 1) % size;
};

/*
 * EnqueueString - place the pointer to the string at the end of queue q,
 * if the queue is full, then block until there is space available
 */
void EnqueueString(Queue *q, char *string) {
	// Lock the monitor lock during enqueue
	int ret1 = pthread_mutex_lock(&q -> Queue_lock);
	if (ret1 != 0) {
		fprintf(stderr, "%s\n", "Fail to lock the monitor lock.");
		exit(1);
	}
	
	// When the queue is full, block enqueue, wait for space to add more
	int ret2 = -1;
	while(move(q -> last, q -> bufferSize) == q -> first) {
		q -> enqueueBlockCount++;
		ret2 = pthread_cond_wait(&q -> cond_full, &q -> Queue_lock);
		if (ret2 != 0) {
			fprintf(stderr, "%s\n", "Fail to wait on condition variable.");
			exit(1);
		}
	}
	
	// Enqueue the input string at the end of queue, move location inside queue
	q -> buffer[q -> last] = string;
	q -> last = move(q -> last, q -> bufferSize);
	q -> enqueueCount++;
	
	// Signal to indicate the queue has space now
	int ret3 = pthread_cond_signal(&q -> cond_empty);
	if (ret3 != 0) {
		fprintf(stderr, "%s\n", "Fail to signal condition variable.");
		exit(1);
	}
	
	// Unlock the monitor lock after enqueue
	int ret4 = pthread_mutex_unlock(&q -> Queue_lock);
	if (ret4 != 0) {
		fprintf(stderr, "%s\n", "Fail to unlock the monitor lock.");
		exit(1);
	}
}

/*
 * DequeueString - remove a pointer to a string from the beginning of queue q,
 * if the queue is empty, then block until there is a string placed into the 
 * queue, return the pointer that was removed from the queue
 */
char *DequeueString(Queue *q) {
	// Lock the monitor lock during dequeue
	int ret1 = pthread_mutex_lock(&q -> Queue_lock);
	if (ret1 != 0) {
		fprintf(stderr, "%s\n", "Fail to lock the monitor lock.");
		exit(1);
	}
	
	// When the queue is empty, block dequeue, wait for string from reader
	int ret2 = -1;
	while(q -> first == q -> last) {
		q -> dequeueBlockCount++; 
		ret2 = pthread_cond_wait(&q -> cond_empty, &q -> Queue_lock);
		if (ret2 != 0) {
			fprintf(stderr, "%s\n", "Fail to wait on condition variable.");
			exit(1);
		}
	}
	
	// Dequeue string from the beginning of queue, move location inside queue
	char *out = q -> buffer[q -> first]; 
	q -> first = move(q -> first, q -> bufferSize);
	q -> dequeueCount++;
	
	// Signal to indicate the queue has string now
	int ret3 = pthread_cond_signal(&q -> cond_full);
	if (ret3 != 0) {
		fprintf(stderr, "%s\n", "Fail to signal condition variable.");
		exit(1);
	}
	
	// Unlock the monitor lock after dequeue
	int ret4 = pthread_mutex_unlock(&q -> Queue_lock);
	if (ret4 != 0) {
		fprintf(stderr, "%s\n", "Fail to unlock the monitor lock.");
		exit(1);
	}
	
	// Return the pointer removed from the queue
	return out;
}

/*
 * PrintQueueStats - print the statistics for this queue
 */
void PrintQueueStats(Queue *q) {
	fprintf(stderr, "%s: %d\n", "enqueueCount", q -> enqueueCount);
	fprintf(stderr, "%s: %d\n", "dequeueCount", q -> dequeueCount);
	fprintf(stderr, "%s: %d\n", "enqueueBlockCount", q -> enqueueBlockCount);
	fprintf(stderr, "%s: %d\n", "dequeueBlockCount", q -> dequeueBlockCount);
}