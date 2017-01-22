#ifndef ASMITH_TASK_GROUP_HPP
#define ASMITH_TASK_GROUP_HPP

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

#include <vector>
#include "task_dispatcher.hpp"

namespace as {
	class task_group {
	private:
		class task_wrapper {
		public:
			virtual ~task_wrapper() {}

			virtual void wait() = 0;
			virtual std::future_status wait_for(const std::chrono::milliseconds&) = 0;
			virtual std::future_status wait_until(const std::chrono::milliseconds&) = 0;
			virtual void set_future(void*) = 0;
			virtual void set_return(void*) = 0;
			virtual task_dispatcher::task_ptr get_task() const = 0;
		};

		template<class T>
		class task_wrapper_2 : public task_wrapper {
		private:
			std::shared_ptr<task<T>> mTask;
			std::future<T> mFuture;
			T* mReturn;
		public:
			task_wrapper_2() :
				mReturn(nullptr)
			{}

			// Inherited from task_wrapper

			void wait() override {
				mFuture.wait();
				if(mReturn) *mReturn = mFuture.get();
			}

			std::future_status wait_for(const std::chrono::milliseconds& aDuration) {
				const std::future_status tmp = mFuture.wait_for(aDuration);
				if(tmp == std::future_status::ready && mReturn) *mReturn = mFuture.get();
				return tmp;
			}

			std::future_status wait_until(const std::chrono::milliseconds& aDuration) {
				const std::future_status tmp = mFuture.wait_until(aDuration);
				if(tmp == std::future_status::ready && mReturn) *mReturn = mFuture.get();
				return tmp;
			}

			void set_future(void* aFuture) {
				mFuture = *static_cast<std::future<T>*>(aFuture);
			}

			void set_return(void* aPtr) {
				mReturn = static_cast<T*>(aPtr);
			}

			task_dispatcher::task_ptr get_task() const {
				return mTask;
			}
		};

		template<>
		class task_wrapper_2<void> : public task_wrapper {
		private:
			task_dispatcher::task_ptr mTask;
			std::future<void> mFuture;
		public:
			// Inherited from task_wrapper

			void wait() override {
				mFuture.wait();
			}

			std::future_status wait_for(const std::chrono::milliseconds& aDuration) {
				return mFuture.wait_for(aDuration);
			}

			std::future_status wait_until(const std::chrono::milliseconds& aDuration) {
				return mFuture.wait_until(aDuration);
			}

			void set_future(void* aFuture) {
				mFuture = *static_cast<std::future<void>*>(aFuture);
			}

			void set_return(void* aPtr) {
				
			}

			task_dispatcher::task_ptr get_task() const {
				return mTask;
			}
		};

		std::vector<std::shared_ptr<task_wrapper>> mWrappers;

		std::future_status wait_for_ms(const std::chrono::milliseconds&);
		std::future_status wait_until_ms(const std::chrono::milliseconds&);
	public:
		~task_group();

		void wait();
		void add(task_dispatcher::task_ptr);
		void schedule(task_dispatcher&);

		template<class R, class P>
		std::future_status wait_for(const std::chrono::duration<R, P>& aPeriod) {
			//! \todo Implement
			return std::future_status::timeout;
		}

		template<class R, class P>
		std::future_status wait_until(const std::chrono::duration<R, P>& aPeriod) {
			//! \todo Implement
			return std::future_status::timeout;
		}

		template<class T>
		void add(std::shared_ptr<task<T>> aTask, T* aReturn = nullptr) {
			//! \todo Implement
		}
	};
}

#endif