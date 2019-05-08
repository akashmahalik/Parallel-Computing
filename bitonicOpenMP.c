#include<stdio.h>
#include<stdlib.h>
#include<omp.h>
#include<time.h>

int min(int a, int b)
{
	return a<b?a:b;
}


int max(int a, int b)
{
	return a>b?a:b;
}

void bitonic_ascending(int *arr, int start, int offset)
{
	int original_offset= offset;
	while(offset> 1)
	{
		#pragma omp parallel for shared(arr)
			for(int k= start; k< start+original_offset; k+= offset)
			{		
				for(int i= k; i<k+ (offset/2); i++)
				{
					int t= arr[i];
					arr[i]= min(arr[i], arr[i+(offset/2)]);
					arr[i+(offset/2)]= max(t, arr[i+(offset/2)]);
				}
			}
		offset/= 2;
	}
}

void bitonic_descending(int *arr, int start, int offset)
{
	int original_offset= offset;
	while(offset> 1)
	{
		#pragma omp parallel for shared(arr)
			for(int k= start; k< start+original_offset; k+= offset)
			{		
				for(int i= k; i<k+ (offset/2); i++)
				{
					int t= arr[i];
					arr[i]= max(arr[i], arr[i+(offset/2)]);
					arr[i+(offset/2)]= min(t, arr[i+(offset/2)]);
				}
			}
		offset/= 2;
	}
}
void bitonic_sequence_generator(int offset, int *arr, int length)
{
	
		int j= 0;
		#pragma omp parallel for shared(arr)
		for(int i= 0; i< length; i+= offset)
		{
			if(j%2== 0)
			{
				//ascending;
				bitonic_ascending(arr, i, offset);
				j++;
			}
			else
			{
				//descnding;
				bitonic_descending(arr ,i, offset);
				j++;
			}
		}
}



void bitonic(int *arr, int len)
{
	for(int i= 2; i<= len; i*= 2)
		bitonic_sequence_generator(i, arr, len);

}

int main()
{
	int num_threads;
	num_threads= omp_get_num_procs();
	omp_set_num_threads(2*num_threads);

	int len;
	printf("Gve the number of elements to be sorted: ");
	scanf("%d", &len);

	int *arr= (int *)malloc(sizeof(int)*len);
	for(int i= 0; i< len; i++)
	{
		arr[i]= (i+ rand())%1000;
		//printf("%d ", arr[i]);
	}
	printf("\n");
	clock_t t; 
    t = clock(); 
    
	bitonic(arr, len);

	t = clock() - t; 
    double time_taken = ((double)t)/CLOCKS_PER_SEC;
    printf("The time taken for parallel computation is: %f", time_taken);

	for(int i= 0; i< len; i++)
	{
		// arr[i]= (i+ rand())%1000;
		//printf("%d ", arr[i]);
	}
	//printf("\n");
}