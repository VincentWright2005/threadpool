# Thread Pool

A modern c++17 thread pool with support for task submission and future-based result retrieval.

## Features
-Fixed-size worker thread pool
-submit tasks with any callable (lambdas, functions, functors)
-Get return values using std::future
-Graceful shutdown
-Thread-safe task queue
-Auto-detects hardware concurrency

## Usage
    cpp
#include "thread-pool.hpp"

## Building
    bash
g++ -std=c++17 -02 -pthread example.cpp -o example
./example

## Requirements
-C++17 or later
-Compiler with thread support

## Implementaion Details
Built using:
    -std::thread for worker threads
    -std::mutex and std::condition_variable for synchronization
    -std::packaged_task and std::future for return values
    -std::function for type-erased task storage


