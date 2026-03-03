//testing my thread pool with examples to ensure logic is correct.
//basic usage first:
#include <numeric>
#include <iostream>
#include <vector>
#include "threadpool.h"
/*
int main() {
    threadpool pool(4); // 4 worker threads

    // Submit a simple task
    auto future1 = pool.submit([] {
        return 42;
    });

    std::cout << "Result: " << future1.get() << "\n";
}*/


//Submitting Multiple Tasks:

/*
int main() {
    threadpool pool(4);

    std::vector<std::future<int>> futures;

    for (int i = 0; i < 10; i++) {
        futures.push_back(
            pool.submit([i] {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                return i * i;
            })
        );
    }

    for (auto& f : futures) {
        std::cout << f.get() << " ";
    }

    std::cout << "\n";
}
*/

//Passing Arguments in

/*
int add(int a, int b) {
    return a + b;
}

int main() {
    threadpool pool(2);

    auto result = pool.submit(add, 10, 20);

    std::cout << result.get() << "\n";  // 30
}*/


//Stress Test
/*
int main() {
    threadpool pool(8);

    std::vector<int> data(1'000'000);
    std::iota(data.begin(), data.end(), 1);

    auto sum_future = pool.submit([&data] {
        long long sum = 0;
        for (auto x : data)
            sum += x;
        return sum;
    });

    std::cout << "Sum: " << sum_future.get() << "\n";
}*/

//Fire and forget tasks:

/*
int main() {
    threadpool pool(4);

    for (int i = 0; i < 5; i++) {
        pool.submit([i] {
            std::cout << "Task " << i 
                      << " running on thread "
                      << std::this_thread::get_id()
                      << "\n";
        });
    }

}
*/

#include<chrono>
//Sleep Test

int main() {
    threadpool pool(4);

    using namespace std::chrono;

    auto start = high_resolution_clock::now();

    std::vector<std::future<void>> futures;

    for (int i = 0; i < 4; i++) {
        futures.push_back(
            pool.submit([] {
                std::this_thread::sleep_for(std::chrono::seconds(2));
            })
        );
    }

    for (auto& f : futures)
        f.get();

    auto end = high_resolution_clock::now();

    std::cout << "Total time: "
              << duration_cast<seconds>(end - start).count()
              << " seconds\n";
}
