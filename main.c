////////////////////////////////////////////////////////////////////////////////
// Main File:        main.c
// This File:        main.c
// Other Files:      queue.c, queue.h, makefile
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

/*
 * main - create the three queues and four pthread threads, wait for threads to
 * finish by calling pthread_join
 */
int main() {
    const int size = 10;
    // Queue for Reader and Munch1
    struct Queue *q1 = CreateStringQueue(size);
    // Queue for Munch1 and Munch2
    struct Queue *q2 = CreateStringQueue(size);
    // Queue for Munch2 and Writer
    struct Queue *q3 = CreateStringQueue(size);

    // Create 4 threads and pass each thread its queues to operate on
    pthread_t thread1;
    pthread_t thread2;
    pthread_t thread3;
    pthread_t thread4;
    int ret1;
    int ret2;
    int ret3;
    int ret4;
    ret1 = pthread_create(&thread1, NULL, Reader, NULL);
    ret2 = pthread_create(&thread2, NULL, Munch1, NULL);
    ret3 = pthread_create(&thread3, NULL, Munch2, NULL);
    ret4 = pthread_create(&thread4, NULL, Writer, NULL);
    
    // Wait for thread termination
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    return 0;
}

/*
 * Reader - read from standard input one line at a time, take each line of 
 * the input and pass it to thread Munch1 through a queue of character strings
 */
void *Reader(void *ptr) {
    const int QUEUESIZE = 1024;
    char *buffer = (char*) malloc(QUEUESIZE * sizeof(char));
    if (buffer == NULL) {
        printf("Cannot to allocate memory for buffer.");
        exit(1);
    }
    
    while (getline(&buffer, &QUEUESIZE, stdin) != -1) {
        EnqueueString(q1, buffer);
    }
    
    /*
    while (fgets(buffer, QUEUESIZE, stdin) != NULL) {
        EnqueueString(q1, buffer);
    }
    */
}

/*
 * Munch1 - scan the line and replace each space character with an asterisk 
 * ("*") character, then pass the line to thread Munch2 through a queue of 
 * character strings
 */
void *Munch1(void *ptr) {
    char *string1 = DequeueString(q1);
    int n = strlen(string1);
    for (int i = 0; i < n; i++) {
        // Convert space character (not tabs or newlines) to asterisk character
        if (string1[i] == ' ' && string1[i] != '\t' && string1[i] != '\n') {
            string1[i] = '*';
        }
    }
    EnqueueString(q2, string1);
}

/*
 * Munch2 - scan the line and convert all lower case letters to upper case, 
 * then pass the line to thread Writer though a queue of character strings
 */
void *Munch2(void *ptr) {
    char *string2 = DequeueString(q2);
    int n = strlen(string2);
    // If there is a lower case letter, convert to its upper case form
    for (int i = 0; i < n; i++) {
        putchar(toupper(string2[i]));
        /*
        if (string2[i] >= 97 && string2[i] <= 122) {
            string2[i] = string2[i] - 32;
        }
        */
    }
    EnqueueString(q3, string2);
}

/*
 * Writer - write the line to standard output
 */
void *Writer(void *ptr) {
    PrintQueueStats(q3);
    char *string = DequeueString(q3);
    printf("%s", string);
    free(q1);
    free(q2);
    free(q3);
}