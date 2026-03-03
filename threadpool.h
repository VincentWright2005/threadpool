//my thread pool implementation from scratch.
#ifndef _Vincent_threadp
#define _Vincent_threadp 
#include<type_traits>
#include<utility>
#include<condition_variable>
#include<functional>
#include<future>
#include<mutex>
#include<vector>
#include<thread>
#include<queue>
class threadpool{
	public:
		//constructor
		threadpool(std::size_t num_threads)
		{
			for(std::size_t i=0;i<num_threads;i++)
				workers.emplace_back([this]{workerLoop();}); // for number of workers, each runs the worker loop
		}





		//destructor
		~threadpool(){
			{	std::unique_lock<std::mutex> lock(mtx); // lock mutex to stop data races again 
				stopped = true;
			}
			cv.notify_all();// wake up all threads
				 //join them and then we can leave safely 
			for(auto& t: workers)
				if(t.joinable()) t.join(); //every thread joins
		}





		//submit task
		template<typename F, typename... Args> 
		auto submit(F&& f, Args&&...arg)-> std::future<std::invoke_result_t<F,Args...>>
						//return a future holding whatever the callable would
						//normally return
		{ 
		  	using ReturnType = std::invoke_result_t<F,Args...>;//creates type alias for return type
								      
			auto task = std::make_shared<std::packaged_task<ReturnType()>>(//wrapping f and args into a packaged task. 
									     //putting that into a shared pointer so it 
									     //can be excecuted by a worker later, 
									     //and return a future<ReturnType>

				[func = std::forward<F>(f),
				targs = std::make_tuple(std::forward<Args>(arg)...)]() mutable{ //to allow moving them if a lamda
					return std::apply(std::move(func),std::move(targs)); // call func with arguments inside targs
					}
			);
			std::future<ReturnType> fut= task->get_future(); // get our result
				
			{std::lock_guard<std::mutex> lock(mtx); // locks our mutex to stop data races
			if(stopped)
				throw std::runtime_error("ThreadPool: Submit() called after shutdown()"); // error case
			tasks.emplace([task=std::move(task)](){ //task is now a lambda so it can go into our queue
				(*task)();
				
			});
			}
			cv.notify_one(); // wake up one worker to do the task we just submitted
			return fut;// now we can return values that our workers get
		}

	private:
		std::vector<std::thread> workers; //vector storing my worker threads
		std::queue<std::function<void()>> tasks; //my queue that will store incoming tasks
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

};
#endif
