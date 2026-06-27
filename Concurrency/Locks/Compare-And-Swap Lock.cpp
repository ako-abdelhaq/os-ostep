#include <iostream>
#include <atomic>
#include <thread>
#include <vector>

class SpinLock {
private:
    std::atomic<bool> flag;

public:
    SpinLock() : flag(false) {}

    void lock() {
        bool expected = false;
        while (!flag.compare_exchange_strong(expected, true)) {
            expected = false; 
            /* We must reset the value of expected to 0 because when compare_exchange_strong() fails,
             It updates expected's value to the new value of the flag.
             This means that if Thread1 succeeds in holding the lock, it will update the 
             value of expected to be 1 and when compare_exchange_strong() checks for another
             thread it will act as if it calls compare_exchange_strong(1,1) and since both
             expected and flag are 1, so they're equal, compare_exchange_strong(1,1) will return 1
             meaning that the spin loop will be bypassed */

            /* Another point is we should mention is that the update of expected 
            is not atomic simply because it is a regular variable; this can lead 
            to some sort of strange behavior and unexpected output */
        }
    }

    void unlock() {
        flag.store(false);
    }
};
int s = 0;
void criticalSection(int threadID, SpinLock &spinLock) {
    std::cout << "Thread " << threadID << " attempting to acquire lock..." << std::endl;
    spinLock.lock();
    std::cout << "Thread " << threadID << " has acquired lock." << std::endl;
    std::cout << "Thread " << threadID << " is in critical section." << std::endl;

    // A working loop
    for(int i=0 ; i < 100000 ; i++){
      s++;
    }

    std::cout << "Thread " << threadID << " has released lock." << std::endl;

    spinLock.unlock();
}

int main() {
    const int numThreads = 5;
    SpinLock spinLock;
    std::vector<std::thread> threads;

    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back(criticalSection, i, std::ref(spinLock));
    }

    for (auto &thread : threads) {
        thread.join();
    }

    std::cout << s; // It should be 500000
    return 0;
}