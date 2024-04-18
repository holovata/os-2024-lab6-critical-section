#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

const int TARGET = 1000;

std::mutex mtx;
std::condition_variable cv;
bool turn = true;

// Додавання змінної для відстеження активності потоків
int active_threads = 1;

void syncIncrement(int& sharedVariable, int& localVariable, bool myTurn) {
    std::unique_lock<std::mutex> lock(mtx);
    while (localVariable < TARGET && sharedVariable < TARGET) {
        // Debug output before waiting
        std::cout << "Thread " << std::this_thread::get_id() << " is waiting. LocalVariable = " << localVariable << ", SharedVariable = " << sharedVariable << std::endl;
        // Чекати на свою чергу лише якщо є інші активні потоки
        if (active_threads > 1) {
            cv.wait(lock, [&]() { return turn == myTurn && sharedVariable < TARGET; });
        }
        // Debug output after being allowed to proceed
        std::cout << "Thread " << std::this_thread::get_id() << " is proceeding. LocalVariable = " << localVariable << ", SharedVariable = " << sharedVariable << std::endl;
        if (sharedVariable < TARGET) {
            ++sharedVariable;
            ++localVariable;
            // Debug output after increment
            std::cout << "Thread " << std::this_thread::get_id() << " incremented. LocalVariable = " << localVariable << ", SharedVariable = " << sharedVariable << std::endl;
        }
        turn = !turn;
        cv.notify_one();
    }
    // Debug output when finishing
    std::cout << "Thread " << std::this_thread::get_id() << " has finished. LocalVariable = " << localVariable << std::endl;
}


int main() {
    int sharedVariable = 0;
    int localVariable1 = 0;
    int localVariable2 = 0;

    std::thread t1(syncIncrement, std::ref(sharedVariable), std::ref(localVariable1), true);
    std::thread t2(syncIncrement, std::ref(sharedVariable), std::ref(localVariable2), false);

    t1.join();
    t2.join();

    std::cout << "Final value of shared variable: " << sharedVariable << std::endl;
    std::cout << "Final value of local variable 1: " << localVariable1 << std::endl;
    std::cout << "Final value of local variable 2: " << localVariable2 << std::endl;

    return 0;
}