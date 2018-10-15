#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
#include"Queue.h"
void *Munch1(void * );
void *Munch2(void * );
void *Reader(void * );
void *Writer(void *);

int size=5;

int main(){
          //queue for Reader and Munch1
        struct Queue * q1=CreateString(size);
        //queue for Munch1 and Munch2
        struct Queue * q2=CreateString(size);
        //queue for Munch2 and Writer
        struct Queue * q3=CreateString(size);


pthread_t thread1;
pthread_t thread2;
pthread_t thread3;
pthread_t thread4;
int ret1;
int ret2;
int ret3;
int ret4;

struct transfer * w1;
struct transfer * w2;
w1=(struct transfer *)malloc(sizeof(struct transfer));
w2=(struct transfer *)malloc(sizeof(struct transfer));
w1->p1=(struct Queue *)malloc(sizeof(struct Queue));
w1->p2=(struct Queue *)malloc(sizeof(struct Queue));
w2->p1=(struct Queue *)malloc(sizeof(struct Queue));
w2->p2=(struct Queue *)malloc(sizeof(struct Queue));
w1->p1=q1;
w1->p2=q2;
w2->p1=q2;
w2->p2=q3;

ret1=pthread_create(&thread1,NULL,Reader,(void*)q1);
ret2=pthread_create(&thread2,NULL,Munch1,(void*)w1);
ret3=pthread_create(&thread3,NULL,Munch2,(void*)w2);
ret4=pthread_create(&thread4,NULL,Writer,(void*)q3);


pthread_join(thread1,NULL);
pthread_join(thread2,NULL);
pthread_join(thread3,NULL);
pthread_join(thread4,NULL);

return 0;
}



