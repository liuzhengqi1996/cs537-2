////////////////////////////////////////////////////////////////////////////////
// Main File:        prodcomm.c
// This File:        Queue.h
// Other Files:      prodcomm.c, Queue.c, makefile
// Semester:         CS 537 Fall 2018
// Lecture:          Lec 001
// Authors:          Zhengqi Liu, Tian Zheng
// CS Login:         zhengqi, tian
// NetID:            mliu292, tzheng24
////////////////////////////////////////////////////////////////////////////////

// Structure for the queue passed down between threads
struct Queue {
	// Variables for buffer
	int size;
	int first;
	int last;
	char ** buffer;
	//char *buffer;

	// Variable for mutex and condition
	pthread_mutex_t Queue_lock;
	pthread_cond_t cond_add;
	pthread_cond_t cond_read;
	// A count of the number of strings enqueued on queue
	int enqueueCount;
	// A count of the number of strings dequeued on queue
	int dequeueCount;
	// A count of the number of times that an enqueue was attempted but blocked
	int enqueueBlockCount;
	// A count of the number of times that a dequeue was attempted but blocked
	int dequeueBlockCount;
};

// Structure to transfer two queues
struct transfer{
	struct Queue *p1;
	struct Queue *p2;
};

/*
 * CreateStringQueue - dynamically allocate a new Queue structure, initialize 
 * it with an array of character points of length size, return a pointer to 
 * the new queue structure
 */
struct Queue *CreateStringQueue(int size);

 /*
 * move - move location inside buffer
 */
int move(int v ,int size);

/*
 * EnqueueString - place the pointer to the string at the end of queue q,
 * if the queue is full, then block until there is space available
 */
void EnqueueString(struct Queue *q, char *string);

/*
 * DequeueString - remove a pointer to a string from the beginning of queue q,
 * if the queue is empty, then block until there is a string placed into the 
 * queue, return the pointer that was removed from the queue
 */
char * DequeueString(struct Queue *q);

/*
 * PrintQueueStats - print the statistics for this queue
 */
void PrintQueueStats(struct Queue *q);
