#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>

const int NUM_INCREMENTS = 1000000000; // 10^9 increments

void unsafeIncrement(int& variable) {
    for (int i = 0; i < NUM_INCREMENTS; ++i) {
        ++variable;
    }
}

void safeIncrementMutex(int& variable, std::mutex& mtx) {
    for (int i = 0; i < NUM_INCREMENTS; ++i) {
        std::lock_guard<std::mutex> lock(mtx);
        ++variable;
    }
}

void safeIncrementUniqueLock(int& variable, std::mutex& mtx) {
    for (int i = 0; i < NUM_INCREMENTS; ++i) {
        std::unique_lock<std::mutex> lock(mtx);
        ++variable;
    }
}

void safeIncrementAtomic(std::atomic<int>& variable) {
    for (int i = 0; i < NUM_INCREMENTS; ++i) {
        variable.fetch_add(1, std::memory_order_relaxed);
    }
}

int main() {
    int sharedVariable = 0;
    std::mutex mtx;
    std::atomic<int> atomicVariable(0);

    // Unsafe increment without synchronization
    auto startUnsafe = std::chrono::high_resolution_clock::now();
    std::thread t1(unsafeIncrement, std::ref(sharedVariable));
    std::thread t2(unsafeIncrement, std::ref(sharedVariable));
    t1.join();
    t2.join();
    auto endUnsafe = std::chrono::high_resolution_clock::now();
    std::cout << "Unsafe increment result: " << sharedVariable << std::endl;
    std::cout << "Unsafe increment time: " << std::chrono::duration_cast<std::chrono::milliseconds>(endUnsafe - startUnsafe).count() << " ms" << std::endl;

    // Reset shared variable
    sharedVariable = 0;

    // Safe increment with mutex
    auto startMutex = std::chrono::high_resolution_clock::now();
    std::thread t3(safeIncrementMutex, std::ref(sharedVariable), std::ref(mtx));
    std::thread t4(safeIncrementMutex, std::ref(sharedVariable), std::ref(mtx));
    t3.join();
    t4.join();
    auto endMutex = std::chrono::high_resolution_clock::now();
    std::cout << "Safe increment with mutex result: " << sharedVariable << std::endl;
    std::cout << "Safe increment with mutex time: " << std::chrono::duration_cast<std::chrono::milliseconds>(endMutex - startMutex).count() << " ms" << std::endl;

    // Reset shared variable again
    sharedVariable = 0;

    // Safe increment with unique_lock
    auto startUniqueLock = std::chrono::high_resolution_clock::now();
    std::thread t7(safeIncrementUniqueLock, std::ref(sharedVariable), std::ref(mtx));
    std::thread t8(safeIncrementUniqueLock, std::ref(sharedVariable), std::ref(mtx));
    t7.join();
    t8.join();
    auto endUniqueLock = std::chrono::high_resolution_clock::now();
    std::cout << "Safe increment with unique_lock result: " << sharedVariable << std::endl;
    std::cout << "Safe increment with unique_lock time: " << std::chrono::duration_cast<std::chrono::milliseconds>(endUniqueLock - startUniqueLock).count() << " ms" << std::endl;

    // Safe increment with atomic
    auto startAtomic = std::chrono::high_resolution_clock::now();
    std::thread t5(safeIncrementAtomic, std::ref(atomicVariable));
    std::thread t6(safeIncrementAtomic, std::ref(atomicVariable));
    t5.join();
    t6.join();
    auto endAtomic = std::chrono::high_resolution_clock::now();
    std::cout << "Safe increment with atomic result: " << atomicVariable << std::endl;
    std::cout << "Safe increment with atomic time: " << std::chrono::duration_cast<std::chrono::milliseconds>(endAtomic - startAtomic).count() << " ms" << std::endl;

    return 0;
}
