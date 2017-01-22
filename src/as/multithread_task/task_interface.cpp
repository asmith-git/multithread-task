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

#include "as/multithread_task/task_interface.hpp"
#include "as/multithread_task/task_controller.hpp"

namespace as {
	// task_interface

	task_interface::task_interface() :
		mState(STATE_INITIALISED)
	{}

	task_interface::~task_interface() {

	}

	task_interface::state task_interface::get_state() const {
		return mState;
	}

	bool task_interface::should_resume() const {
		return true;
	}

	void task_interface::execute(task_controller& aController) throw() {
		// Try to execute the function
		try{
			// Check if the task has already been paused mid-execution
			switch(mState) {
			case STATE_INITIALISED:
				mState = STATE_EXECUTING;
				on_execute(aController);
				break;
			case STATE_PAUSED:
				mState = STATE_EXECUTING;
				on_resume(aController, mPauseLocation);
				break;
			}
		// Catch an exception
		}catch(std::exception&) {
			set_exception(std::current_exception());
		}
		// If the task hasn't been paused then it is now complete
		if(mState == STATE_EXECUTING) mState = STATE_COMPLETE;
	}
}