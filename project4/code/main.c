#include <stdlib.h> /* require for rand()*/
#include "buffer.h"
#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#define THREAD_NUM 20

/* define the buffer 
 * define the produce and consume index of buffer
 * define the semaphore
 */
buffer_item buffer[BUFFER_SIZE];
int index = 0;
sem_t empty;
sem_t full;
sem_t mutex;

int insert_item(buffer_item item){
	if( index < BUFFER_SIZE ){
		index = (index + 1) % BUFFER_SIZE;
		buffer[index] = item;
		return 0;
	}
	else
		return -1;
}

int remove_item(buffer_item *item){
	if( index < BUFFER_SIZE ){
		*item = buffer[index];
		index--;
		if(index < 0)
			index += BUFFER_SIZE;
		return 0;
	}
	else
		return -1;
}

/* define the producer and consumer*/
void *producer(void *param){
	buffer_item ran;

	while(1){
		/* sleep for a random period of time */
		sleep(rand()%6);
		/* generate a random number */
		ran = rand()%30;

		sem_wait(&empty);
		sem_wait(&mutex);

		/* add nextp to buffer*/
		printf("producer produced %d\n", ran);
		if( insert_item(ran))
			printf("report error condition");

		sem_post(&mutex);
		sem_post(&full);
	}
}

void *consumer(void *param){
	buffer_item ran;

	while(1){
		/* sleep for a random period of time */
		sleep(rand()%6);

		sem_wait(&full);
		sem_wait(&mutex);

		int re = remove_item(&ran);

		sem_post(&mutex);
		sem_post(&empty);

		if(re)
			printf("report error condition");
		else
			printf("consumer consumed %d\n", ran);

	}
}

int main(int argc, char *argv[])
{
	/* check the number of parameters*/
	if(argc != 4){
		printf("Wrong number of parameters\n");
		return 0;
	}

	int sleeptime = atoi(argv[1]);
	int pNum = atoi(argv[2]);
	int cNum = atoi(argv[3]);

	/* The number of producers or consumers no more than THREAD_NUM*/
	if (pNum > THREAD_NUM || cNum > THREAD_NUM){
		printf("The number of producers or consumers no more than %d\n", THREAD_NUM);
		return 0;
	}

	/* Store the threads*/
	pthread_t producers_thread[THREAD_NUM];
	pthread_t consumers_thread[THREAD_NUM];

	/* initialize semaphore*/
	sem_init(&mutex, 0 ,1);
	sem_init(&empty, 0, BUFFER_SIZE);
	sem_init(&full, 0, 0);
	srand(time(NULL));

	/* create threads */
	for (int i = 0; i < pNum; ++i)
	{
		if(pthread_create(&producers_thread[i],NULL,producer,NULL))
			printf("Producer Thread Creating Error!\n");
	}

	for (int i = 0; i < cNum; ++i)
	{
		if(pthread_create(&consumers_thread[i],NULL,consumer,NULL))
			printf("consumer Thread Creating Error!\n");
	}

	/* Sleep */
	sleep(sleeptime);
	
	return 0;
}
