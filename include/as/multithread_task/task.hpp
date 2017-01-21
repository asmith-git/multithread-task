#ifndef ASMITH_TASK_HPP
#define ASMITH_TASK_HPP

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

#include <future>
#include "task_interface.hpp"

namespace as {

	template<class T>
	class task : public task_interface{
	private:
		std::promise<T> mPromise;
	protected:
		void set_return(const T& aValue) {
			mPromise.set_value(aValue);
		}

		// Inherited from task_interface

		void* get_promise() override {
			return &mPromise;
		}
	public:
		virtual ~task() {}
	};

	template<>
	class task<void> : public task_interface {
	private:
		std::promise<void> mPromise;
	protected:
		void set_return() {
			mPromise.set_value();
		}

		// Inherited from task_interface

		void* get_promise() override {
			return &mPromise;
		}
	public:
		virtual ~task() {}
	};
}

#endif