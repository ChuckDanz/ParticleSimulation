#ifndef THREAD_HPP
#define THREAD_HPP

#include <functional>
#include <queue>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>

struct TaskQueue
{
	std::queue<std::function<void()>> tasks;
	std::mutex mutex_;// prevents shared queue data races
	std::atomic<int> remaining_tasks = 0; // lockfree datatype that will increment the number of tasks
	std::condition_variable cv;

	std::atomic<bool> stop_flag = false;
					      

	void addTask(std::function<void()>&& callback)
	{
		{
			std::lock_guard<std::mutex> lock_guard{mutex_}; // lock the mutex until the task is fully added to the queue then unlock once out of scope
			tasks.push(std::move(callback));
			remaining_tasks++;
		}
		cv.notify_one();
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
		std::unique_lock<std::mutex> lock(mutex_);
		cv.wait(lock, [&]
				{
					return remaining_tasks == 0;
				});
	}

	void completeTask()
	{
		remaining_tasks--;
		cv.notify_all();
	
	}

};


struct Thread
{
	int id = 0;
	std::thread cur_thread;
	std::function<void()> task = nullptr;
	bool running = true;
	TaskQueue* t_queue = nullptr;

	Thread (TaskQueue& tq, int id_) :
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
		while (true)
		{
			std::unique_lock<std::mutex> lock(t_queue->mutex_);
			t_queue->cv.wait(lock, [this]()
					{
						return !t_queue->tasks.empty() || t_queue->stop_flag;
					});
			if (t_queue->stop_flag && t_queue->tasks.empty()) break;
			auto task = std::move(t_queue->tasks.front());
			t_queue->tasks.pop();
			lock.unlock();

			task();
			t_queue->completeTask();
		
		}
	
	}

	void stop()
	{
		t_queue->stop_flag = true;
		t_queue->cv.notify_all();
		cur_thread.join();
	}

};




#endif
