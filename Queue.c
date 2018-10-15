
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
#include"Queue.h"
extern struct Queue * CreateStringQueue(int sizee);
extern void EnqueueString(struct Queue *q, char *string);
extern char * DequeueString(struct Queue *q);
extern void PrintQueueStats(struct Queue * q);
extern int move(int v ,int size);



struct Queue * CreateStringQueue(int sizee){
        struct Queue * Q=(struct Queue *)malloc(sizeof(struct Queue));
        Q->size=sizee;

        Q->buffer[Q->size]=(char *)malloc(sizeof(char *)*Q->size);
        Q->first=Q->last=0;
        pthread_mutex_unlock(&Q->Queue_lock);
        return Q;


                                        }

int move(int v ,int size){
return(v+1)% size;


};

void EnqueueString(struct Queue *q, char *string){
        pthread_mutex_lock(&q->Queue_lock);
        while(move(q->last,q->size)==q->first){
                q->enqueueBlockCount++;
                pthread_cond_wait(&q->cond_add,&q->Queue_lock);
                pthread_mutex_unlock(&q->Queue_lock);
                                }
        q->enqueueCount++;
        q->buffer[q->last]=string;
        q->last=move(q->last,q->size);
        pthread_cond_signal(&q->cond_read);
        pthread_mutex_unlock(&q->Queue_lock);
                                        }


char * DequeueString(struct Queue *q){
        pthread_mutex_lock(&q->Queue_lock);
        while(q->first==q->last){
                q->dequeueBlockCount++; 
                pthread_cond_wait(&q->cond_read,&q->Queue_lock);
                pthread_mutex_unlock(&q->Queue_lock);
                        }
        q->dequeueCount++;
        char * out=q->buffer[q->first]; 
        q->first=move(q->first,q->size);
        pthread_cond_signal(&q->cond_add); 
        pthread_mutex_unlock(&q->Queue_lock);
    
                                }

void PrintQueueStats(struct Queue * q){

	//
	}
