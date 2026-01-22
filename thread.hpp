#ifndef THREAD_HPP
#define THREAD_HPP

#include <functional>
#include <queue>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

struct TaskQueue
{
	std::queue<std::function<void()>> tasks;
	std::mutex mutex_;// prevents shared queue data races
	std::atomic<int> remaining_tasks = 0; // lockfree datatype that will increment the number of tasks
	//std::condition_variable cv;

	std::atomic<bool> stop_flag = false;
					      

	void addTask(std::function<void()>&& callback)
	{
		{
			std::lock_guard<std::mutex> lock_guard{mutex_}; // lock the mutex until the task is fully added to the queue then unlock once out of scope
			tasks.push(std::move(callback));
			remaining_tasks++;
		}
	}

	void getTask(std::function<void()>& task)
	{
		std::lock_guard<std::mutex> lock_guard{mutex_};
		if (tasks.empty()) return;
		task = std::move(tasks.front());
		tasks.pop();
	}

	//instead of idling and wasting cpu, use a condition variable to change the task based on state not waiting for workers to finish
	void waitUntilDone() 
	{
		while(remaining_tasks > 0)
		{
			std::this_thread::yield();
		}
	}

	void completeTask()
	{
		remaining_tasks--;
	
	}

};


struct Thread
{
	int id = 0;
	std::thread cur_thread;
	std::function<void()> task = nullptr;
	bool running = true;
	TaskQueue* t_queue = nullptr;

	Thread (TaskQueue* tq, int id_) :
		id{id_},
		t_queue{tq}
	{
		cur_thread = std::thread([this]()
				{
					run();
				});
	}

	void run()
	{
		while (!t_queue->stop_flag)
		{
			t_queue->getTask(task);	
			if (task == nullptr) std::this_thread::yield();
			else
			{
				task();
				t_queue->completeTask();
				task = nullptr;
			
			}	
		}
	
	}

	void stop()
	{
		t_queue->stop_flag = true;
		cur_thread.join();
	}

};

struct Threader
{
	int num_threads = 1;
	TaskQueue t_queue;
	std::vector<Thread> threads;


	Threader(unsigned int number_threads) : 
		num_threads{number_threads}
	{
		threads.reserve(num_threads);
		for(int i = 0; i < num_threads; i++)
		{
			threads.emplace_back(&t_queue, i);
		}
	}
	
	void parallel(int num_objects, std::function<void(int start, int end)>&& callback)
	{
		int slice_size = num_objects / num_threads;
		for (int i = 0; i < num_threads; i++)
		{
			int start = i * slice_size;
			int end = start + slice_size;
			t_queue.addTask([start, end, &callback](){ callback(start, end);});
		}
		if (slice_size * num_threads < num_objects)
		{
			int start = slice_size * num_threads;
			callback(start, num_objects);
		}
		t_queue.waitUntilDone();
	}

};


#endif
