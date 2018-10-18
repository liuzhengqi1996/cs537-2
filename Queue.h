////////////////////////////////////////////////////////////////////////////////
// Main File:        main.c
// This File:        Queue.h
// Other Files:      main.c, Queue.c, makefile
// Semester:         CS 537 Fall 2018
// Lecture:          Lec 001
// Authors:          Zhengqi Liu, Tian Zheng
// CS Login:         zhengqi, tian
// NetID:            mliu292, tzheng24
////////////////////////////////////////////////////////////////////////////////

#include <pthread.h>

// Structure for the queue passed to threads
typedef struct Queue{
	// Variables for buffer
	int bufferSize;
	int first;
	int last;
	char **buffer;

	// Variable for mutex and condition
	pthread_mutex_t Queue_lock;
	pthread_cond_t cond_full;
	pthread_cond_t cond_empty;
	
	// A count of the number of strings enqueued on queue
	int enqueueCount;
	// A count of the number of strings dequeued on queue
	int dequeueCount;
	// A count of the number of times that an enqueue was attempted but blocked
	int enqueueBlockCount;
	// A count of the number of times that a dequeue was attempted but blocked
	int dequeueBlockCount;
} Queue;

// Structure to transfer two queues
struct transfer{
	Queue *p1;
	Queue *p2;
};

// Structure to transfer three queues
struct writertransfer{
	Queue *p1;
	Queue *p2;
	Queue *p3;
};

/*
 * CreateStringQueue - dynamically allocate a new Queue structure, initialize 
 * it with an array of character points of length size, return a pointer to 
 * the new queue structure
 */
Queue *CreateStringQueue(int size);

 /*
 * move - move location inside queue
 */
int move(int v, int size);

/*
 * EnqueueString - place the pointer to the string at the end of queue q,
 * if the queue is full, then block until there is space available
 */
void EnqueueString(Queue *q, char *string);

/*
 * DequeueString - remove a pointer to a string from the beginning of queue q,
 * if the queue is empty, then block until there is a string placed into the 
 * queue, return the pointer that was removed from the queue
 */
char * DequeueString(Queue *q);

/*
 * PrintQueueStats - print the statistics for this queue
 */
void PrintQueueStats(Queue *q);