//my thread pool implementation from scratch.
#pragma once
#include<condition_variable>
#include<functional>
#include<future>
#include<mutex>
#include<vector>
#include<thread>
#include<queue>
class threadpool{
	public:

	private:
		std::vector<std::thread> workers; //vector storing my worker threads
		std::queue<<std::function<void()>> tasks; //my queue that will store incoming tasks
							  //is wrapped so any callable can be stored
		std::mutex mtx; //my lock to protect data
		std::condition_variable cv; //The signal for work available or shutting down.
					    //Lets workers sleep efficiently
		bool stopped = false; // a flag to indicate if pool is shutting down or not.
		void workerLoop() // A loop that runs indefinetely as pool is on.
		{
			for(;;) //infinite loop, stops when pool stops
			{
				std::function<void()> task; //creates variable to store callable
				{ //creates a block so that the lock gets destroyed when we exit
				std::unique_lock<std::mutex>lock(mtx); //locks our mutex in
				cv.wait(lock,[this]{return stopped || !tasks.empty();});
					//sleep if theres no work & pool still running
					//wake up for work or shut down to join
				if(stopped && tasks.empty()) return;
					//loop stops once out of tasks and stopped
				task = std::move(tasks.front()); 
				//task now holds the task that was at front of our queue
				tasks.pop(); //remove that task from queue
				}//exit scope, release lock
				 //it is released now so that other workers can grab there tasks while
				 //this worker runs theres below
				 task(); //worker will run the task it was given
			}
		}


