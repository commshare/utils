#include <thread>
#include <mutex>
#include <vector>
#include <iostream>
#include <assert.h>
#include <chrono>
//https://thispointer.com/c11-how-to-use-stdthread-as-a-member-variable-in-class/
/*
* A class that has thread object as member variable
*/
class ThreadWrapper
{
// std::thread object
std::thread  threadHandler;
public:
//Delete the copy constructor
ThreadWrapper(const ThreadWrapper&) = delete;
//Delete the Assignment opeartor
ThreadWrapper& operator=(const ThreadWrapper&) = delete;
// Parameterized Constructor
ThreadWrapper(std::function<void()> func);
// Move Constructor
ThreadWrapper(ThreadWrapper && obj);
//Move Assignment Operator
ThreadWrapper & operator=(ThreadWrapper && obj);
//Destructor
~ThreadWrapper();
};
// Parameterized Constructor
ThreadWrapper::ThreadWrapper(std::function<void()> func) : threadHandler(func)
{}
// Move Constructor
ThreadWrapper::ThreadWrapper(ThreadWrapper && obj) : threadHandler(std::move(obj.threadHandler))
{
std::cout << "Move Constructor is called" << std::endl;
}
//Move Assignment Operator
ThreadWrapper & ThreadWrapper::operator=(ThreadWrapper && obj)
{
std::cout << "Move Assignment is called" << std::endl;
if (threadHandler.joinable())
threadHandler.join();
threadHandler = std::move(obj.threadHandler);
return *this;
}
// Destructor
ThreadWrapper::~ThreadWrapper()
{
if (threadHandler.joinable())
threadHandler.join();
}
int main()
{
// Creating a std::function object
std::function<void()> func = []() {
// Sleep for 1 second
std::this_thread::sleep_for (std::chrono::seconds(1));
// Print thread ID
std::cout << "From Thread ID : " << std::this_thread::get_id() << "\n";
};
{
// Create a ThreadWrapper object
// It will internally start the thread
ThreadWrapper wrapper(func);
//When wrapper will go out of scope, its destructor will be called
// Which will internally join the member thread object
}
// Create a vector of ThreadWrapper objects
std::vector<ThreadWrapper> vecOfThreads;
// Add ThreadWrapper objects in thread
ThreadWrapper thwp1(func);
ThreadWrapper thwp2(func);
vecOfThreads.push_back(std::move(thwp1));
vecOfThreads.push_back(std::move(thwp2));
ThreadWrapper thwp3(func);
// Change the content of vector
vecOfThreads[1] = std::move(thwp3);
//When vector will go out of scope, its destructor will be called, which will
// internally call the destructor all ThreadWrapper objects , which in turn
// joins the member thread object.
return 0;
}