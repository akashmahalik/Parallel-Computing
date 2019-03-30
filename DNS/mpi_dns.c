#include <mpi.h>
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <time.h>


int main()
{
	MPI_Init(NULL, NULL);
    clock_t t; 
    t = clock(); 
    
	int num_p, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &num_p);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    MPI_Comm cart_comm;
    int dims[3]={0,0,0};
    int prd[3] = {1,1,1};
    MPI_Dims_create(num_p, 3, dims);
    MPI_Cart_create(MPI_COMM_WORLD, 3, dims, prd, 1, &cart_comm);

    int cur[3]={-1,-1,-1};
    MPI_Comm_rank(cart_comm, &rank);
    MPI_Cart_coords(cart_comm, rank,3, cur);

    int n= 4, m= 4, k= 4, x= 2;
    int a_row = n/x;
   	int a_col = m/x;
   	int a_col2= k/x;
    int temp;
    int A[n][m];
    for(int i= 0; i< n; i++)
    {
        for(int j= 0; j< m; j++)
        {
            A[i][j]= i+j;
        }
    }
    int a[a_row][a_col];
	MPI_Comm z_comm;
	int new_dim[3] = {0,0,1};
	int coords2D[2], z_id;
	MPI_Cart_sub(cart_comm, new_dim, &z_comm);
	MPI_Comm_rank(z_comm, &z_id);
	MPI_Cart_coords(z_comm, z_id, 1, coords2D);
	
    if(cur[2]==0)
    {
    	for(int i= 0; i< a_row; i++)
    	{
    		for(int j= 0; j< a_col; j++)
    		{
    			a[i][j]= A[2*(rank/n)+ i][(rank%4)+ j];
               // printf("(%d):: %d", rank, a[i][j]);
    		}
    	}
        
    }
    
    MPI_Bcast(
    a,
    a_row*a_col,
    MPI_INT,
    0,
    z_comm);
    
    
    MPI_Comm y_comm;
    int new_dim_y[]= {0, 1, 0};
    int y_coords[1], y_id;
    MPI_Cart_sub(cart_comm, new_dim_y, &y_comm);
    MPI_Comm_rank(y_comm, &y_id);
    MPI_Cart_coords(y_comm, y_id, 1, y_coords);

	int b[a_col][a_col2];
    
    if(cur[1]==0)
    {
    	for(int i= 0; i< a_col; i++)
    	{
    		for(int j= 0; j< a_col2; j++)
    		{
    			b[i][j]= A[2*(rank/n)+ i][2*(rank%4)+ j];
    		   // printf("(%d):: %d", rank, b[i][j]);
            }
    	}

    }

    MPI_Bcast(
    b,
    a_col*a_col2,
    MPI_INT,
    0,
    y_comm);

    // for(int i= 0; i< a_row; i++)
    // {
    //         for(int j= 0; j< a_col; j++)
    //         {
    //             printf("(%d) : %d ", rank, a[i][j]);
    //         }
    //         printf("\n");
    // }
    
    // for(int i= 0; i< a_row; i++)
    // {
    //         for(int j= 0; j< a_col; j++)
    //         {
    //             printf("(%d) : %d ", rank, b[i][j]);
    //         }
    //         printf("\n");
    // }
    
    
    int local[a_row][a_col2];
    // for(int i= 0; i< a_row; i++)
    // {
    // 	for(int j= 0; j< a_col2; j++)
    // 	{
    // 		local[i][j]= 0;
    // 	}
    // }

    for(int i= 0; i< a_row; i++)
    {
    	for(int j= 0; j< a_col2; j++)
    	{
    		int val= 0;
    		for(int k= 0; k< a_col; k++)
    		{
    			val+= a[i][k]*b[k][j];
    		}
    		local[i][j]= val;
    	}
    }

    MPI_Comm x_comm;
    int n_dimy[3] = {1, 0, 0};
	MPI_Cart_sub(cart_comm, n_dimy, &x_comm);

	int output[a_row][a_col2];
	for(int i= 0; i< a_row; i++)
	{
		for(int j= 0; j< a_col2; j++)
		{
			output[i][j]= 0;
		}
	}


    // for(int i= 0; i< a_row; i++)
    // {
    //     for(int j= 0; j< a_col2; j++)
    //     {
    //         printf("(%d):: %d", rank, local[i][j]);
    //     }
    //     printf("\n");
    // }

	MPI_Reduce(local, output, a_row*a_col2, MPI_INT, MPI_SUM, 0, x_comm);
	

	if(cur[0]== 0)
	{
		for(int i= 0; i< a_row; i++)
		{
			for(int j= 0; j< a_col2; j++)
			{
				printf("(%d) %d ", rank, output[i][j]);
			}
			printf("\n");
		}
	}
	MPI_Finalize();
	t = clock() - t; 
    double time_taken = ((double)t)/CLOCKS_PER_SEC;
    printf("The time taken is: %f\n", time_taken);
    return 0;
}
