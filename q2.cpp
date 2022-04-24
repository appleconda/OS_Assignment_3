#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <iostream> 
#include <random> 
#include <ctime>



using namespace std; 
#define SIZE 4
#define NO_THREADS 4


// float weight_matrix[SIZE][SIZE];
// float x_i_matrix[SIZE][SIZE];  
// float resultant_matrix[SIZE][SIZE]; 


class mat_mult
{
public: 
    float weight_matrix[SIZE][SIZE];
    float x_i_matrix[SIZE][SIZE];  
    float resultant_matrix[SIZE][SIZE]; 
    int step;

    mat_mult()
    {
        step = 0; 
        for (int i = 0; i < SIZE; i++) 
        {
            for (int j = 0; j < SIZE; j++) 
            {
                weight_matrix[i][j] = rand() % 10;
                x_i_matrix[i][j] = rand() % 10;
            }
        }
    }

    void* multiply(); 

    void print_resultant_matrix()
    {
        for (auto i  = 0; i < SIZE; i++)
        {
            for (auto j = 0; j < SIZE; j++)
            {
                std::cout << resultant_matrix[i][j] << " "; 
            }
            std::cout << std::endl; 
        }
    }
};


int main()
{
    typedef void* (*THREADFUNCPTR) (void* ); 
    mat_mult* matrix_mult_obj = new mat_mult; 
    pthread_t threads[NO_THREADS]; 
    int rtn(0); 
    for (auto i = 0; i < NO_THREADS; i++)
    {
        if (rtn = pthread_create(&threads[i], NULL, (THREADFUNCPTR) &mat_mult::multiply, matrix_mult_obj))
        {
            fprintf(stderr, "error: Pthread_create, "); 
            if (rtn == EAGAIN)
                fprintf(stderr, "insufficient resources "); 
            else if(rtn == EINVAL)
                fprintf (stderr, "Invalid Arguments"); 
            exit(1); 
         }
    }

    for (auto i = 0;i < NO_THREADS; i++)
    {
        
        pthread_join(threads[i], NULL); 
    }

    matrix_mult_obj->print_resultant_matrix(); 

    return 0;
}

void* mat_mult::multiply()
{
    int i = step++; 
    for (int j = 0; j < SIZE; j++)
      for (int k = 0; k < SIZE; k++)
        resultant_matrix[i][j] += weight_matrix[i][k] * x_i_matrix[k][j];
    
    
}