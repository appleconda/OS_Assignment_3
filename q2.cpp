#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <iostream> 
#include <random> 
#include <ctime>
#include <sstream> 
#include <sys/wait.h>
#include <vector> 
#include <cstring>

using namespace std; 
#define SIZE 4
#define NO_THREADS 4


pthread_mutex_t global_data_mutex = PTHREAD_MUTEX_INITIALIZER;

int* tokenizer(const string& str, int&); 

class mat_mult //function object class to pass into the thread
{
public: 
    int weight_matrix[SIZE][SIZE];
    int x_i_matrix[SIZE][SIZE];  
    int resultant_matrix_mul[SIZE][SIZE]; 
    int resultant_matrix_add[SIZE][SIZE]; 
    int** bias_matrix;  
    int* serialized_arr; 
    int step_for_multiply;
    int step_for_additon; 
    int count; 

    mat_mult(); //constructor initiliazes matrix with random values 
    void init_bias_matrix();
    void* multiply();  
    void* addition(); 
    void print_resultant_matrix_of_mul(); 
    void print_weight_matrix(); 
    void print_xi_matrix();
    void print_resultant_matrix_of_add(); 
    void print_bias_matrix(); 
    void serialize_resultant_matrix_mul(char str[200]); 
};



int main()
{
    mat_mult* matrix_mult_obj = new mat_mult; 
    
    int fd[2]; 
    pipe(fd); 
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

        //serializing the resultant matrix to send over the pipe; 
        string serialized_string_mul; 
        char read_write_char[200]; 
        matrix_mult_obj->serialize_resultant_matrix_mul(read_write_char); 

        //writing the resultant matrix after multiplication into pipe so p2 can read it . 
        close(fd[0]); 
        read_write_char[strlen(read_write_char) - 1] = '\0';
        int n  = strlen(read_write_char) + 1; 
        write(fd[1], &n, sizeof(int));  
        write(fd[1], read_write_char, sizeof(char) *n); 
        close(fd[1]); 
        
        exit(0); 
    }
    else 
    {  
        
        wait(NULL); 
         
        //-----------Representing P2------------
        if ((pids[1] = fork()) == 0)
        {
            mat_mult* matrix_add = new mat_mult; 

            int rtn(0); 
            string resultant_matrix_str; 
            matrix_add->init_bias_matrix(); 
            pthread_t threads[NO_THREADS]; 

            char read_write_char[200]; 
            close(fd[1]);
            int n; 
            read(fd[0], &n, sizeof(int)); 
            read(fd[0], read_write_char, sizeof(char)* n); 

            
            for(auto i = 0;i < 200; i++)
            {
                resultant_matrix_str.push_back(read_write_char[i]); 

            }
            //----------------------------------------------------------------
            /*Serialized_arr & size_of_serialized_arr
            PURPOSE: Serialized_arr is a pointer to an integer array which will hold 
            the resultant matrix calculated by p1 (by multiplication) p1 sent resultant matrix in the form 
            a string to p2, p2 tokenizes it and stores the values in the string into an integer array*/
            //----------------------------------------------------------------
            int size_of_serialized_arr = 0; 
            matrix_add->serialized_arr = NULL; 
            matrix_add->serialized_arr = tokenizer(resultant_matrix_str, size_of_serialized_arr);
            for(auto i = 0; i < 16; i++)
            {
                cout<< "here " << matrix_add->serialized_arr[i] << endl; 
            }
            close(fd[0]);

            for (auto i = 0; i < NO_THREADS; i++)
            {
                if (rtn = pthread_create(&threads[i], NULL, (THREADFUNCPTR) &mat_mult::addition, matrix_add))
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

             
            
            

            std::cout << "Printing the bias matrix " << std::endl; 
            matrix_add->print_bias_matrix(); 
            std::cout << std::endl; 

            std::cout << "Printing the matrix after addition with Bias Matrix " << std::endl; 
            matrix_add->print_resultant_matrix_of_add(); 
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
    cout << "Thread ID = " << pthread_self() << " Entering" << endl;  

    int i = step_for_multiply++; 
    for (int j = 0; j < SIZE; j++)
      for (int k = 0; k < SIZE; k++)
        resultant_matrix_mul[i][j] += weight_matrix[i][k] * x_i_matrix[k][j];
    cout << "Thread ID = " << pthread_self() << " leaving" << endl;  
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
    serialized_arr = nullptr;
    count = 0; 
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

    bias_matrix = new int*[rows]; 
    for (auto i = 0; i < rows; i++)
        bias_matrix[i] = new int[coloumns]; 

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
    pthread_mutex_lock(&global_data_mutex);
    int i = step_for_additon++; 
    int count(0); 
        for (auto k = 0; k < SIZE; k++)
        {
           resultant_matrix_add[i][k] = bias_matrix[i][k] + serialized_arr[count];
           cout << "Added value inside Addition function "<< serialized_arr[count]<< endl;  
           count++; 
        }
    pthread_mutex_unlock(&global_data_mutex);
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

void mat_mult::serialize_resultant_matrix_mul(char str[200])
{
/*
SUMMARY 
    Function of the class: mat_mult
    PURPOSE: This funciton is used in the child process p1 (where multiplication of
    weight matrix and xi matrix happens). Now when the child process works on a instance 
    of the class object mat_mult after exiting all the values that it updated in the instance are destroyed
    so i have to use pipes to serialize the resultant matrix calculated with that process 
    convert into string and send via pipe so p2 can read it and use it. 
*/
    string return_str; 
    for (auto i =0; i < SIZE; i++)
    {
        for(auto j = 0; j < SIZE; j++)
        {
            return_str += to_string(resultant_matrix_mul[i][j]); 
            
            return_str += " "; 
        }
    }
    strcpy(str, return_str.c_str()); 
}

int* tokenizer(const string& str, int& size)
{
    cout << "Entered Tokenizer" << endl; 
    vector<string> strTok;
    istringstream ss(str);
    string deliminiters;
    while (ss >> deliminiters)
    {
        strTok.push_back(deliminiters);
    }

    int* arr = new int[strTok.size()]; 
    int count(0); 
    bool is_ok = false; 
    for (auto i = 0; i < strTok.size(); i++)
    {
        string str = strTok[i]; 
        try 
        {
            arr[i] = stoi(str); 

        }
        catch(exception &err)
        {
            size = count; 
            return arr; 
        }
        cout << arr[i] << endl; 
        count++; 
        
    }
    return arr; 
   
}