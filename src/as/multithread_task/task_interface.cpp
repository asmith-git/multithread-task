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
		mState(STATE_INITIALISED),
		mPauseLocation(0),
		mPauseRequest(false)
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

	

	bool task_interface::pause(task_controller& aController, uint8_t aLocation) throw() {
		if(mState != task_interface::STATE_EXECUTING) return false;
		mPauseRequest = false;
		if(aController.on_pause(*this)) {
			mState = task_interface::STATE_PAUSED;
			mPauseLocation = aLocation;
			return true;
		}else {
			return false;
		}
	}

	bool task_interface::cancel(task_controller& aController) throw() {
		if(mState != task_interface::STATE_INITIALISED) return false;
		return aController.on_cancel(*this);
	}

	bool task_interface::reschedule(task_controller& aController, implementation::task_priority aPriority) throw() {
		if(mState != task_interface::STATE_INITIALISED) return false;
		return aController.on_reschedule(*this, aPriority);
	}

	bool task_interface::reinitialise() throw() {
		if(mState != task_interface::STATE_COMPLETE) return false;
		const bool tmp = on_reinitialise();
		if(tmp) {
			mState = STATE_INITIALISED;
			mPauseLocation = 0;
			mPauseRequest = false;
			return true;
		}
		return false;
	}

	bool task_interface::is_pause_requested() const throw() {
		return mPauseRequest;
	}

	void task_interface::request_pause() throw() {
		if(mState == STATE_EXECUTING) mPauseRequest = true;
	}
}