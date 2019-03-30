#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "buffer.h"
#include "prod_con.h"

void do_something() { usleep(rand()%20); }

void *producer(void *x)
{
	int i;
	for(i=0;i<1000;i++)
       	{
	       do_something(); produce();
	       printf("Produced Job #%d\n",i);

       	}
	pthread_exit(NULL);

}

void *consumer(void *x)
{
	int i;
	for(i=0;i<1000;i++)
       	{
	       do_something(); consume();
	       printf("Consumed Job #%d\n",i);
       	}
	pthread_exit(NULL);
}

int main()
{
	init_buffer();
	void init_prod_con();

	pthread_t p,c;

	pthread_create(&p,NULL,producer,NULL);
	pthread_create(&c,NULL,consumer,NULL);


	pthread_join(p,NULL);
	pthread_join(c,NULL);

	 destroy_prod_cond();
	pthread_exit(NULL);
}
