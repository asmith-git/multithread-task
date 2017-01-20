// Copyright 2017 Adam Smith
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "as/multithread_task/thread_pool.hpp"

namespace as {
	// thread_pool

	thread_pool::thread_pool() :
		mExit(false)
	{
		// Create a worker thread for each CPU core
		const size_t cores = std::thread::hardware_concurrency();
		for (size_t i = 0; i < cores; ++i) mThreads.push_back(std::thread(&thread_pool::worker_function, this));
	}

	thread_pool::thread_pool(size_t aThreads) :
		mExit(false)
	{
		// Create worker threads
		for(size_t i = 0; i < aThreads; ++i) mThreads.push_back(std::thread(&thread_pool::worker_function, this));
	}

	thread_pool::~thread_pool() {
		mExit = true;
		mTaskScheduled.notify_all();
		for(std::thread& i : mThreads) i.join();
	}

	void thread_pool::schedule_task(task_ptr aTask, priority aPriority) {
		// Add the task to the queue
		mTasksLock.lock();
		mTasks.push_back(aTask);
		mTasksLock.unlock();

		// Notify a waiting worker that a task has been added
		mTaskScheduled.notify_one();
	}

	void thread_pool::worker_function() {
		task_controller controller;
		while(! mExit) {
			// Wait for task to be added
			{
				std::unique_lock<std::mutex> lock(mTasksLock);
				mTaskScheduled.wait(lock);
			}
			if(mExit) break;

			// Execute the task
			task_ptr task;
			mTasksLock.lock();
			if (!mTasks.empty()) {
				task = mTasks.front();
				mTasks.pop_front();
			}
			mTasksLock.unlock();
			if(task) task->execute(controller);

			// Check if there are more tasks before waiting again
			do {
				task.swap(task_ptr());
				mTasksLock.lock();
				if(! mTasks.empty()) {
					task = mTasks.front();
					mTasks.pop_front();
				}
				mTasksLock.unlock();
				if(task) task->execute(controller);
			}while(task);
		}
	}

}