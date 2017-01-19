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
		//! \todo Add as many threads as there are CPU cores
		mThreads.push_back(std::thread(&thread_pool::worker_function, this));
	}

	thread_pool::thread_pool(size_t aThreads) :
		mExit(false)
	{
		//! \todo Add as many threads as in aThreads
	}

	thread_pool::~thread_pool() {
		mExit = true;
		//! \todo Notify all worker threads waiting for tasks
		for(std::thread& i : mThreads) i.join();
	}

	void thread_pool::schedule_task(task_ptr aTask) {
		mTasksLock.lock();
		task_ptr task = mTasks.front();
		mTasks.push_back(aTask);
		//! \todo Notify a worker thread that a task has been added
		mTasksLock.unlock();
	}

	void thread_pool::worker_function() {
		task_controller controller;
		while(! mExit) {
			//! \todo Wait for a task to be added
			//if(mExit) break;
			mTasksLock.lock();
			task_ptr task = mTasks.front();
			mTasks.pop_front();
			mTasksLock.unlock();
			task->execute(controller);
		}
	}

}