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

#include <exception>
#include <cstdint>
#include <memory>

namespace as {
	/*!
		\brief Base interface for implemented task-based parallel code.
		\date 19th January 2017
		\author Adam Smith
	*/
	class task_interface : public std::enable_shared_from_this<task_interface> {
	public:
		friend class task_controller;
		friend class task_dispatcher;

		enum state {				//!< Describes the current execution state of the task.
			STATE_INITIALISED,		//!< The task has been initialised and is waiting to be executed.
			STATE_EXECUTING,		//!< The task is currently executing.
			STATE_PAUSED,			//!< The task has been paused and is waiting to resume execution.
			STATE_COMPLETE			//!< The task has completed its execution.
		};
	private:
		state mState;				//!< The current state of the task
		uint8_t mPauseLocation;		//!< The location at which the task was paused
	protected:
		/*!
			\brief Called when the task is being executed.
			\detail Implement the main behaviour of the function here.
			If pausing is used then this function should simply call on_resume instead.
			\param aController The controller for the dispatcher responsible for the current execution.
			\see on_resume
		*/
		virtual void on_execute(task_controller&) = 0;
		
		/*!
			\brief Called when the task resumes execution from being paused.
			\detail Implement the main behaviour of the function here if pauseing is used.
			\param aController The controller for the dispatcher responsible for the current execution.
		*/
		virtual void on_resume(task_controller&, uint8_t) = 0;
		
		/*!
			\brief Return the address of the std::promise<?> object associated with this task's execution.
			\return The promise's address.
		*/
		virtual void* get_promise() = 0;
		
		/*!
			\brief Pass a caught exception to the std::promise<?> object associated with this task's execution.
		*/
		virtual void set_exception(std::exception_ptr) = 0;
	public:
		/*!
			\brief Create a new task.
		*/
		task_interface();
		
		/*!
			\brief Destroy the task.
		*/
		virtual ~task_interface();

		/*!
			\brief Return the current state of this task.
			\return The state.
		*/
		state get_state() const;
		
		/*!
			\brief Check if this paused task should resume execution.
			\return True if the task should resume, false if the task should remain paused.
		*/
		virtual bool should_resume() const;

		/*!
			\brief Start or resume execution of this task.
			\param aController The controller for the dispatcher responsible for the current execution.
		*/
		void execute(task_controller&) throw();
	};
}

#endif