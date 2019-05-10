#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "limits.h"
#include <time.h>
#include "myclock.h"
#define MAX_THREADS 8


typedef struct barrier_node
{

	pthread_mutex_t count_lock;
	pthread_cond_t ok_to_proceed;
	int count;

}mylib_barrier_t;

typedef struct barrier_node1
{

	pthread_mutex_t count_lock;
	pthread_cond_t ok_to_proceed_up;
	pthread_cond_t ok_to_proceed_down;
	int count;

}mylib_logbarrier_t;


pthread_t p_threads[MAX_THREADS];
pthread_attr_t attr;

mylib_barrier_t *barrier_var;
mylib_logbarrier_t *logbarrier_var;

void initialise()
{
	barrier_var= (mylib_barrier_t*)malloc(sizeof(mylib_barrier_t));
	barrier_var->count= 0;
	pthread_mutex_init(&(barrier_var->count_lock), NULL);
	pthread_cond_init(&(barrier_var->ok_to_proceed), NULL);
}


void initialise_log()
{
	logbarrier_var= (mylib_logbarrier_t*)malloc(sizeof(mylib_logbarrier_t)*(4*MAX_THREADS));
	for(int i= 0; i< (4*MAX_THREADS); i++)
	{
		(logbarrier_var[i]).count= 0;
		pthread_mutex_init(&((logbarrier_var[i]).count_lock), NULL);
		pthread_cond_init(&((logbarrier_var[i]).ok_to_proceed_up), NULL);
		pthread_cond_init(&((logbarrier_var[i]).ok_to_proceed_down), NULL);
	}
}

void * foo(void* x)
{
	int *data= (int *)x;
	int id= data[0];
	usleep(1000);
	pthread_mutex_lock(&(barrier_var->count_lock));
	barrier_var->count++;
	printf("Thread %d is waiting \n", id);
	if(barrier_var->count== MAX_THREADS)
	{
		barrier_var->count= 0;
		pthread_cond_broadcast(&(barrier_var->ok_to_proceed));
	}
	else
	{
		while (pthread_cond_wait(&(barrier_var->ok_to_proceed),&(barrier_var->count_lock)) != 0);
	}
	pthread_mutex_unlock(&(barrier_var->count_lock));
	printf("Thread %d has crossed \n", id);
	pthread_exit(NULL);
}

void *foo_log(void *x)
{
	int *data= (int *)x;
	int tid= data[0];
	usleep(1000);
	int base= 0, index, i= 2;
	do
	{
		index= base+ tid/i;
		if(tid%i== 0)
		{
			pthread_mutex_lock(&((logbarrier_var[index]).count_lock));
			((logbarrier_var[index]).count)++;
			printf("Thread %d is waiting at the index: %d \n", tid, index);
			while(((logbarrier_var[index]).count)< 2)
			{
				pthread_cond_wait(&(logbarrier_var[index]).ok_to_proceed_up, &(logbarrier_var[index]).count_lock); 
			}
			pthread_mutex_unlock(&((logbarrier_var[index]).count_lock));
		}
		else
		{
			pthread_mutex_lock(&((logbarrier_var[index]).count_lock));
			((logbarrier_var[index]).count)++;
			if(((logbarrier_var[index]).count)== 2)
			{
				pthread_cond_signal(&(logbarrier_var[index]).ok_to_proceed_up);
			}
			printf("Thread %d is waiting at index: %d\n", tid, index);

			while(pthread_cond_wait(&(logbarrier_var[index]).ok_to_proceed_down, &(logbarrier_var[index]).count_lock)!= 0);
			pthread_mutex_unlock(&((logbarrier_var[index]).count_lock));
			printf("Thread %d has crossed at index: %d\n", tid, index);
			break;
		}
		base+= MAX_THREADS/i;
		i*= 2;
	}while(i<= MAX_THREADS);


	i= i/2;
	for(;i> 1; i/=2)
	{
		base-= MAX_THREADS/i;
		index= base+ tid/i;
		pthread_mutex_lock(&((logbarrier_var[index]).count_lock));
		((logbarrier_var[index]).count)= 0;
		printf("Thread %d has crossed at index: %d\n", tid, index);
		pthread_cond_signal(&(logbarrier_var[index]).ok_to_proceed_down);
		pthread_mutex_unlock(&((logbarrier_var[index]).count_lock));
	}

}

void linear_barrier()
{
	pthread_t threads[MAX_THREADS];
	int *data[MAX_THREADS];
	for(int i= 0; i< MAX_THREADS; i++)
	{
		data[i]= (int *)malloc(sizeof(int));
		data[i][0]= i;
		pthread_create(&threads[i], NULL, foo, (void *)data[i]);
	}
	for(int i= 0; i< MAX_THREADS; i++)
	{
		pthread_join(threads[i], NULL);
	}
}

void log_barrier()
{
	pthread_t threads[MAX_THREADS];
	int *data[MAX_THREADS];

	for(int i= 0; i< MAX_THREADS; i++)
	{
		data[i]= (int *)malloc(sizeof(int));
		data[i][0]= i;
		pthread_create(&threads[i], NULL, foo_log, (void *)data[i]);
	}
	for(int i= 0; i< MAX_THREADS; i++)
	{
		pthread_join(threads[i], NULL);
	}
}

int main()
{

	initialise();

	int trials= 1;
	clock_t ti;
	double total= 0.0;
	for(int i=0;i<trials;i++)
	{	
    	ti = clock(); 
    	linear_barrier();
		ti = clock() - ti;
		total+= ((double)ti)/CLOCKS_PER_SEC;
    	

	}
	printf("\n");
	printf("Avg time for linearbarrier %lf sec\n\n",total/10);

	total= 0.0;
	initialise_log();

	
	for(int i=0;i<trials;i++)
	{	
    	ti = clock(); 
    	log_barrier();
		ti = clock() - ti;
		total+= ((double)ti)/CLOCKS_PER_SEC;
  

	}
	printf("\n");
	printf("Avg time for logbarrier %lf sec\n\n",total/10);


}