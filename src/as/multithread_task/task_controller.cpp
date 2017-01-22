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

#include "as/multithread_task/task_controller.hpp"

namespace as {
	// task_controller
	task_controller::~task_controller() {

	}

	bool task_controller::pause(task_interface& aTask, uint8_t aLocation) throw() {
		if(aTask.mState != task_interface::STATE_EXECUTING) return false;
		if(on_pause(aTask)) {
			aTask.mState = task_interface::STATE_PAUSED;
			aTask.mPauseLocation = aLocation;
			return true;
		}else {
			return false;
		}
	}

	bool task_controller::cancel(task_interface& aTask) throw() {
		if(aTask.mState != task_interface::STATE_INITIALISED) return false;
		return on_cancel(aTask);
	}

	bool task_controller::reschedule(task_interface& aTask, task_dispatcher::priority aPriority) throw() {
		if(aTask.mState != task_interface::STATE_INITIALISED) return false;
		return on_reschedule(aTask, aPriority);
	}

};