#include <thread> 
#include <iostream> 
#include <iomanip>
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

using namespace std; 

class thread_guard
{
	std::thread& t; 
public:
	explicit thread_guard(std::thread& t_): t(t_) {}
	~thread_guard()
	{
		if(t.joinable())
		{
			t.join();
		}
	}
	thread_guard(thread_guard const&) = delete; 
	thread_guard& operator = (thread_guard const&)=delete; 
};

class functionObject_Class_3
{
public: 
    void do_work_thread_4(int sum, int price1, int price2, int price3)
    {
        cout << "------------------------------------" << endl;
        cout << left << setw(15) << setfill(' ') << "Items" 
        << left << setw(15) << setfill(' ') << "Price" << endl; 
        cout << left << setw(15) << setfill(' ') << "Choclates" 
        << left << setw(15) << setfill(' ') << price1 << endl; 
        cout << left << setw(15) << setfill(' ') << "Bread" 
        << left << setw(15) << setfill(' ') << price2 << endl; 
        cout << left << setw(15) << setfill(' ') << "Eggs" 
        << left << setw(15) << setfill(' ') << price3 << endl; 
        cout << "------------------------------------" << endl ;
    }
    void operator() (int sum, int price1, int price2, int price3)
    {
        do_work_thread_4(sum, price1, price2, price3); 
        cout << "---------Exiting Thread 4------" << endl; 

    }
};

class functionObject_Class_2
{
public: 
    void do_work_thread_3(int sum)
    {
        float sale(0.1); 
        float ans = (sum*sale) + sum; 
        
        cout << "Price = " << sum << ", sale = 0.1, " << sum <<"*0.1 = " 
        << (sum*sale) << ", " << sum << "-" << (sum*sale) << "= " << ans << endl; 
    
    }
    void operator() (int sum)
    {
        do_work_thread_3(sum); 
        cout << "-------Exiting Thread 3---------" << endl; 
    }
};

class functionObject_Class_1
{
public: 
    void do_work_thread_2(int sum)
    {
        float temp(0); 
        temp = sum/(1+0.8); 
        float temp1(0); 
        temp1 = temp + sum; 
        cout << sum << "/(1+0.8) = " << temp << ", taxed sum = " <<
        temp << "+" << sum << " = " << temp1 << endl; 
        
        
    }
    void operator() (int sum) 
    {
        do_work_thread_2(sum); 
        cout << "------Entering Thread 3----------" << endl; 
        std::thread l{functionObject_Class_2(), sum}; 
        thread_guard g(l); 
        cout << "--------Exititng thread 2-------- " << endl; 
    }
};

class functionObject_Class
{
    int total_sum; 
    int* quantity; 
    int* price; 
public:
    functionObject_Class() 
    {
        total_sum = 0; quantity = new int[3]; price = new int[3]; 
        price[0] = 15; 
        price[1] = 60; 
        price[2] = 50; 
    }
    
    int do_work_thread_1()
    {
        int temp; 
        cout << "Please enter the quantity of Eggs "; cin >> temp; 
        quantity[0] = temp; 
        total_sum += temp * price[0];
        cout << "Please enter the quantity of Bread "; cin >> temp; 
        quantity[1] = temp; 
        total_sum += temp * price[1];
        cout << "Please enter the quantity of Chocolate "; cin >> temp; 
        quantity[2] = temp; 
        total_sum += temp * price[2]; 
        return total_sum; 
    }
    void operator() ()
    {
        cout << "--------Entering Thread 1--------" << endl; 
        int temp = do_work_thread_1(); 
        cout << "-------Entering thread 2 ---------" << endl ;
        std::thread p{functionObject_Class_1(), temp};
        thread_guard g(p); 


        Sleep(10); 
        cout << "---------Entering Thread 4-------" << endl ;
        std::thread q{functionObject_Class_3(), temp, price[2], price[1], price[0]}; 
        thread_guard m(q); 

    }
  
};

void f()
{
    std::thread t{functionObject_Class()};
    t.join(); 
    cout << "--------Exiting Thread 1---------" << endl;   

}

int main()
{
    f();

};