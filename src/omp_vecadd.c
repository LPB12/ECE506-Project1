#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include "data.h"
#include <omp.h>
#include "timer.h"

int main(int argc, char **argv)
{
    if(argc != 4){
        printf("Usage: <num_thread> <vec_a> <vec_b>.\n");
        exit(EXIT_FAILURE);
    }

    // int numThreads = omp_get_max_threads();
    // convinient for use in partitioning
    int numThreads = atoi(argv[1]);
    omp_set_num_threads(numThreads);
        
    struct timespec start;
    start_timer(&start);

    data_struct *d_1 = get_data_struct(argv[2]);
    data_struct *d_2 = get_data_struct(argv[3]);

    stop_timer(&start);
    fprintf(stderr, " (reading input)\n");

    if(d_1->cols != 1 || d_2->cols !=1){
        printf("ERROR: The dimiension of vector is not correct.\n");
        exit(EXIT_FAILURE);
    }

    //Before timer start declare the objects used and initialize them to zero with calloc
    double **newVec;
    newVec = calloc(d_1->rows, sizeof(double*));
    for(unsigned int i = 0; i < d_1->rows; i++)
    {
      newVec[i] = calloc(d_1->cols, sizeof(double));
    }

    start_timer(&start);
    /* TODO: Implement Vector Addition openMP here */
    #pragma omp parallel
    {
        
        #pragma omp for
        for(unsigned int i = 0; i < d_1->rows; i++)
        {
            *newVec[i] =  *d_1->data_point[i] + *d_2->data_point[i]; 
        }
    }
    
    stop_timer(&start);
    fprintf(stderr, " (calculating answer)\n");
    
    start_timer(&start);
    /* TODO: Print output */
    for(int i = 0; i < d_1->rows; i++)
    {
        printf("%f\n", newVec[i][0]);
    }

    stop_timer(&start);
    fprintf(stderr, " (printing output)\n");
   
}
