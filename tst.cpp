#include <fstream> 
#include <iostream> 
#include <string> 
#include <vector> 
#include <sstream>
#include <queue> 
#include <pthread.h>
using namespace std; 

class task
{
public: 
	int id;
    int task_value;
    int arrival_time;
    int unit_count;
	
    int *unitIdList;

	task() 
	{ id = 0; task_value = 0; arrival_time = 0; unit_count = 0; unitIdList = NULL;}

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
	cout <<	line << endl; 

	vector<string> row_data;
	string temp = "";
	while (getline(fout, temp))
	{
		row_data.push_back(temp);

	}
	int number_of_lines = 0; 
	for (auto i : row_data)
	{
		cout << i << endl ;
		number_of_lines++;
	}
	cout << number_of_lines; 
	queue<task> waiting_queue; 
	fout.close(); 
	task* tasks = new task[number_of_lines]; 
	for (auto i= 0; i < number_of_lines; i++)
	{
		tokenizer(row_data[i], tasks[i], waiting_queue);
	}
	
}