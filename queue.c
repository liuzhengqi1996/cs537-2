////////////////////////////////////////////////////////////////////////////////
// Main File:        main.c
// This File:        queue.c
// Other Files:      main.c, queue.h, makefile
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
#include "queue.h"

// Structure for the queue passed down between threads
struct Queue {
    // Variables for buffer
    int first;
    int last;
    char* buffer[];
    
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
    
    // Manage buffer storage
    int move(int v) {
        return (v + 1) % size;
    }
}

/*
 * CreateStringQueue - dynamically allocate a new Queue structure, initialize 
 * it with an array of character points of length size, return a pointer to 
 * the new queue structure
 */
Queue *CreateStringQueue(int size) {
    first = 0;
    last = 0;
    Queue *q = malloc(sizeof(Queue));
    if (q == NULL) {
        printf("Cannot to allocate memory for queue.");
        exit(1);
    }
    char *buffer[size] = malloc(size * sizeof(*char));
    if (buffer == NULL) {
        printf("Cannot to allocate memory for buffer.");
        exit(1);
    }
    pthread_mutex_unlock(&Queue_lock);
    return q;
}

/*
 * EnqueueString - place the pointer to the string at the end of queue q,
 * if the queue is full, then block until there is space available
 */
void EnqueueString(Queue *q, char *string) {
    pthread_mutex_lock(&q.Queue_lock);
    while (move(q.last) == q.first) {
        enqueueBlockCount++;
        pthread_cond_wait(&q.cond_add);
        pthread_mutex_unlock(&q.Queue_lock);
    }
    enqueueCount++;
    q.buffer[q.last] = string;
    q.last = move(q.last);
    pthread_cond_signal(&q.dequeue);
    pthread_mutex_unlock(&q.Queue_lock);
}

/*
 * DequeueString - remove a pointer to a string from the beginning of queue q,
 * if the queue is empty, then block until there is a string placed into the 
 * queue, return the pointer that was removed from the queue
 */
char *DequeueString(Queue *q) {
    pthread_mutex_lock(&q.Queue_lock);
    while (q.first == q.last) {
        dequeueBlockCount++;
        pthread_cond_wait(&q.cond_read);
        pthread_mutex_unlock(q.Queue_lock);
    }
    dequeueCount++;
    char *out = q.buffer[q.first];
    q.first = move(q.first);
    pthread_cond_signal(&q.cond_add);
    pthread_mutex_unlock(&q.Queue_lock);
}

/*
 * PrintQueueStats - print the statistics for this queue
 */
void PrintQueueStats(Queue *q) {
    fprintf(stderr, "%d", enqueueCount);
    fprintf(stderr, "%d", dequeueCount);
    fprintf(stderr, "%d", enqueueBlockCount);
    fprintf(stderr, "%d", dequeueBlockCount);
}

