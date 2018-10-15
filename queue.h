////////////////////////////////////////////////////////////////////////////////
// Main File:        main.c
// This File:        queue.h
// Other Files:      main.c, queue.c, makefile
// Semester:         CS 537 Fall 2018
// Lecture:          Lec 001
// Authors:          Zhengqi Liu, Tian Zheng
// CS Login:         zhengqi, tian
// NetID:            mliu292, tzheng24
////////////////////////////////////////////////////////////////////////////////

// Structure for the queue passed down between threads
struct Queue;

/*
 * CreateStringQueue - dynamically allocate a new Queue structure, initialize 
 * it with an array of character points of length size, return a pointer to 
 * the new queue structure
 */
Queue *CreateStringQueue(int size);

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
char *DequeueString(Queue *q);

/*
 * PrintQueueStats - print the statistics for this queue
 */
void PrintQueueStats(Queue *q);