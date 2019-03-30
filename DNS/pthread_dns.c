#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>


#define a_row 4
#define a_col 4
#define b_row 4
#define b_col 4

// int a_row = 4,a_col = 4,b_row = 4,b_col = 4;

int a[a_row][a_col];
int b[b_row][b_col];
int c[a_row][b_col];
int k,flag;
struct custom
{
	int a,b,c;
};


void *foo(void *y)
{
	// struct xx *w = (struct xx *)(y);
	struct custom *temp = (struct custom *)y;
	// printf("temp a : %d temp b : %d temp c : %d \n",temp->a,temp->b ,temp->c);

	for(int i=(temp->a)*(a_row/k);i<(temp->a)*(a_row/k)+(a_row/k);i++)
	{
		for(int j=(temp->c)*(b_col/k);j<(temp->c)*(b_col/k)+(b_col/k);j++)
		{
			for(int l=(temp->b)*(a_col/k);l<(temp->b)*(a_col/k)+(a_col/k);l++)
			{

				c[i][j] += a[i][l]*b[l][j];
			}	
		}
			

	}
	flag--;
	pthread_exit(NULL);

}
int main()
{	printf("Enter number of processors in each dimension: ");
	scanf("%d",&k);
	clock_t t; 
    t = clock(); 
	flag = k*k*k;
	for(int i =0 ;i<a_row;i++)
	{
		for(int j=0 ;j<a_col;j++)
		{
			a[i][j] = i+j;
			printf("%d ",a[i][j] );
		}
		printf("\n");
	}
	printf("\n");
	for(int i=0;i<a_row;i++)
	{
		for(int j=0;j<b_col;j++)
			c[i][j] = 0;
	}

	for(int i =0 ;i<b_row;i++)
	{
		for(int j=0 ;j<b_col;j++)
		{
			b[i][j] = i+j;

		}
	}


	pthread_t *p= (pthread_t *)malloc(sizeof(pthread_t)*k*k*k);

	struct custom data[k*k*k];

	for(int i=0;i<k;i++)
	{
		for(int j=0;j<k;j++)
		{
			for(int l=0;l<k;l++)
			{
				data[(i*k+j)*k+l].a = i;
				data[(i*k+j)*k+l].b = j;
				data[(i*k+j)*k+l].c = l;

			}
			
		}
	}





	for(int i=0;i<k;i++)
	{
		for(int j=0;j<k;j++)
		{
			for(int l = 0;l<k;l++)
			{

				pthread_create(&p[(i*k+j)*k+l],NULL,foo,(void *)&data[(i*k+j)*k+l]);
			}
		}
	}

	while(flag!=0);

	for(int i=0;i<a_row;i++)
	{
		for(int j=0;j<b_col;j++)
		{
			printf("%d ",c[i][j] );
		}
		printf("\n");
	}



	t = clock() - t; 
    double time_taken = ((double)t)/CLOCKS_PER_SEC;
    printf("The time taken is: %f\n", time_taken);
	pthread_exit(NULL);

}