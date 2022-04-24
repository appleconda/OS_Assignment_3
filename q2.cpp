#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <iostream> 
#include <random> 
#include <ctime>
#include <sys/wait.h>

using namespace std; 
#define SIZE 4
#define NO_THREADS 4


pthread_mutex_t global_data_mutex = PTHREAD_MUTEX_INITIALIZER;

class mat_mult //function object class to pass into the thread
{
public: 
    float weight_matrix[SIZE][SIZE];
    float x_i_matrix[SIZE][SIZE];  
    float resultant_matrix_mul[SIZE][SIZE]; 
    float resultant_matrix_add[SIZE][SIZE]; 
    float** bias_matrix;  
    int step_for_multiply;
    int step_for_additon; 

    mat_mult(); //constructor initiliazes matrix with random values 
    void init_bias_matrix();
    void* multiply();  
    void* addition(); 
    void print_resultant_matrix_of_mul(); 
    void print_weight_matrix(); 
    void print_xi_matrix();
    void print_resultant_matrix_of_add(); 
    void print_bias_matrix(); 
};

mat_mult* matrix_mult_obj = new mat_mult; 


int main()
{
    srand(time(0)); 
    pid_t pids[3]; //for P1, P2, P3
    typedef void* (*THREADFUNCPTR) (void* ); 

    std::cout << "Printing Weight Matrix " << std::endl; 
    matrix_mult_obj->print_weight_matrix(); 
    std::cout << std::endl; 

    std::cout << "Printing the X_i Matrix" << std::endl; 
    matrix_mult_obj->print_xi_matrix(); 
    std::cout << std::endl; 

    if((pids[0]=fork()) == 0)
    {
       
        pthread_t threads[NO_THREADS]; 
        int rtn(0); 
        for (auto i = 0; i < NO_THREADS; i++)
        {
            if (rtn = pthread_create(&threads[i], NULL, (THREADFUNCPTR) &mat_mult::multiply, matrix_mult_obj))
            {
                fprintf(stderr, "Error: Pthread_create, "); 
                if (rtn == EAGAIN)
                    fprintf(stderr, "Insufficient resources "); 
                else if(rtn == EINVAL)
                    fprintf (stderr, "Invalid Arguments"); 
                exit(1); 
            }
        }

        for (auto& exit: threads)
        {
            pthread_join(exit, NULL); 
        }

        std:: cout << "Printing the resultant matrix after Multiplication " << std::endl; 
        matrix_mult_obj->print_resultant_matrix_of_mul(); 
        std::cout << std::endl; 
        

        exit(0); 
    }
    else 
    {  
        
        wait(NULL); 
        //-----------Representing P2------------
        if ((pids[1] = fork()) == 0)
        {
            int rtn(0); 
            matrix_mult_obj->init_bias_matrix(); 
            pthread_t threads[NO_THREADS]; 
            for (auto i = 0; i < NO_THREADS; i++)
            {
                if (rtn = pthread_create(&threads[i], NULL, (THREADFUNCPTR) &mat_mult::addition, matrix_mult_obj))
                {
                    fprintf(stderr, "Error: Pthread_create, "); 
                    if (rtn == EAGAIN)
                        fprintf(stderr, "Insufficient resources "); 
                    else if(rtn == EINVAL)
                        fprintf (stderr, "Invalid Arguments"); 
                    exit(1); 
                }
            }
            for (auto& exit:threads)
                pthread_join(exit, NULL); 

            // std:: cout << "Printing the resultant matrix after Multiplication " << std::endl; 
            // matrix_mult_obj->print_resultant_matrix_of_mul(); 
            // std::cout << std::endl; 

            std::cout << "Printing the bias matrix " << std::endl; 
            matrix_mult_obj->print_bias_matrix(); 
            std::cout << std::endl; 

            std::cout << "Printing the matrix after addition with Bias Matrix " << std::endl; 
            matrix_mult_obj->print_resultant_matrix_of_add(); 
            std::cout << std::endl; 
            exit(0); 
        }
        else
        {
            wait(NULL); 
        }

    }   
   

    return 0;
}

void* mat_mult::multiply()
{
    pthread_mutex_lock(&global_data_mutex);
    int i = step_for_multiply++; 
    for (int j = 0; j < SIZE; j++)
      for (int k = 0; k < SIZE; k++)
        resultant_matrix_mul[i][j] += weight_matrix[i][k] * x_i_matrix[k][j];
    pthread_mutex_unlock(&global_data_mutex);
}

void mat_mult::print_resultant_matrix_of_mul()
{
/*
SUMMARY
<> 
    Function of class: mat_mult
    purpose: To print the resultant matrix
</> 

*/
    for (auto i  = 0; i < SIZE; i++)
    {
        for (auto j = 0; j < SIZE; j++)
        {
            std::cout << resultant_matrix_mul[i][j] << " "; 
        }
        std::cout << std::endl; 
    }
}

mat_mult::mat_mult()
{
/*
SUMMARY
<>
    Fucntion of class: mat_mult
    Purpose: Acting as constructor for the class
    initializing the "weight_matrix" and "x_i_matrix" with random value between 0 and 10; 
</>
*/
    step_for_multiply = 0; 
    step_for_additon = 0; 
    for (int i = 0; i < SIZE; i++) 
    {
        for (int j = 0; j < SIZE; j++) 
        {
            weight_matrix[i][j] = rand() % 10;
            x_i_matrix[i][j] = rand() % 10;
        }
    }
}

void mat_mult::print_weight_matrix()
{
/*
SUMMARY:
<>
    Function of class "mat_mult"
    Purpose: To print the 2d Array known as "weight_matrix"
</>
*/
    for (auto i = 0; i < SIZE; i++)
    {
        for (auto j = 0; j < SIZE; j++)
        {
            std::cout << weight_matrix[i][j] << " "; 
        }
        std::cout << std::endl; 
    }
}

void mat_mult::print_xi_matrix()
{
/*
SUMMARY:
<>
    Function of class "mat_mult"
    Purpose: To print the 2d Array known as "x_i_matrix"
</>
*/
    for (auto i = 0; i < SIZE; i++)
    {
        for (auto j = 0; j < SIZE; j++)
        {
            std::cout << x_i_matrix[i][j] << " "; 
        }
        std::cout << std::endl; 
    }
}

void mat_mult::init_bias_matrix()
{
/*
SUMMARY
<> 
    Funtion of class: mat_mult
    Purpose: Tp initiliaze the bias matrix by taking input from the user about the number of rows 
    and coloumns and the randomly initliazing the values; 
</>
*/
    // initliazing memory for the matrix
    int rows(0);
    int coloumns(0); 

    //std::cout<< "Please enter the number of rows for Bias matrix "; cin >>rows; 
    //std::cout << std::endl << "Please enter the number of coloumns for Bias Matrix "; cin >> coloumns; 

    rows = 4; 
    coloumns = 4; 

    bias_matrix = new float*[rows]; 
    for (auto i = 0; i < rows; i++)
        bias_matrix[i] = new float[coloumns]; 

    for (auto i = 0; i < rows; i++)
    {
        for (auto j= 0; j < coloumns; j++)
        {
            bias_matrix[i][j] = rand() % 10; 
        }
    }
}

void* mat_mult::addition()
{
/*
SUMMARY 
<>
    Function of class: mat_mult
    PRUPOSE: This function will be passed in created threads each will do addition
    per each row
</>
*/

    int i = step_for_additon++; 
        for (auto k = 0; k < SIZE; k++)
            resultant_matrix_add[i][k] = bias_matrix[i][k] + resultant_matrix_mul[i][k]; 

}

void mat_mult::print_resultant_matrix_of_add()
{
    for(auto i = 0; i < SIZE; i++)
    {
        for (auto j = 0; j < SIZE; j++)
            std::cout << resultant_matrix_add[i][j] << " "; 
        std::cout << std::endl; 
    }
}

void mat_mult::print_bias_matrix()
{
    for (auto  i = 0; i < SIZE; i++)
    {
        for (auto j = 0; j < SIZE; j++)
            std::cout << bias_matrix[i][j] << " ";
        std::cout << std::endl; 
    }
}