#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include "data.h"
#include "timer.h"

int main(int argc, char **argv)
{
    if(argc != 3){
        printf("ERROR: Please specify only 2 files.\n");
        exit(EXIT_FAILURE);
    }
        
    struct timespec start;
    start_timer(&start);

    data_struct *d_1 = get_data_struct(argv[1]);
    data_struct *d_2 = get_data_struct(argv[2]);
    
    stop_timer(&start);
    fprintf(stderr, " (reading input)\n");

    if(d_1->cols != d_2->rows){
        printf("ERROR: Matrix dimension mismatch.\n");
        exit(EXIT_FAILURE);
    }


    //Before timer start declare the objects used and initialize them to zero with calloc
    double **newMat;
    newMat = calloc(d_1->rows, sizeof(double*));
    for(unsigned int i = 0; i < d_1->rows; i++)
    {
      newMat[i] = calloc(d_2->cols, sizeof(double));
    }

    start_timer(&start);
    /* TODO: Implement serial Matrix Multiplication */
    for(int i = 0; i < d_1->rows; i++)
    {
        for(int j = 0; j < d_2->cols; j++)
        {
            for(int w = 0; w < d_1->cols; w++)
            {
                newMat[i][j] += d_1->data_point[i][w] * d_2->data_point[w][j];
            }
        }
    }
    stop_timer(&start);
    fprintf(stderr, " (calculating answer)\n");
    
    start_timer(&start);
    /* TODO: Print output */
    for(int i = 0; i < d_1->rows; i++)
    {
        for(int j = 0; j < d_2->cols; j++)
        {
            printf("%f ",newMat[i][j]);
        }
        printf("\n");
    }
    stop_timer(&start);
    fprintf(stderr, " (printing output)\n");
    
}
