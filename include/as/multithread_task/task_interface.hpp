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
	class task_controller;

	class task_interface : public std::enable_shared_from_this<task_interface> {
	public:
		friend task_controller;

		enum state {
			STATE_INITIALISED,
			STATE_EXECUTING,
			STATE_PAUSED,
			STATE_COMPLETE
		};
	private:
		state mState;
		uint8_t mPauseLocation;
	protected:
		//virtual void on_queue(task_controller&, uint8_t) = 0;
		virtual void on_execute(task_controller&) = 0;
		virtual void on_resume(task_controller&, uint8_t) = 0;
		virtual void* get_promise() = 0;
		virtual void set_exception(std::exception_ptr) = 0;
	public:
		task_interface();
		virtual ~task_interface();

		state get_state() const;
	};
}

#endif