// WRITE YOUR CODE HERE
#include <iostream>
#include <atomic>
#include <thread>
#include <vector>

class TicketLock {
private:
    std::atomic<int> ticketCounter;
    std::atomic<int> turn;

public:
    TicketLock() : ticketCounter(0), turn(0) {}

    void lock() {
        int myTicket = ticketCounter.fetch_add(1); // Fetch-And-Add
        while (turn.load(std::memory_order_relaxed) != myTicket) {
            ; // Spin-wait
        }
    }

    void unlock() {
        turn.fetch_add(1); // Move to next ticket
    }
};

int s = 0 ;

void criticalSection(int threadID, TicketLock &ticketLock) {
    std::cout << "Thread " << threadID << " attempting to acquire lock..." << std::endl;
    ticketLock.lock();
    std::cout << "Thread " << threadID << " has acquired lock." << std::endl;
    std::cout << "Thread " << threadID << " is in critical section." << std::endl;

    // A working loop
    for(int i = 0 ; i < 100000 ; i++){
        s++;
    }

    std::cout << "Thread " << threadID << " has released lock." << std::endl;
    ticketLock.unlock();
}


int main() {
    const int numThreads = 5;
    TicketLock ticketLock;
    std::vector<std::thread> threads;

    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back(criticalSection, i, std::ref(ticketLock));
    }

    for (auto &thread : threads) {
        thread.join();
    }

    std::cout << s << std::endl;

    return 0;
}