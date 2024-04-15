#include <iostream>
#include <vector>
#include <thread>
#include <memory>
#include <mutex>
#include <chrono>

// fuel tank class
class fuel_tank 
{
    unsigned int fuel; // fuel amount
    std::mutex mtx;    // mutex for safe access

public:
    // constructor for initial fuel amount
    fuel_tank(unsigned int initial_fuel) : fuel(initial_fuel) {}

    // refuels and returns the refueled amount
    unsigned int refuel(unsigned int amount) 
    {
        std::lock_guard<std::mutex> lock(mtx);
        if (fuel >= amount) 
        {
            fuel -= amount;
            return amount;
        }
        return 0;
    }

    // gets current fuel amount
    unsigned int current_fuel() 
    {
        std::lock_guard<std::mutex> lock(mtx);
        return fuel;
    }
};

// engine class
class engine 
{
    std::vector<std::shared_ptr<fuel_tank>> tanks; // list of fuel tanks
    std::thread worker;    // thread for engine operation
    std::mutex mtx;        // mutex for tank access
    bool running;          // flag to keep engine running
    int engine_id;         // engine identifier mainly for testing

    // function for engine running
    void run(int interval, int fuel_amount) 
    {
        size_t current_tank_index = 0;

        while (running) 
        {
            std::this_thread::sleep_for(std::chrono::seconds(interval));

            bool fuel_taken = false;
            while (!fuel_taken && running) 
            {
                std::lock_guard<std::mutex> lock(mtx);
                
                if (tanks.empty()) 
                {
                    running = false;
                    break;
                }

                if (current_tank_index >= tanks.size()) 
                {
                    current_tank_index = 0;
                }

                unsigned int fuel = tanks[current_tank_index]->refuel(fuel_amount);
                if (fuel > 0) 
                {
                    std::cout << "Engine " << engine_id << " took out " << fuel_amount
                              << " units of fuel, fuel left in tank " << current_tank_index << ": " 
                              << tanks[current_tank_index]->current_fuel() << std::endl;
                    fuel_taken = true;
                } 
                else 
                {
                    current_tank_index = (current_tank_index + 1) % tanks.size();
                }
            }
        }
    }

public:
    // constructor to start engine
    engine(int id, int interval, int fuel_amount) : engine_id(id), running(true) 
    {
        worker = std::thread(&engine::run, this, interval, fuel_amount);
    }

    // destructor to stop engine
    ~engine() 
    {
        running = false;
        if (worker.joinable()) { worker.join(); }
    }

    // connects a tank to the engine
    void connect_tank(const std::shared_ptr<fuel_tank>& tank) 
    {
        std::lock_guard<std::mutex> lock(mtx);
        tanks.push_back(tank);
    }
};

int main() 
{
    std::vector<std::shared_ptr<fuel_tank>> tanks; // holding fuel tanks

    // creating 10 fuel tanks with 10 units each 
    for (int i = 0; i < 10; ++i) 
    {
        tanks.push_back(std::make_shared<fuel_tank>(10));
    }

    // setting up engines with values from task
    engine engine1(1, 2, 5); // Engine 1: 5 units/2 seconds
    engine engine2(2, 1, 1); // Engine 2: 1 unit/1 second
    engine engine3(3, 3, 2); // Engine 3: 2 units/3 seconds

    // connecting tanks to engines
    for (auto& tank : tanks) 
    {
        engine1.connect_tank(tank);
        engine2.connect_tank(tank);
        engine3.connect_tank(tank);
    }

    // let engines run for about 10 seconds
    // actually runs for 14 seconds in this case to let every engine finish work
    std::this_thread::sleep_for(std::chrono::seconds(10));

    return 0;
}
