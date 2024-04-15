#include <iostream>
#include <future>
#include <mutex>

void printInfo(const std::string& message) 
{
    
    // Synchronize access to std::cout
    static std::mutex coutMutex;
    std::lock_guard<std::mutex> coutLock(coutMutex);

	std::cout << message << " - Thread ID: " << std::this_thread::get_id() << std::endl;

}

void asyncFunction(std::launch policy) 
{
	
    auto printInfoAsync = [](const std::string& message) { printInfo("Async: " + message); };

    auto f1 = std::async(policy, [printInfoAsync]() { printInfoAsync("Task 1"); });

    auto f2 = std::async(policy, [printInfoAsync]() { printInfoAsync("Task 2"); });

    auto f3 = std::async(policy, [printInfoAsync]() { printInfoAsync("Task 3"); });

    f1.get();
    f2.get();
    f3.get();
    
}

int main() 
{
    
	std::launch policy = std::launch::async | std::launch::deferred;
    asyncFunction(policy);

    return 0;

}
