#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <string.h>
#include "data.h"
#include "timer.h"
#include <omp.h>
#include <malloc.h>

int main(int argc, char *argv[]) {

//TODO: Implement Vector addition MPI here //
    if(argc != 4){
        printf("Usage: <num_thread> <vec_a> <vec_b>.\n");
        exit(EXIT_FAILURE);
    }

    // int numThreads = omp_get_max_threads();
    // convinient for use in partitioning
    int numThreads = atoi(argv[1]);
    omp_set_num_threads(numThreads);
    
    int processRank, numRanks;
    int rowsPerRankA;
    int rowsPerRankB;
    int rowsA;
    int colsA;
    int rowsB;
    int colsB;
    double *matA_Data = NULL;
    double *matB_Data = NULL;
    double *matArankData = NULL;
    double *matBrankData = NULL;
    double *finalVec = NULL;


    struct timespec start;
    

    
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numRanks);
    MPI_Comm_rank(MPI_COMM_WORLD, &processRank);
    
    if(processRank == 0)
    {
        
        start_timer(&start);

        
        data_struct *d_1 = get_data_struct(argv[2]);
        data_struct *d_2 = get_data_struct(argv[3]);
        
        stop_timer(&start);
        fprintf(stderr, " (reading input)\n");

        //Vector is of size (nx1), it will always return coloumn =1
        if(d_1->cols != 1 || d_2->cols !=1){
            printf("ERROR: The dimiension of vector is not correct\n");
            exit(EXIT_FAILURE);
        }

        
        rowsA = d_1->rows;
        colsA = d_1->cols;
        rowsB = d_2->rows;
        colsB = d_2->cols;

        
        rowsPerRankA = d_1->rows / numRanks;
        rowsPerRankB = d_2->rows / numRanks;
        matA_Data = calloc((d_1->rows*d_1->cols), sizeof(double));
        matB_Data = calloc((d_2->rows*d_2->cols), sizeof(double));
        
        

        
		#pragma omp parallel
		{
			#pragma omp for
			for(int i = 0; i < d_1->rows; i++)
			{
				for(int j = 0; j < d_1->cols; j++)
				{
					
					matA_Data[i*d_1->cols+j] = d_1->data_point[i][j];
					
					
					
				}
			}
		}
         

        
        
        
		#pragma omp parallel
		{
			#pragma omp for
			for(int i = 0; i < d_2->rows; i++)
			{
				for(int j = 0; j < d_2->cols; j++)
				{
					
					matB_Data[i*d_2->cols+j] = d_2->data_point[i][j];
					
					
				}
			} 
		}
        
        
        
        

    }
    
    MPI_Bcast(&rowsPerRankA, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&rowsPerRankB, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&rowsA, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&colsA, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&rowsB, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&colsB, 1, MPI_INT, 0, MPI_COMM_WORLD);



    
    matArankData = malloc(rowsPerRankA * colsA * sizeof(double));
    matBrankData = malloc(rowsPerRankB * colsB * sizeof(double));
    
    
    start_timer(&start);
    /* TODO: Implement serial Vector Addition */
    MPI_Scatter(matA_Data, rowsPerRankA, MPI_DOUBLE, matArankData, rowsPerRankA, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatter(matB_Data, rowsPerRankB, MPI_DOUBLE, matBrankData, rowsPerRankB, MPI_DOUBLE, 0, MPI_COMM_WORLD);

   

    //Before timer start declare the objects used and initialize them to zero with calloc
    double *newVec;
    newVec = calloc(rowsPerRankA * colsA, sizeof(double));
	
	#pragma omp parallel
	{
		#pragma omp for
		for(unsigned int i = 0; i < (colsA*rowsPerRankA); i++)
		{
		  newVec[i] =  matArankData[i] + matBrankData[i]; 
		}
	}

    MPI_Barrier(MPI_COMM_WORLD);
    if(processRank == 0)
    {
        finalVec = calloc(colsA * rowsPerRankA * numRanks, sizeof(double));
    }
    MPI_Gather(newVec, (rowsPerRankA*colsA), MPI_DOUBLE, finalVec, (rowsPerRankA*colsA), MPI_DOUBLE, 0, MPI_COMM_WORLD);
    
    MPI_Finalize();
    if(processRank == 0)
    {
        stop_timer(&start);
        fprintf(stderr, " (calculating answer)\n");
    }
    

    
    if(processRank == 0)
    {
        start_timer(&start);
        /* TODO: Print output */
        for(unsigned int i = 0; i < rowsA; i++)
        {
            for(unsigned int j = 0; j < colsA; j++)
            {
                printf("%f", finalVec[(i*colsA)+j]);
            }
            printf("\n");
        }
        stop_timer(&start);
        fprintf(stderr, " (printing output)\n");
    }

    
    
}
