//my thread pool implementation from scratch.
#ifndef _Vincent_threadp
#define _Vincent_threadp
using namespace std; 
#include<condition_variable>
#include<functional>
#include<future>
#include<mutex>
#include<vector>
#include<thread>
#include<queue>
class threadpool{
	public:
		threadpool(size_t num_threads)//my constructor
		{
			for(size_t i=0;i<num_threads;i++)
				workers.emplace_back([this]{workerLoop();}); // for number of workers, each runs the worker loop
		}
		~threadpool(){
			for(size_t i =0;i<workers.size();i++) //for every thread in the vector
				workers[i].join(); //join them and then we can leave safely 
			stopped = true;
		}

		void submit(function<void()> task){ //submit task function
			{	lock_guard<mutex> lock(mtx); // locks our mutex to stop data races
				tasks.push(std::move(task)); // more efficient to use move semantics, pushes task into queue
			}
			cv.notify_one(); // wake up one worker to do the task we just submitted
		}

	private:
		vector<thread> workers; //vector storing my worker threads
		queue<function<void()>> tasks; //my queue that will store incoming tasks
							  //is wrapped so any callable can be stored
		mutex mtx; //my lock to protect data
		condition_variable cv; //The signal for work available or shutting down.
					    //Lets workers sleep efficiently
		bool stopped = false; // a flag to indicate if pool is shutting down or not.
		void workerLoop() // A loop that runs indefinetely as pool is on.
		{
			for(;;) //infinite loop, stops when pool stops
			{
				function<void()> task; //creates variable to store callable
				{ //creates a block so that the lock gets destroyed when we exit
				unique_lock<mutex>lock(mtx); //locks our mutex in
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

};
#endif
