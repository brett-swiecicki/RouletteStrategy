//Brett Swiecicki
#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <thread>
#include <future>
#include <atomic>
#include <queue>
#include <mutex>
#include <condition_variable>
using namespace std;

//Thread Pool Implementation With Waitable Tasks
//Derived From C++ Concurrency In Action: Listing 9.2
//Thread Safe Queue Derived From C++ Concurrency In Action: Chapter 6, Listing 6.2
//join_threads Derived From C++ Concurrency In Action: Chapter 8, pg. 248

template<typename T>
class threadsafe_queue
{
private:
	mutable std::mutex mut;
	std::queue<T> data_queue;
	std::condition_variable data_cond;
public:
	threadsafe_queue()
	{}
	void push(T new_value)
	{
		std::lock_guard<std::mutex> lk(mut);
		data_queue.push(std::move(data));
		data_cond.notify_one();
	}
	void wait_and_pop(T& value)
	{
		std::unique_lock<std::mutex> lk(mut);
		data_cond.wait(lk, [this] {return !data_queue.empty(); });
		value = std::move(data_queue.front());
		data_queue.pop();
	}
	std::shared_ptr<T> wait_and_pop()
	{
		std::unique_lock<std::mutex> lk(mut);
		data_cond.wait(lk, [this] {return !data_queue.empty(); });
		std::shared_ptr<T> res(
			std::make_shared<T>(std::move(data_queue.front())));
		data_queue.pop();
		return res;
	}
	bool try_pop(T& value)
	{
		std::lock_guard<std::mutex> lk(mut);
		if (data_queue.empty())
			return false;
		value = std::move(data_queue.front());
		data_queue.pop();
		return true;
	}
	std::shared_ptr<T> try_pop()
	{
		std::lock_guard<std::mutex> lk(mut);
		if (data_queue.empty())
			return std::shared_ptr<T>();
		std::shared_ptr<T> res(
			std::make_shared<T>(std::move(data_queue.front())));
		data_queue.pop();
		return res;
	}
	bool empty() const
	{
		std::lock_guard<std::mutex> lk(mut);
		return data_queue.empty();
	}
};

class join_threads
{
	std::vector<std::thread>& threads;
public:
	explicit join_threads(std::vector<std::thread>& threads_) :
		threads(threads_)
	{}
	~join_threads()
	{
		for (unsigned long i = 0; i<threads.size(); ++i)
		{
			if (threads[i].joinable())
				threads[i].join();
		}
	}
};

class function_wrapper {
	struct impl_base {
		virtual void call() = 0;
		virtual ~impl_base() {}
	};
	std::unique_ptr<impl_base> impl;
	template<typename F>
	struct impl_type : impl_base {
		F f;
		impl_type(F&& f_) : f(std::move(f_)) {}
		void call() { f(); }
	};
public:
	template<typename F>
	function_wrapper(F&& f) :
		impl(new impl_type<F>(std::move(f)))
	{}
	void operator()() { impl->call(); }
	function_wrapper() = default;
	function_wrapper(function_wrapper&& other) :
		impl(std::move(other.impl))
	{}
	function_wrapper& operator=(function_wrapper&& other) {
		impl = std::move(other.impl);
		return *this;
	}
	function_wrapper(const function_wrapper&) = delete;
	function_wrapper(function_wrapper&) = delete;
	function_wrapper& operator=(const function_wrapper&) = delete;
};

class thread_pool {
	std::atomic_bool done;
	threadsafe_queue<function_wrapper> work_queue;
	std::vector<std::thread> threads;
	join_threads joiner;

	void worker_thread() {
		while (!done) {
			function_wrapper task;
			if (work_queue.try_pop(task)) {
				task();
			}
			else {
				std::this_thread::yield();
			}
		}
	} //End of worker_thread
public:

	thread_pool(unsigned num_threads) :
		done(false), joiner(threads)
	{
		unsigned const thread_count = num_threads;
		try {
			for (unsigned i = 0; i<thread_count; ++i) {
				threads.push_back(
					std::thread(&thread_pool::worker_thread, this));
			}
		}
		catch (...) {
			done = true;
			throw;
		}
	}

	~thread_pool() {
		done = true;
	}

	template<typename FunctionType>
	std::future<typename std::result_of<FunctionType()>::type> submit(FunctionType f) {
		typedef typename std::result_of<FunctionType()>::type
			result_type;
		std::packaged_task<result_type()> task(std::move(f));
		std::future<result_type> res(task.get_future());
		work_queue.push(std::move(task));
		return res;
	}
};

/*
Interfacing with this thread pool:

	This implementation of the thread pool allows you to wait until tasks submitted to the queue have finished before continuing in the
	main program. Waiting is combined with result transfer through the use of futures.

	The sumbit function returns std::future<> to hold the return value of the task and allow the caller to wait for the task to complete.
	Submit takes in argument f. f is a function or callable object that takes no parameters and returns an instance of type result_type, 
	as we just deduced.
	If result_type is void then everything should be okay.

	A callable object is something that can be called like a function, with the syntax object() or object(args); that is, a function 
	pointer, or an object of a class type that overloads operator(). The overload of operator() in your class makes it callable.

	class CallableObjectExample {
	private:
		//...
	public:
		return_type operator()() { 
			return return_type;
		}
	};

	If you don't need return values, then just call .get(); for the future of the last num_threads task(s) submitted to the queue if 
	you need to wait for the entire queue to finish before continuing.

	Constructing:
		thread_pool processing_pool(num_threads);
	4 indicates the number of threads to be utilized for the pool as defined by the user or std::thread::hardware_concurrency()

	Holding Return Values:
		std::vector<std::future<T> > futures(num_blocks-1);

	Example Submit:
		futures[i]=pool.submit(accumulate_block<Iterator,T>());

	Getting Return Values:
		futures[i].get();

*/

#endif
