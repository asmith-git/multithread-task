#ifndef ASMITH_TASK_DISPATCHER_HPP
#define ASMITH_TASK_DISPATCHER_HPP

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

#include <memory>
#include <future>
#include "task_interface.hpp"

namespace as {
	class task_dispatcher {
	public:
		typedef std::shared_ptr<task_interface> task_ptr;
	protected:
		virtual void schedule_task(task_ptr) = 0;
	public:
		virtual ~task_dispatcher() {}

		template<class R>
		std::future<R> schedule(task_ptr aTask) {
			schedule_task(aTask);
			return static_cast<std::promise<R>*>(aTask->get_promise())->get_future();
		}
	};
}

#endif