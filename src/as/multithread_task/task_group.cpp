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

#include "as/multithread_task/task_group.hpp"

namespace as {
	// task_group

	task_group::~task_group() {
		wait();
	}

	void task_group::wait() {
		for(std::shared_ptr<task_wrapper>& i : mWrappers) i->wait();
		mWrappers.clear();
	}

	void task_group::schedule(task_dispatcher& aDispatcher, task_dispatcher::priority aPriority) {
		for(std::shared_ptr<task_wrapper>& i : mWrappers) i->schedule(aDispatcher, aPriority);
	}

	std::future_status task_group::wait_for_ms(const std::chrono::milliseconds& aDuration) {
		//! \todo Implement
		return std::future_status::timeout;
	}

	std::future_status task_group::wait_until_ms(const std::chrono::milliseconds& aDuration) {
		//! \todo Implement
		return std::future_status::timeout;
	}


}