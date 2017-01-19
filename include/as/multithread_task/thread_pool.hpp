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
#include "task_dispatcher.hpp"

namespace as {
	class thread_pool : public task_dispatcher {
	private:
		std::vector<std::thread> mThreads;
		std::deque<task_ptr> mTasks;
		std::mutex mTasksLock;
		bool mExit;
	private:
		void worker_function();
	protected:
		// Inherited from task_dispatcher
		void schedule_task(task_ptr) override;
	public:
		thread_pool();
		thread_pool(size_t);
		~thread_pool();
	};
}

#endif