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
#include "Queue.h"

extern struct Queue *CreateStringQueue(int inputSize);
extern void EnqueueString(struct Queue *q, char *string);
extern char *DequeueString(struct Queue *q);
extern void PrintQueueStats(struct Queue *q);
extern int move(int v, int size);

/*
 * CreateStringQueue - dynamically allocate a new Queue structure, initialize 
 * it with an array of character points of length size, return a pointer to 
 * the new queue structure
 */
struct Queue *CreateStringQueue(int inputSize) {
	struct Queue *Q = (struct Queue *) malloc(sizeof(struct Queue));
	if (Q == NULL) {
		fprintf(stderr, "%s\n", "Cannot to allocate memory for Q.");
		exit(1);
	}
	
	pthread_mutex_init(&Q -> Queue_lock, NULL);
	pthread_cond_init(&Q -> cond_add, NULL);
	pthread_cond_init(&Q -> cond_read, NULL);
	
	Q -> size = inputSize;
	Q -> buffer = (char **) malloc(sizeof(char *)*Q -> size);
	if (Q -> buffer == NULL) {
		fprintf(stderr, "%s\n", "Cannot to allocate memory for Q -> buffer[Q -> size].");
		exit(1);
	}
	
	Q -> first = Q -> last = 0;
	pthread_mutex_unlock(&Q -> Queue_lock);
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
void EnqueueString(struct Queue *q, char *string) {
	int ret1 = pthread_mutex_lock(&q -> Queue_lock);
	if (ret1 != 0) {
		fprintf(stderr, "%s\n", "Fail to lock the monitor lock.");
		exit(1);
	}
	
	int ret2 = -1;
	int ret3 = -1;
	while(move(q -> last, q -> size) == q -> first) {
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
	printf("%s", "TEST: ENQUEUE A LINE\n");
	q -> enqueueCount++;
	q -> buffer[q -> last] = string;
	q -> last = move(q -> last, q -> size);
	
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
char *DequeueString(struct Queue *q) {
	int ret1 = pthread_mutex_lock(&q -> Queue_lock);
	if (ret1 != 0) {
		fprintf(stderr, "%s\n", "Fail to lock the monitor lock.");
		exit(1);
	}
	
	int ret2 = -1;
	int ret3 = -1;
	while(q -> first == q -> last) {
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
	printf("%s", "TEST: DEQUEUE A LINE\n");
	q -> dequeueCount++;
	char *out = q -> buffer[q -> first]; 
	q -> first = move(q -> first, q -> size);
	
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
void PrintQueueStats(struct Queue *q) {
	fprintf(stderr, "%s: %d\n", "enqueueCount", q -> enqueueCount);
	fprintf(stderr, "%s: %d\n", "dequeueCount", q -> dequeueCount);
	fprintf(stderr, "%s: %d\n", "enqueueBlockCount", q -> enqueueBlockCount);
	fprintf(stderr, "%s: %d\n", "dequeueBlockCount", q -> dequeueBlockCount);
}
