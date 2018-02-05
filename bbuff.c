#include "bbuff.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int candy;
void* buffer[BUFFER_SIZE];
sem_t isEmpty;
sem_t isFull;

/**
** Initialize Buffer
**
*/
void bbuff_init(void)
{
	sem_init(&isEmpty, 0, 0);
	sem_init(&isFull, 0, BUFFER_SIZE);

	candy = 0;
}


/**
** Check if Buffer empty
**
*/
_Bool bbuff_is_empty()
{
	if (candy == 0){
		return true;
	}
	else
		return false;

}

/**
** Add Candy
**
*/
void bbuff_blocking_insert(void* item)
{
	sem_wait(&isFull);
	pthread_mutex_lock(&mutex);

	buffer[candy] = item;
	candy++;
    //printf("Added, Candy count is %d\n", candy);
	pthread_mutex_unlock(&mutex);
	sem_post(&isEmpty);  //If the semaphore's value becomes greater
                        //than zero, then wake next in wait()

}


/**
** Remove Candy
**
*/
void* bbuff_blocking_extract(void)
{
	sem_wait(&isEmpty);
	pthread_mutex_lock(&mutex);

	candy--;
	void* thisCandy = buffer[candy];

    //printf("Removed, Candy count is %d\n", candy);
	pthread_mutex_unlock(&mutex);
	sem_post(&isFull);   //If the semaphore's value becomes greater
                        //than zero, then wake next in wait()

	return thisCandy;
}


