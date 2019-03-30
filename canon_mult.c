#include <stdio.h>
#include <mpi.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

int N = 2;

int *create_mat(int r,int c)
{
	int *m = calloc(r*c,sizeof(int));
	return m;
}


void populate_mat(int *a,int r,int c)
{
	for(int i=0;i<r*c;i++)
	{
		a[i] = i+1;
	}
}

int main(int argc, char const *argv[])
{
	
	int a=0,b=0,c=0;
	int *A = create_mat(N,N);
	int *B = create_mat(N,N);
	int *C = create_mat(N,N);


	int size,rank;

	MPI_Init(NULL,NULL);

	MPI_Comm_size(MPI_COMM_WORLD,&size);

	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	populate_mat(A,N,N);
	populate_mat(B,N,N);
	

	MPI_Bcast(&N,1,MPI_INT,0,MPI_COMM_WORLD);

	int periods[]={1,1};
	int dims[] = {2,2};
	
	// int dims[] = {pow(size,1/2),pow(size,1/2)};
	int cords[2];
	int left=0,right=0,up=0,down=0;
	MPI_Comm cart_comm;

	MPI_Cart_create(MPI_COMM_WORLD,2,dims,periods,1,&cart_comm);
	MPI_Scatter(A,1,MPI_INT,&a,1,MPI_INT,0,cart_comm);
	MPI_Scatter(B,1,MPI_INT,&b,1,MPI_INT,0,cart_comm);
	MPI_Comm_rank(cart_comm,&rank);

	MPI_Cart_coords(cart_comm,rank,2,cords);

	MPI_Cart_shift(cart_comm,1,cords[0],&left,&right);
	MPI_Cart_shift(cart_comm,0,cords[1],&up,&down);

	MPI_Sendrecv_replace(&a,1,MPI_INT,left,50,right,50,cart_comm,MPI_STATUS_IGNORE);
	MPI_Sendrecv_replace(&b,1,MPI_INT,up,50,down,50,cart_comm,MPI_STATUS_IGNORE);

	c = c + a*b;

	for(int i = 1;i<N;i++)
	{
			MPI_Cart_shift(cart_comm,1,1,&left,&right);
			MPI_Cart_shift(cart_comm,0,1,&up,&down);

			MPI_Sendrecv_replace(&a,1,MPI_INT,left,50,right,50,cart_comm,MPI_STATUS_IGNORE);
			MPI_Sendrecv_replace(&b,1,MPI_INT,up,50,down,50,cart_comm,MPI_STATUS_IGNORE);
			c = c + a*b;
	}

	C = (int *)calloc(sizeof(int),N*N);
	MPI_Gather(&c,1,MPI_INT,C,1,MPI_INT,0,cart_comm);


	if(rank==0)
	{
		int k=0;
		for(int i=0;i<N;i++)
		{	
			for(int j=0;j<N;j++)
			{
				printf("%d ",C[k] );
				k++;
			}
			printf("\n");
		}
	}
	MPI_Finalize();
	return 0;
}