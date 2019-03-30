#include<stdio.h>
#include<mpi.h>
#include<stdlib.h>


int main()
{
	MPI_Init(NULL, NULL);
	int p_num, wrank;
	MPI_Comm_size(MPI_COMM_WORLD, &p_num);
	MPI_Comm_rank(MPI_COMM_WORLD, &wrank);

	MPI_Comm cart_comm;
	int dims[2]= {0, 0};
	int prd[2]= {1, 1};

	MPI_Dims_create(p_num, 2, dims);
	MPI_Cart_create(MPI_COMM_WORLD, 2, dims, prd, 1, &cart_comm);

	
	MPI_Comm_rank(cart_comm, &wrank);
	int coords[2]= {-1, -1};

	MPI_Cart_coords(cart_comm, wrank, 2, coords);


	int sub_n= 2;
	int a[sub_n][sub_n];
	int x[sub_n];
	int y_ij[sub_n][sub_n];
	int y_i[sub_n];
	int local[sub_n][sub_n];
	int output[sub_n];

	for(int i= 0; i< sub_n; i++)
	{
		for(int j= 0; j< sub_n; j++)
		{
			a[i][j]= wrank+1;
		}
	}
	int target_coord[2], target_rank;

	int color = wrank/2; 
	MPI_Comm col_comm;
	MPI_Comm_split(cart_comm, wrank%dims[1], wrank, &col_comm);

	int col_rank, col_size;
	MPI_Comm_rank(col_comm, &col_rank);
	MPI_Comm_size(col_comm, &col_size);
	if(coords[0]== 0)
	{
		for(int i= 0; i< sub_n; i++)
		{
			x[i]= wrank+2;
		}
		for(int i= 0; i< sub_n; i++)
		{
			for(int j= 0; j< sub_n; j++)
			{
				y_ij[i][j]= a[i][j]*x[j];
			}
		}
	}
	MPI_Bcast(
    x,
    sub_n,
    MPI_INT,
    0,
    col_comm);
	if(coords[0]!= 0)
	{
		for(int i= 0; i< sub_n; i++)
		{
			for(int j= 0; j< sub_n; j++)
			{
				y_ij[i][j]= a[i][j]*x[j];
			}
		}
	}

	MPI_Comm row_comm;
	MPI_Comm_split(cart_comm, color, wrank, &row_comm);

	int row_rank, row_size;
	MPI_Comm_rank(row_comm, &row_rank);
	MPI_Comm_size(row_comm, &row_size);
	MPI_Reduce(y_ij, local, sub_n*sub_n, MPI_INT, MPI_SUM, coords[0], row_comm);

	if(coords[0]== coords[1])
	{
		for(int i= 0; i< sub_n; i++)
		{
			y_i[i]= 0;
			for(int j= 0; j< sub_n; j++)
			{
				y_i[i]+= local[i][j];
			}
		
		}
		target_coord[0]= 0;
		target_coord[1]= coords[1];

		MPI_Cart_rank(cart_comm, target_coord, &target_rank);
		MPI_Send(y_i, sub_n, MPI_INT, target_rank, 4, cart_comm);
	}
	if(coords[0]== 0)
	{
		target_coord[0]= coords[1];
		target_coord[1]= coords[1];

		MPI_Cart_rank(cart_comm, target_coord, &target_rank);

		MPI_Recv(output, sub_n, MPI_INT, target_rank, 4, cart_comm, MPI_STATUS_IGNORE);
		for(int i= 0; i< sub_n; i++)
		{
			printf("(%d) %d\n", wrank, output[i]);
		}
		printf("\n");
	}
	
	MPI_Finalize();
	return 0;
	
}
