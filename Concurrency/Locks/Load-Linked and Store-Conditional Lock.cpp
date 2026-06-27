// WRITE YOUR CODE HERE
#include <iostream>
#include <atomic>
#include <thread>
#include <vector>

class LLSCLock {
private:
    std::atomic<int> flag;

public:
    LLSCLock() : flag(0) {}

    void lock() {
        
        while (true) {
            int expected = 0;
            // Load-Linked equivalent: load the value
            if (flag.load(std::memory_order_relaxed) == expected) {
                // Store-Conditional equivalent: attempt to store a new value
                if (flag.compare_exchange_strong(expected, 1)) {
                    break; // Break if SC succeeds
                }
            }
        }
        

        /*

        // This is a compact form of Load-Linked and Store-Conditional
        int expected = 0;
        while( flag.load(std::memory_order_relaxed) || !flag.compare_exchange_strong(expected, 1)){
          expected = 0;
        }
        
        */
    }

    void unlock() {
        flag.store(0, std::memory_order_relaxed);
    }
};

int s = 0 ;

void criticalSection(int threadID, LLSCLock &llscLock) {
    std::cout << "Thread " << threadID << " attempting to acquire lock..." << std::endl;
    llscLock.lock();
    std::cout << "Thread " << threadID << " has acquired lock." << std::endl;
    std::cout << "Thread " << threadID << " is in critical section." << std::endl;

    // A working loop
    for(int i = 0 ; i < 100000 ; i++){
        s++;
    }

    std::cout << "Thread " << threadID << " has released lock." << std::endl;
    llscLock.unlock();
}

int main() {
    const int numThreads = 5;
    LLSCLock llscLock;
    std::vector<std::thread> threads;

    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back(criticalSection, i, std::ref(llscLock));
    }

    for (auto &thread : threads) {
        thread.join();
    }

    std::cout << s << std::endl;

    return 0;
}