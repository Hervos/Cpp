#include <iostream>
#include <utility>  // For std::swap and std::move
#include <cassert>  // For assert

namespace cpplab 
{
    
    template <typename T>
    class unique_ptr
    {
    public:
        // Constructor
        explicit unique_ptr(T* p = nullptr) : ptr(p) {}

        // Destructor
        ~unique_ptr() { delete ptr; }

        // Move constructor
        unique_ptr(unique_ptr&& other) noexcept : ptr(other.ptr) 
        {
            other.ptr = nullptr;
        }

        // Move assignment operator
        unique_ptr& operator=(unique_ptr&& other) noexcept 
        {
            if (this != &other) 
            {  
                delete ptr;
                ptr = other.ptr;
                other.ptr = nullptr;
            }
            return *this;
        }

        // Deleted copy constructor and copy assignment operator
        unique_ptr(const unique_ptr&) = delete;
        unique_ptr& operator=(const unique_ptr&) = delete;

        // Access operators
        T& operator*() const { return *ptr; }
        T* operator->() const { return ptr; }

        // Utility functions
        T* get() const { return ptr; }
        T* release() 
        {
            T* temp = ptr;
            ptr = nullptr;
            return temp;
        }
        void reset(T* p = nullptr) 
        {
            delete ptr;
            ptr = p;
        }

    private:
        T* ptr;
    };
	
    template <typename T>
    class non0_ptr 
    {
    public:
        // Constructor
        explicit non0_ptr(T* p) : ptr(p) 
        {
            assert(p != nullptr);  // Runtime check
        }

        // Destructor
        ~non0_ptr() { delete ptr; }

        // Move constructor
        non0_ptr(non0_ptr&& other) noexcept : ptr(other.ptr) 
        {
            other.ptr = nullptr;
        }

        // Move assignment operator
        non0_ptr& operator=(non0_ptr&& other) noexcept 
        {
            assert(other.ptr != nullptr);
            if (this != &other) 
            {
                ptr = other.ptr;
                other.ptr = nullptr;
            }
            return *this;
        }

        // Deleted copy constructor and copy assignment operator
        non0_ptr(const non0_ptr&) = delete;
        non0_ptr& operator=(const non0_ptr&) = delete;

        // Access operators
        T& operator*() const 
        { 
            assert(ptr != nullptr);
            return *ptr; 
        }
        T* operator->() const 
        { 
            assert(ptr != nullptr);
            return ptr; 
        }

        // Utility functions
        T* get() const { return ptr; }
        void reset(T* p) 
        {
            assert(p != nullptr);
            delete ptr;
            ptr = p;
        }

    private:
        T* ptr;
    };

}

int main() {
    // Testing
    
	// Test construction and basic pointer access
    {
        cpplab::unique_ptr<int> up(new int(10));
        std::cout << "Test 1: UniquePtr points to: " << *up << std::endl;
        assert(*up == 10);
    } // 'up' goes out of scope here and deletes the integer

    // Test move constructor
    {
        cpplab::unique_ptr<int> up1(new int(20));
        cpplab::unique_ptr<int> up2(std::move(up1));

        assert(up1.get() == nullptr); // up1 should be null after move
        assert(*up2 == 20); // up2 should hold the value
        std::cout << "Test 2: After move, up2 points to: " << *up2 << std::endl;
    } // 'up2' goes out of scope here and deletes the integer

    // Test move assignment
    {
        cpplab::unique_ptr<int> up1(new int(30));
        cpplab::unique_ptr<int> up2;
        up2 = std::move(up1);

        assert(up1.get() == nullptr); // up1 should be null after move
        assert(*up2 == 30); // up2 should hold the value
        std::cout << "Test 3: After move assignment, up2 points to: " << *up2 << std::endl;
    } // 'up2' goes out of scope here and deletes the integer

    // Test reset and release
    {
        cpplab::unique_ptr<int> up(new int(40));
        int* rawPtr = up.release();
        assert(up.get() == nullptr); // up should be null after release
        std::cout << "Test 4: After release, raw pointer points to: " << *rawPtr << std::endl;

        up.reset(new int(50));
        assert(*up == 50); // up should hold new value after reset
        std::cout << "Test 5: After reset, UniquePtr points to: " << *up << std::endl;

        delete rawPtr; // Manually delete since it is no longer managed by unique_ptr
    } // 'up' goes out of scope here and deletes the new integer
	
	// the only test I will do for non0_ptr is runtime check for nullptr
    // rest of my class is the same as unique_ptr so it works the same way as unique_ptr
    cpplab::non0_ptr<int> nnp_runtime_check(nullptr);

    return 0;
}
