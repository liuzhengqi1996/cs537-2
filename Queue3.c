////////////////////////////////////////////////////////////////////////////////
// Main File:        prodcomm.c
// This File:        Queue.c
// Other Files:      prodcomm.c, Queue.h, makefile
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
//#include <assert.h>
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
	Queue *Q = (Queue *) malloc(sizeof(Queue));
	if (Q == NULL) {
		fprintf(stderr, "%s\n", "Cannot to allocate memory for Q.");
		exit(1);
	}
	
	int ret1 = pthread_mutex_init(&Q -> Queue_lock, NULL);
	if (ret1 != 0) {
		fprintf(stderr, "%s\n", "Fail to initialize Queue_lock.");
		exit(1);
	}
	
	int retA = pthread_mutex_lock(&Q -> Queue_lock);
	if (retA != 0) {
		fprintf(stderr, "%s\n", "Fail to lock the monitor lock.");
		exit(1);
	}

	int ret2 = pthread_cond_init(&Q -> cond_add, NULL);
	if (ret2 != 0) {
		fprintf(stderr, "%s\n", "Fail to initialize cond_add.");
		exit(1);
	}
	
	int ret3 = pthread_cond_init(&Q -> cond_read, NULL);
	if (ret3 != 0) {
		fprintf(stderr, "%s\n", "Fail to initialize cond_read.");
		exit(1);
	}
	
	Q -> bufferSize = size;
	Q -> buffer = (char **) malloc(sizeof(char *) * Q -> bufferSize);
	if (Q -> buffer == NULL) {
		fprintf(stderr, "%s\n", "Cannot to allocate memory for Q -> buffer[Q -> bufferSize].");
		exit(1);
	}
	
	Q -> first = Q -> last = 0;
	//Q -> numIn = 0;
	Q -> enqueueCount = Q -> dequeueCount = 0;
	Q -> enqueueBlockCount = Q -> dequeueBlockCount = 0;
	
	int ret5 = pthread_mutex_unlock(&Q -> Queue_lock);
	if (ret5 != 0) {
		fprintf(stderr, "%s\n", "Fail to unlock the monitor lock.");
		exit(1);
	}
	return Q;
}

/*
 * move - move location inside buffer
 */
 
int move(int v, int size) {
	return (v + 1) % size;
};


/*
 * EnqueueString - place the pointer to the string at the end of queue q,
 * if the queue is full, then block until there is space available
 */
void EnqueueString(Queue *q, char *string) {
	int ret1 = pthread_mutex_lock(&q -> Queue_lock);
	if (ret1 != 0) {
		fprintf(stderr, "%s\n", "Fail to lock the monitor lock.");
		exit(1);
	}
	
	int ret2 = -1;
	int ret3 = -1;
	while(move(q -> last, q -> bufferSize) == q -> first) {
	//while(q -> numIn >= q -> bufferSize) {
		q -> enqueueBlockCount++;
		ret2 = pthread_cond_wait(&q -> cond_add, &q -> Queue_lock);
		if (ret2 != 0) {
			fprintf(stderr, "%s\n", "Fail to wait on condition variable.");
			exit(1);
		}
		
		ret3 = pthread_mutex_unlock(&q -> Queue_lock);
		if (ret3 != 0) {
			fprintf(stderr, "%s\n", "Fail to unlock the monitor lock.");
			exit(1);
		}
		
	}
	
	q -> buffer[q -> last] = string;
	q -> last = move(q -> last, q -> bufferSize);
	
	//assert(q -> numIn < q -> bufferSize);
	//q -> buffer[q -> last++] = string;
	//q -> numIn = q -> numIn % q -> bufferSize;
	//q -> numIn++;
	q -> enqueueCount++;
	
	int ret4 = pthread_cond_signal(&q -> cond_read);
	if (ret4 != 0) {
		fprintf(stderr, "%s\n", "Fail to signal condition variable.");
		exit(1);
	}
	
	int ret5 = pthread_mutex_unlock(&q -> Queue_lock);
	if (ret5 != 0) {
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
	int ret1 = pthread_mutex_lock(&q -> Queue_lock);
	if (ret1 != 0) {
		fprintf(stderr, "%s\n", "Fail to lock the monitor lock.");
		exit(1);
	}
	
	int ret2 = -1;
	int ret3 = -1;
	while(q -> first == q -> last) {
	//while(q -> numIn <= 0) {
		q -> dequeueBlockCount++; 
		ret2 = pthread_cond_wait(&q -> cond_read, &q -> Queue_lock);
		if (ret2 != 0) {
			fprintf(stderr, "%s\n", "Fail to wait on condition variable.");
			exit(1);
		}
		
		ret3 = pthread_mutex_unlock(&q -> Queue_lock);
		if (ret3 != 0) {
			fprintf(stderr, "%s\n", "Fail to unlock the monitor lock.");
			exit(1);
		}
		
	}
	
	char *out = q -> buffer[q -> first]; 
	q -> first = move(q -> first, q -> bufferSize);
	
	//assert(q -> numIn > 0);
    //char *out = q -> buffer[q -> first++];
    //q -> first = q -> first % q -> bufferSize;
    //q -> numIn--;
	q -> dequeueCount++;
	
	int ret4 = pthread_cond_signal(&q -> cond_add);
	if (ret4 != 0) {
		fprintf(stderr, "%s\n", "Fail to signal condition variable.");
		exit(1);
	}
	
	int ret5 = pthread_mutex_unlock(&q -> Queue_lock);
	if (ret5 != 0) {
		fprintf(stderr, "%s\n", "Fail to unlock the monitor lock.");
		exit(1);
	}
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
