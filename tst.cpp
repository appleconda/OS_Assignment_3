#include <fstream> 
#include <iostream> 
#include <string> 
#include <vector> 
#include <sstream>
#include <queue> 
#include <stdlib.h>
#include <pthread.h>
#include <cmath>
#include <iomanip>
#include <chrono>
#include <math.h>
#include <time.h> 
using namespace std; 

typedef void* (*THREADFUNCPTR) (void* ); 

class task
{
public: 
	int id;
    int task_value;
    int arrival_time;
    int unit_count;
	int* task_value_arr; 
    int *unitIdList;
	time_t start, end; 
	double timeTaken; 

	task() 
	{ id = 0; task_value = 0; arrival_time = 0; unit_count = 0; unitIdList = NULL; task_value_arr = NULL;}
	void* work_thread1()
	{
		clock_t begin = clock(); 
		for (auto i = 0; i < unit_count; i++)
		{
			if (unitIdList[i] == 0)
			{
				task_value_arr[i] = task_value; 
				task_value_arr[i] += 5; 
				task_value_arr[i] = task_value_arr[i] % 5000; 
			}
			else if (unitIdList[i] == 1)
			{
				task_value_arr[i] = task_value; 
				task_value_arr[i] = task_value_arr[i] * 7; 
				task_value_arr[i] = task_value_arr[i]%5000; 
			}
			else if (unitIdList[i] == 2)
			{
				task_value_arr[i] = task_value; 
				double temp = task_value; 
				double x = 5;
				task_value_arr[i] =  pow(task_value, x); 
				task_value_arr[i] = task_value_arr[i] % 5000; 
			}
			else if (unitIdList[i] == 3)
			{
				task_value_arr[i] = task_value; 
				task_value_arr[i] -= 100; 
			}
			else if (unitIdList[i] == 4)
			{
				task_value_arr[i] = task_value; 
				task_value_arr[i] = sqrt(task_value_arr[i]);
			}
		}
		clock_t end = clock(); 
		timeTaken = (double)(end - begin); 
		
	}
	void* work_thread()
	{
		pthread_t threads[unit_count]; 
		int rtn; 
		for (auto i = 0; i < unit_count; i++)
		{
			
			if (rtn = pthread_create(&threads[i], NULL, (THREADFUNCPTR) &task::work_thread1, this))
			{   
				fprintf(stderr, "Error: Pthread_create, "); 
				if (rtn == EAGAIN)
					fprintf(stderr, "Insufficient resources "); 
				else if(rtn == EINVAL)
					fprintf (stderr, "Invalid Arguments"); 
				exit(1); 
			}
           
		}	
		for (auto& exit : threads)
			pthread_join(exit, NULL); 

		
	}

	void print()
	{
		for (auto i = 0;i < unit_count; i++)
		{
			//cout << i << endl; 
			cout << "Task value " << i << " " << task_value_arr[i] << endl; 
		}
	}
	task operator = (const task &obj)
	{
		id = obj.id; 
		task_value = obj.task_value; 
		arrival_time = obj.arrival_time; 
		unit_count = obj.unit_count; 
		unitIdList = obj.unitIdList; 

	}
};

int ms = 0; 

void tokenizer(string str, task& tasks, queue<task> waiting_queue)
{
	
	vector<string> token;
    stringstream s(str); 
	string tempString; 
	while(getline(s, tempString, ','))
	{
		token.push_back(tempString); 
	}
	tasks.id = stoi(token[0]); 
	tasks.arrival_time = ms; 
	ms++; 
	tasks.task_value = stoi(token[1]); 
	tasks.unit_count = stoi(token[2]); 
	tasks.unitIdList = new int[stoi(token[2])]; 
	tasks.task_value_arr = new int[tasks.unit_count]; 
	int count(3); 
	
	for (auto i = 0;i < stoi(token[2]); i++)
	{
		tasks.unitIdList[i] = stoi(token[count]); 
		count++; 
	}
	waiting_queue.push(tasks); 
}

int main()
{
	fstream fout; 
	fout.open("input.csv", ios:: in); 
	vector<string> row; 
	
	string line = "";

	getline(fout, line); 

	vector<string> row_data;
	string temp = "";
	while (getline(fout, temp))
	{
		row_data.push_back(temp);

	}
	int number_of_lines = 0; 
	for (auto i : row_data)
	{
		number_of_lines++;
	}
	
	queue<task> waiting_queue; 
	fout.close(); 
	task* tasks = new task[number_of_lines]; 
	for (auto i= 0; i < number_of_lines; i++)
	{
		tokenizer(row_data[i], tasks[i], waiting_queue);
	}
	//type definign here 



	// Work of dispathcer start from here 
	pthread_t pids[number_of_lines]; 
	int rtn; 
	task** my_task = new task*[number_of_lines]; 

	for (auto i = 0; i < number_of_lines; i++)
	{
		my_task[i] = &tasks[i] ;
	}
	
	cout << endl << endl; 
	cout << "Starting " << number_of_lines << " threads to cater for every structure" << endl << endl; 
	for (auto i = 0; i < number_of_lines; i++)
	{
		
		if (rtn = pthread_create(&pids[i], NULL, (THREADFUNCPTR) &task::work_thread, my_task[i]))
 		{   
			fprintf(stderr, "Error: Pthread_create, "); 
			if (rtn == EAGAIN)
				fprintf(stderr, "Insufficient resources "); 
			else if(rtn == EINVAL)
				fprintf (stderr, "Invalid Arguments"); 
			exit(1); 
    	}
           
	}

	for (auto& exit : pids)
		pthread_join(exit, NULL); 

	//system("CLS"); 

	for (auto i = 0;i < number_of_lines; i++)
	{
		cout << "-------------------------------------" << endl; 
		cout << "Task ID = " << my_task[i]->id << endl; 
		cout << "Completion time = " << my_task[i]->timeTaken << endl; 
		for (auto j = 0; j < my_task[i]->unit_count; j++)
			cout << "Unit IDs = " << my_task[i]->unitIdList[j] << ",";
		cout << endl; 
		cout << "Following are the task Values : " << endl ;
		cout << left << setw(10) << "Value#1 "
		 << left << setw(10) << "Value #2 "
		  << left << setw(10) << "Value#3 "
		   << left << setw(10) << "Value#4 "
		    << left << setw(10) << "Value#5 " << endl; 
		for (auto j = 0; j < my_task[i]->unit_count; j++)
		{
			cout << left << setw(10) << my_task[i]->task_value_arr[j]; 
		}
		cout << endl; 

	}

}