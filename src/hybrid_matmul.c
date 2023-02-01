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
    int dataPerRankA;
    int rowsPerRankA = 0;
    int rowsA;
    int colsA;
    int rowsB;
    int colsB;
    int rowsC;
    int colsC;
    int processedDataSize = 0;
    int totalDataSize;
    double *matA_Data = NULL;
    double *matB_Data = NULL;
    double *matArankData = NULL;
    double *finalMat = NULL;
    double *newMat = NULL;
    


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

        
        if(d_1->cols != d_2->rows){
            printf("ERROR: Matrix dimension mismatch.\n");
            exit(EXIT_FAILURE);
        }

        
        rowsA = d_1->rows;
        colsA = d_1->cols;
        rowsB = d_2->rows;
        colsB = d_2->cols;
        rowsC = d_1->rows;
        colsC = d_2->cols;
        

        dataPerRankA = (d_1->rows*d_1->cols) / numRanks;
        rowsPerRankA = (d_1->rows) / numRanks;
        processedDataSize = (d_1->rows * d_2->cols) / numRanks;
        totalDataSize = (d_1->rows * d_2->cols);
        matA_Data = calloc((d_1->rows*d_1->cols), sizeof(double));
        matB_Data = calloc((d_2->rows*d_2->cols), sizeof(double));
        
        
        int k = 0;
        
    		
          
  			#pragma omp parallel for	
  			for(int i = 0; i < d_1->rows; i++)
  			{
  				for(int j = 0; j < d_1->cols; j++)
  				{
  					
  					matA_Data[i*d_1->cols+j] = d_1->data_point[i][j];
  					
  					k++;
  					
  				}
  
  				
  			} 
    		

        
        
        
        k = 0;
  			#pragma omp parallel for
  			for(int i = 0; i < d_2->rows; i++)
  			{
  				for(int j = 0; j < d_2->cols; j++)
  				{
  					
  					matB_Data[i*d_2->cols+j] = d_2->data_point[i][j];
  					
  					k++;
  				}
  			} 
    		
        
        
        
        finalMat = calloc(rowsA * colsB, sizeof(double));
           

    }
    
    
    
    MPI_Bcast(&dataPerRankA, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&rowsPerRankA, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&rowsA, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&colsA, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&rowsB, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&colsB, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&rowsC, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&colsC, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&processedDataSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    if(processRank != 0)
    { 
      matB_Data = malloc(rowsB*colsB*sizeof(double));
    }
    
    MPI_Bcast(matB_Data, rowsB*colsB, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    
    
    matArankData = malloc(dataPerRankA * sizeof(double));
    
    
    start_timer(&start);
    /* TODO: Implement serial Vector Addition */
    MPI_Scatter(matA_Data, dataPerRankA, MPI_DOUBLE, matArankData, dataPerRankA, MPI_DOUBLE, 0, MPI_COMM_WORLD);

   

    //Before timer start declare the objects used and initialize them to zero with calloc
    newMat = calloc(rowsPerRankA*colsB, sizeof(double));

  	
		#pragma omp parallel for
		for(int i = 0; i < rowsPerRankA; i++)
		{
			for(int j = 0; j < colsB; j++)
			{
				for(int w = 0; w < colsA; w++)
				{
					newMat[(i*colsB)+j] += matArankData[(i*colsA)+w] * matB_Data[(w*colsB)+j];
				}
			}
		}
  	

    
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Gather(newMat, processedDataSize, MPI_DOUBLE, finalMat, processedDataSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    
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
        for(unsigned int i = 0; i < rowsC; i++)
        {
          
            for(unsigned int j = 0; j < colsC; j++)
            {
                printf("%f ", finalMat[(i*colsC)+j]);
            }
            printf("\n");
        }
        
        stop_timer(&start);
        fprintf(stderr, " (printing output)\n");
    }

    
    
}
