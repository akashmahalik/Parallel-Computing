#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>


int n;
int flag ;
int sum;
int maxx = 10;
int arr[10000];
int *locarr;
struct xx {
	int a;
	int b;
};




void *foo(void *y)
{
	
	int loc=0;
	int *z =(int *)y; 
	printf("block no  %d \n",z[0] );
	int size = maxx/n;
	for(int i=0;i<size;i++)
	{
		loc+=arr[*z+i];
	}
	locarr[*z] = loc;
	printf("%d\n",loc );
	flag--;
	pthread_exit(NULL);

}

int main()
{
	
	for(int i=0;i<10;i++)
	{
		
		arr[i] = i;
	}

	n = 4;
	locarr = (int *)malloc(sizeof(int)*n);


	flag = n;
	int blocksize = maxx/n;
	int *block_no[n];

	pthread_t *p= (pthread_t *)malloc(sizeof(pthread_t)*n);
	
	struct xx *x = (struct xx *)malloc(sizeof(struct xx));
	x->b = blocksize;
	for(int i=0;i<n;i++){
	
		block_no[i]= (int *)malloc(sizeof(int));
		block_no[i][0]= i;
		
		pthread_create(&p[i],NULL,foo,(void*)block_no[i]);

		
	}
	while(flag!= 0);
	
		int glob=0;
		for(int i=0;i<n;i++)
		{
			glob+=locarr[i];
		}
		printf("%d  glob sum \n",glob );
	
	
	pthread_exit(NULL);

}