#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include<time.h>




int merge(int *a1,int lena,int *a2,int lenb,int *b )
{
	int i,j;
	int outcount = 0;
	for(i=0,j=0;i<lena;i++)
	{
		while(a2[j]<a1[i] && j<lenb )
		{
			b[outcount++] = a2[j++];
		}
		b[outcount++] = a1[i];
	}
	while(j<lenb)
		b[outcount++] = a2[j++];
	return 0;
}
int mergeSort(int *a,int start,int end,int *b)
{
	if(start<end)
	{
		int mid = (start+end)>>1;
		mergeSort(a,start,mid,b);
		mergeSort(a,mid+1,end,b);
		merge(a+start,mid-start+1,a+mid+1,end-(mid+1)+1,b);

		for(int i=start;i<=end;i++)
			a[i] = b[i];
		
	}
	return 0;


}

int merge_me(int n,int *a)
{
	int *b = (int *)malloc(n*sizeof(int));
	mergeSort(a,0,n-1,b);
	return 0;



}

void MPI_Exchange(int local_n,int *local_a,int sendrank,int recvrank,MPI_Comm comm)
{

	int rank;
	int *remote = (int *)malloc(local_n*sizeof(int));

	int *combine = (int *)malloc(local_n*2*sizeof(int));

	int mergeTag = 1;
	int sortedTag = 2;

	MPI_Comm_rank(comm,&rank);

	if(rank==sendrank)
	{
		MPI_Send(local_a,local_n,MPI_INT,recvrank,mergeTag,MPI_COMM_WORLD);
		MPI_Recv(local_a,local_n,MPI_INT,recvrank,sortedTag,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
	}
	else
	{
		MPI_Recv(remote,local_n,MPI_INT,recvrank,mergeTag,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		merge(local_a,local_n,remote,local_n,combine);

		int theirStart = 0,myStart = local_n;
		if(sendrank>rank)
		{
			theirStart = local_n;
			myStart = 0;
		}

		MPI_Send(combine+theirStart,local_n,MPI_INT,sendrank,sortedTag,MPI_COMM_WORLD);
		for (int i=myStart; i<=myStart+local_n; i++)
            local_a[i-myStart] = combine[i];

	}


}

int MPI_OddEvenSort(int n,int *a,int root,MPI_Comm comm)
{

	int rank,size,i;
	int *local_a;

	MPI_Comm_rank(comm,&rank);
	MPI_Comm_size(comm,&size);

	local_a = (int *)malloc( (n/size)*sizeof(int));


	// Scattering done
	MPI_Scatter(a,n/size,MPI_INT,local_a,n/size,MPI_INT,root,comm);
	// Local Sorting first time
	merge_me(n/size,local_a);
	//Odd - Even 

	for(i=1;i<=size;i++)
	{
				//// stats
				printf("rank : %d , round: %d before : ",rank,i );
				for(int j=0;j<n/size;j++)
				{
					printf("%d ",local_a[j] );
				}
				printf("\n");
				//// stats end

				if( !((i+rank)&1))
				{
					if(rank<size-1)
					{
						MPI_Exchange(n/size,local_a,rank,rank+1,comm);
					}
				}else if(rank>0)
				{
						MPI_Exchange(n/size,local_a,rank-1,rank,comm);
				}

				//// stats
				printf("rank : %d , round: %d after : ",rank,i );
				for(int j=0;j<n/size;j++)
				{
					printf("%d ",local_a[j] );
				}
				printf("\n");
				//// stats end


				 MPI_Gather(local_a, n / size, MPI_DOUBLE, a, n / size, MPI_DOUBLE,
           root, comm);

				 if(rank==root)
				 {
				 		//// stats
				printf("rank : %d , round: %d all done  : ",rank,i );
				for(int j=0;j<n;j++)
				{
					printf("%d ",a[j] );
				}
				printf("\n");
				//// stats end

				 }
				 return MPI_SUCCESS;




	}


}

int main()
{
	int n=12;
	// scanf("%d\n",&n);
	int *a = (int *)malloc(n*sizeof(int));
	for(int i=0;i<n;i++)
	{
		a[i] = rand()%1000;
		printf("%d ",a[i] );

	}
	printf("\n");
	MPI_Init(NULL,NULL);

	


	MPI_OddEvenSort(n,a,0,MPI_COMM_WORLD);

	MPI_Finalize();

}

