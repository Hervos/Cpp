#include <iostream>
#include <future>
#include <thread>
#include <mutex>

// Function to generate a thread identifier
unsigned int generateThreadId() 
{
	
    static std::mutex idMutex;
    static unsigned int nextId = 1;

    // Lock to ensure exclusive access to nextId
    std::lock_guard<std::mutex> lock(idMutex);
    return nextId++;
    
}

// Function to print text with the thread identifier
void whichthread(const std::string& text) 
{
	
    unsigned int threadId = generateThreadId();

    // Synchronize access to std::cout
    static std::mutex coutMutex;
    std::lock_guard<std::mutex> coutLock(coutMutex);

    // Print the thread identifier and text
    std::cout << "Thread " << threadId << ": " << text << std::endl;

}

int main() 
{
	
    // Test with std::launch::async
    std::future<void> asyncFuture = std::async(std::launch::async, whichthread, "Async");
    asyncFuture.get();  // Wait for completion

    // Test with std::launch::deferred
    std::future<void> deferredFuture = std::async(std::launch::deferred, whichthread, "Deferred");
    deferredFuture.get();  // Wait for completion

    return 0;

}
