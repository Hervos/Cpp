#include <iostream>
#include <chrono>
#include <set>
#include <cmath>
#include <functional>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <numeric>

class thread_pool {
public:
	// Constructor
    thread_pool(size_t thread_count) : running(true)
	{
        for (size_t i = 0; i < thread_count; ++i) 
		{
            threads.emplace_back([this] 
			{
                while (true) 
				{
                    std::function<double()> task;
                    {
                    	// Locking queue, waiting ultil there is a task to complete or pool is no longer running
						std::unique_lock<std::mutex> lock(queue_mutex);
                        condition.wait(lock, [this] { return !running || !tasks.empty(); });
                        // If there are no tasks left and pool is not running exits the thread
                        if (!running && tasks.empty()) 
						{
                        
						    return;
                        
						}
						
						// Moving the tasks
                        task = std::move(tasks.front());
                        tasks.pop();
                    
					}
                    // Executing tasks and storing results
					double result = task();
                    std::lock_guard<std::mutex> lock(result_mutex);
                    results.push_back(result);
                
				}
            });
        }
    }
	
	// Adding new tasks to the list
    void add_task(std::function<double()> task) 
	{
    
	    std::lock_guard<std::mutex> lock(queue_mutex);
        tasks.push(std::move(task));
        condition.notify_one();
    
	}

	// Average result of completed tasks
    double average() 
	{
    
	    std::lock_guard<std::mutex> lock(result_mutex);
        return std::accumulate(results.begin(), results.end(), 0.0) / results.size();
    
	}
	
	// Waiting for all threads to finish tasks
    void stop() 
	{
		// This second pair of "{}" looks wrong but this is limiting lifetime of lock_guard and is needed :)
        {
        	
			std::lock_guard<std::mutex> lock(queue_mutex);
            running = false;
        
		}
        condition.notify_all();
        for (auto& thread : threads) 
		{
            if (thread.joinable())
			{
            
			    thread.join();
            
			}
        }
    }

private:
    std::vector<std::thread> threads;
    std::queue<std::function<double()>> tasks;
    std::vector<double> results;
    std::mutex queue_mutex;
    std::mutex result_mutex;
    std::condition_variable condition;
    std::atomic<bool> running;
    
};

int main() 
{
	
    // Create a thread pool with a specified number of threads
    size_t num_threads = 4;
    thread_pool pool(num_threads);

    // A set to store unique task identifiers
    std::set<int> completed_tasks;
    std::mutex set_mutex;

    // Add tasks to the pool and calculate expected average
    int num_tasks = 50;
    double expected_sum = 0;
    for (int i = 0; i < num_tasks; ++i) 
	{
		
        double task_result = i * 3.0; // Example calculation
        expected_sum += task_result;

        pool.add_task([i, task_result, &completed_tasks, &set_mutex]() -> double 
		{
            // Simulate some work
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            {
              
			    std::lock_guard<std::mutex> lock(set_mutex);
                completed_tasks.insert(i);
            
			}
            
			return task_result;
        
		});
    }

    // Calculate the expected average
    double expected_average = expected_sum / num_tasks;

    // Stop the thread pool and wait for all takss to complete
    pool.stop(); // Comment this line and it should print error stated below

    // Check if all tasks were completed
    if (completed_tasks.size() != num_tasks) 
	{
        
		std::cerr << "Error: Some tasks were not completed." << std::endl;
        return 1;
    
	}

    // Get the average from thread pool and compare it with the expected average
    double actual_average = pool.average();
    if (std::abs(actual_average - expected_average) < 1e-6) // Comparing floating points so shouldn't do "== 0", this accounts for rounding error
	{
    
	    std::cout << "Average calculated correctly: " << actual_average << std::endl;
    
	}
	else 
	{
        std::cerr << "Error: Average calculated incorrectly. "
                  << "Expected: " << expected_average << ", Actual: " << actual_average << std::endl;
        
		return 1;
    }

    return 0;
}

