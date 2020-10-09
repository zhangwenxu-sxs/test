#ifndef __VTS_QUEUE_H
#define __VTS_QUEUE_H

#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h> 

typedef struct queue{	
	//use to store item
	void** buffer;	

	int head;
	int tail;
	
	//the number of item the queue can store
	size_t size;	
	//the number of item current in queue
	size_t count;	

	sem_t sem;
	pthread_mutex_t mutex;

	/* function member */
	int   (*enqueue)(struct queue* this, void* item);
	void* (*dequeue)(struct queue* this);
	void* (*try_dequeue)(struct queue* this, int wait_time);
	int  (*is_empty)(struct queue* this);
	int  (*is_full)(struct queue* this);
} queue_t;

#endif
