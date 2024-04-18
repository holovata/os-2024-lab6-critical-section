#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <random>
#include <chrono>

std::mutex mtx; // Глобальний м'ютекс для синхронізації виводу

void fillMatrix(std::vector<std::vector<int>>& matrix) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(1, 10);

    for (auto& row : matrix) {
        for (auto& element : row) {
            element = dist(gen);
        }
    }
}

void multiplySegment(const std::vector<std::vector<int>>& A, const std::vector<std::vector<int>>& B, std::vector<std::vector<int>>& C, int start, int end, int m, int k, std::string func) {
    for (int index = start; index < end; ++index) {
        int i = index / k;
        int j = index % k;
        for (int x = 0; x < m; ++x) {
            C[i][j] += A[i][x] * B[x][j];
        }
        if (func == "task1") {
            // Вивід результату обчислення для кожної клітинки [i, j] одразу після обчислення
            std::lock_guard<std::mutex> lock(mtx); // Захоплення м'ютексу для синхронізованого виводу
            std::cout << "[" << i << "," << j << "]=" << C[i][j] << " ";
        }
        
    }
}

void parallelMatrixMultiply(const std::vector<std::vector<int>>& A, const std::vector<std::vector<int>>& B, std::vector<std::vector<int>>& C, int numThreads, int m, int k, std::string func) {
    std::vector<std::thread> threads;
    for (int i = 0; i < numThreads; ++i) {
        int start_idx = i * (C.size() * k) / numThreads;
        int end_idx = (i + 1) * (C.size() * k) / numThreads;
        threads.emplace_back(multiplySegment, std::cref(A), std::cref(B), std::ref(C), start_idx, end_idx, m, k, func);
    }

    for (auto& thread : threads) {
        thread.join();
    }
}

void testPerformance(int n, int m, int k) {
    double fastestTime = std::numeric_limits<double>::max();
    int optimalThreads = 0;

    for (int numThreads = 1; numThreads <= 32; ++numThreads) {
        std::vector<std::vector<int>> A(n, std::vector<int>(m));
        std::vector<std::vector<int>> B(m, std::vector<int>(k));
        std::vector<std::vector<int>> C(n, std::vector<int>(k, 0));

        fillMatrix(A);
        fillMatrix(B);

        auto start = std::chrono::high_resolution_clock::now();
        parallelMatrixMultiply(A, B, C, numThreads, m, k, "task2");
        auto finish = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double> elapsed = finish - start;
        std::cout << "Threads: " << numThreads << " - Time taken: " << elapsed.count() << " seconds.\n";

        if (elapsed.count() < fastestTime) {
            fastestTime = elapsed.count();
            optimalThreads = numThreads;
        }
    }

    std::cout << "\nFastest execution time was " << fastestTime << " seconds with " << optimalThreads << " threads.\n";
}

int main() {
    int n = 55;
    int m = 54;
    int k = 53;
    std::vector<std::vector<int>> A(n, std::vector<int>(m));
    std::vector<std::vector<int>> B(m, std::vector<int>(k));
    std::vector<std::vector<int>> C(n, std::vector<int>(k, 0));

    fillMatrix(A);
    fillMatrix(B);

    std::cout << "Demonstrating parallelism with real-time results:\n";
    parallelMatrixMultiply(A, B, C, n * k, m, k, "task1");

    std::cout << "\n";

    std::cout << "\nTesting performance with varying number of threads:\n";
    testPerformance(n, m, k);

    return 0;
}
