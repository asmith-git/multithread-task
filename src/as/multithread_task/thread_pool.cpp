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
#include "as/multithread_task/task_controller.hpp"

namespace as {
	// thread_pool

	thread_pool::thread_pool() :
		mHighPriority(PRIORITY_LOW),
		mExit(false)
	{
		// Create a worker thread for each CPU core
		const size_t cores = std::thread::hardware_concurrency();
		for (size_t i = 0; i < cores; ++i) mThreads.push_back(std::thread(&thread_pool::worker_function, this));
	}

	thread_pool::thread_pool(size_t aThreads) :
		mHighPriority(PRIORITY_LOW),
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
		mHighPriority = aPriority > mHighPriority ? aPriority : mHighPriority;
		mTasks[aPriority].push_back(aTask);
		mTasksLock.unlock();

		// Notify a waiting worker that a task has been added
		mTaskScheduled.notify_one();
	}

	void thread_pool::worker_function() {
		const auto pop_task = [this]()->task_ptr {
			for(int i = mHighPriority; i >= 0; --i) {
				mHighPriority = static_cast<priority>(i);
				if(! mTasks[i].empty()) {
					task_ptr tmp = mTasks[i].front();
					mTasks[i].pop_front();
					return tmp;
				}
			}
			return task_ptr();
		};

		class controller_t : public task_controller {
		private:
			thread_pool& mPool;
		protected:
			// Inherited from task_controller
			bool on_pause(task_interface& aTask) throw() override {
				mPool.schedule<void>(aTask.shared_from_this(), PRIORITY_MEDIUM);
				return false;
			}

			bool on_cancel(task_interface& aTask) throw() override {
				const task_ptr ptr = aTask.shared_from_this();

				mPool.mTasksLock.lock();
				for(int i = mPool.mHighPriority; i >= 0; --i) {
					auto end = mPool.mTasks[i].end();
					for(auto j = mPool.mTasks[i].begin(); j != end; ++j) {
						if(*j == ptr) {
							mPool.mTasks[i].erase(j);
							mPool.mTasksLock.unlock();
							return true;
						}
					}
				}
				mPool.mTasksLock.unlock();
				return false;
			}

			bool on_reschedule(task_interface& aTask, task_dispatcher::priority aPriority) throw()override {
				if(! on_cancel(aTask)) return false;
				mPool.schedule<void>(aTask.shared_from_this(), aPriority);
				return true;
			}
		public:
			controller_t(thread_pool& aPool) :
				mPool(aPool)
			{}
		};

		controller_t controller(*this);

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
			task.swap(pop_task());
			mTasksLock.unlock();
			if(task) controller.execute(*task);

			// Check if there are more tasks before waiting again
			do {
				task.swap(task_ptr());
				mTasksLock.lock();
				task.swap(pop_task());
				mTasksLock.unlock();
				if(task) controller.execute(*task);
			}while(task);
		}
	}

}