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

	/*!
		\brief Interface for scheduling tasks for parallel execution.
		\date 19th January 2017
		\author Adam Smith
	*/
	class task_dispatcher {
	public:
		typedef implementation::task_priority priority;		//!< Defines priority levels for scheduled tasks.
		typedef std::shared_ptr<task_interface> task_ptr;	//!< Smart pointer containing a task.
	protected:
		/*!
			\brief Schedule a task.
			\param aTask The task to schedule.
			\param aPriority The priority to schedule the task with.
		*/
		virtual void schedule_task(task_ptr, priority) = 0;
	public:
		/*!
			\brief Destroy the dispatcher.
		*/
		virtual ~task_dispatcher() {}

		/*!
			\brief Schedule a task.
			\param aTask The task to schedule.
			\param aPriority The priority to schedule the task with.
			\tparam R The return type of the task (the type of the std::promise<?> object).
		*/
		template<class R>
		std::future<R> schedule(task_ptr aTask, priority aPriority = PRIORITY_MEDIUM) {
			schedule_task(aTask, aPriority);
			return static_cast<std::promise<R>*>(aTask->get_promise())->get_future();
		}
	};
}

#endif