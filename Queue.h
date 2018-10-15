struct Queue {
        int size;
        int first;
        int last;
        char * * buffer;
        pthread_mutex_t Queue_lock;
        pthread_cond_t cond_add;
        pthread_cond_t cond_read;
        int enqueueCount;
        int dequeueCount;
        int enqueueBlockCount;
        int dequeueBlockCount;

                };


struct transfer{
struct Queue *p1;
struct Queue *p2;


};
struct Queue *CreateStringQueue(int size);

void EnqueueString(struct Queue *q, char *string);

char * DequeueString(struct Queue *q);

void PrintQueueStats(struct Queue *q);

int move(int v ,int size);
