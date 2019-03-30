#include "buffer.h"
#include "prod_con.h"
#include <pthread.h>
#include <stdio.h>


pthread_cond_t Buffer_Not_Full;
	pthread_cond_t Buffer_Not_Empty;
	pthread_mutex_t mVar;

void init_prod_con()
{
	
	// initialise lock, condition variables etc.

	pthread_cond_init(&Buffer_Not_Full,NULL);
	pthread_cond_init(&Buffer_Not_Empty,NULL);
	pthread_mutex_init(&mVar,NULL);

}


// Modify this function. Use mutex and condition variable
void produce()
{
        pthread_mutex_lock(&mVar);
		
		while(buffer_full())
		{
					

            pthread_cond_wait(&Buffer_Not_Full,&mVar);

		}
		
		produce_to_buffer(); // <- Do not remove. Add code before and after this line
        pthread_mutex_unlock(&mVar);
        pthread_cond_signal(&Buffer_Not_Empty);        


}

// Modify this function. Use mutex and condition variable
void consume()
{
    pthread_mutex_lock(&mVar);
	while(buffer_empty())
	{
        pthread_cond_wait(&Buffer_Not_Empty,&mVar);

	}
	consume_from_buffer(); // <- Do not remove. Add code before and after this line
    pthread_mutex_unlock(&mVar); 
    pthread_cond_signal(&Buffer_Not_Full);                


}

void destroy_prod_cond()
{
	// destroy mutex, condition variable etc.
	pthread_mutex_destroy(&mVar);
	pthread_cond_destroy(&Buffer_Not_Full);
	pthread_cond_destroy(&Buffer_Not_Empty);
}
