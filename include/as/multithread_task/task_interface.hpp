#ifndef ASMITH_TASK_INTERFACE_HPP
#define ASMITH_TASK_INTERFACE_HPP

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

namespace as {
	class task_controller;
	class task_dispatcher;

	
	class task_interface {
	public:
		friend class task_dispatcher;
	protected:
		/*!
			\brief Return the address of the std::promise<?> object associated with the state of this task.
			\detail The template type of the std::promise is dependant on the return type of the wrapperd function.
			\return The address of the promise object.
		*/
		virtual void* get_promise() = 0;
	public:
		/*!
			\brief Destroy the task.
		*/
		virtual ~task_interface() {}
		
		/*!
			\brief Execute the task.
			\param aControl The task_controller of the executing thread.
			\see task_dispatcher
		*/
		virtual void execute(task_controller&) = 0;
	};
}

#endif