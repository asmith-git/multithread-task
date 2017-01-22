#ifndef ASMITH_THEAD_POOL_HPP
#define ASMITH_THEAD_POOL_HPP

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

#include <mutex>
#include <vector>
#include <deque>
#include <thread>
#include <condition_variable>
#include "task_dispatcher.hpp"

namespace as {

	/*!
		\brief A multi-thread based task dispatcher.
		\date 19th January 2017
		\author Adam Smith
	*/
	class thread_pool : public task_dispatcher {
	private:
		std::condition_variable mTaskScheduled;						//!< Notifies when a task is scheduled or the pool is being deleted.
		std::vector<std::thread> mThreads;							//!< The worker threads.
		std::deque<task_ptr> mTasks[priority::PRIORITY_HIGH + 1];	//!< The tasks that are scheduled.
		std::mutex mTasksLock;										//!< Thread-safe access to mTasks.
		priority mHighPriority;										//!< The highest priority rating that is currently scheduled.
		bool mExit;													//!< Set to true when the destructor is called.
	private:
		/*!
			\brief The task dispatch and execution loop.
			\detail Called once on each worker thread.
		*/
		void worker_function();
	protected:
		// Inherited from task_dispatcher
		void schedule_task(task_ptr, priority) override;
	public:
		/*!
			\brief Create a new thread_pool.
			\detail The number of worker threads will be equal to the reported hardware capability.
		*/
		thread_pool();

		/*!
			\brief Create a new thread_pool.
			\param aThreads The number of worker threads.
		*/
		thread_pool(size_t);

		/*!
			\brief Destroy the pool and join the worker threads.
			\detail Any still scheduled tasks will not be executed.
		*/
		~thread_pool();
	};
}

#endif