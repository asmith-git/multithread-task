#ifndef ASMITH_PARALLEL_FOR_TASK_HPP
#define ASMITH_PARALLEL_FOR_TASK_HPP

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

#include "task_dispatcher.hpp"
#include "task.hpp"

namespace as {
	namespace implementation {
		template<class F>
		class for_less_than_task : public task<void> {
		private:
			const int mMin;
			const int mMax;
			const F mFunction;
		protected:
			// Inherited from task
			void execute_for_return(task_controller&) override {
				for (int i = mMin; i < mMax; ++i) mFunction(i);
			}
		public:
			for_less_than_task(int aMin, int aMax, F aFunction) :
				mMin(aMin),
				mMax(aMax),
				mFunction(aFunction)
			{}
		};

		template<class F>
		class for_less_than_equals_task : public task<void> {
		private:
			const int mMin;
			const int mMax;
			const F mFunction;
		protected:
			// Inherited from task
			void execute_for_return(task_controller&) override {
				for (int i = mMin; i <= mMax; ++i) mFunction(i);
			}
		public:
			for_less_than_equals_task(int aMin, int aMax, F aFunction) :
				mMin(aMin),
				mMax(aMax),
				mFunction(aFunction)
			{}
		};

		template<class F>
		class for_greater_than_task : public task<void> {
		private:
			const int mMin;
			const int mMax;
			const F mFunction;
		protected:
			// Inherited from task
			void execute_for_return(task_controller&) override {
				for (int i = mMin; i > mMax; --i) mFunction(i);
			}
		public:
			for_greater_than_task(int aMin, int aMax, F aFunction) :
				mMin(aMin),
				mMax(aMax),
				mFunction(aFunction)
			{}
		};

		template<class F>
		class for_greater_than_equals_task : public task<void> {
		private:
			const int mMin;
			const int mMax;
			const F mFunction;
		protected:
			// Inherited from task
			void execute_for_return(task_controller&) override {
				for (int i = mMin; i >= mMax; --i) mFunction(i);
			}
		public:
			for_greater_than_equals_task(int aMin, int aMax, F aFunction) :
				mMin(aMin),
				mMax(aMax),
				mFunction(aFunction)
			{}
		};

		template<class F, class T>
		void parallel_for_less(task_dispatcher& aDispatcher, int aMin, int aMax, F aFunction, int aBlocks, task_dispatcher::priority aPriority) {
			//! \todo Implement

			//const int range = aMax - aMin;
			//const int sub_range = range / aBlocks;

			//std::future<void>* const futures = new std::future<void>[aBlocks];
			//try{
			//	for(int i = 0; i < aBlocks; ++i) {
			//		task_dispatcher::task_ptr task(new T(sub_range * i, i + 1 == aBlocks ? aMax : sub_range * (i + 1), aFunction));
			//		futures[i] = aDispatcher.schedule<void>(task, aPriority);
			//	}
			//	for(int i = 0; i < aBlocks; ++i) futures[i].get();
			//}catch (std::exception& e) {
			//	delete[] futures;
			//	throw e;
			//}

			//delete[] futures;
		}

		template<class F, class T>
		void parallel_for_greater(task_dispatcher& aDispatcher, int aMin, int aMax, F aFunction, int aBlocks, task_dispatcher::priority aPriority) {
			//! \todo Implement
		}
	}

	template<class F>
	void parallel_for_less_than(task_dispatcher& aDispatcher, int aMin, int aMax, F aFunction, int aBlocks = 4, task_dispatcher::priority aPriority = task_dispatcher::PRIORITY_MEDIUM) {
		implementation::parallel_for_less< F, implementation::for_less_than_task<F>>(
			aDispatcher,
			aMin,
			aMax,
			aFunction,
			aBlocks,
			aPriority
		);
	}

	template<class F>
	void parallel_for_less_than_equals(task_dispatcher& aDispatcher, int aMin, int aMax, F aFunction, int aBlocks = 4, task_dispatcher::priority aPriority = task_dispatcher::PRIORITY_MEDIUM) {
		implementation::parallel_for_less< F, implementation::for_less_than_equals_task<F>>(
			aDispatcher,
			aMin,
			aMax,
			aFunction,
			aBlocks,
			aPriority
		);
	}

	template<class F>
	void parallel_for_greater_than(task_dispatcher& aDispatcher, int aMin, int aMax, F aFunction, int aBlocks = 4, task_dispatcher::priority aPriority = task_dispatcher::PRIORITY_MEDIUM) {
		implementation::parallel_for_greater< F, implementation::for_greater_than_task<F>>(
			aDispatcher,
			aMin,
			aMax,
			aFunction,
			aBlocks,
			aPriority
		);
	}

	template<class F>
	void parallel_for_greater_than_equals(task_dispatcher& aDispatcher, int aMin, int aMax, F aFunction, int aBlocks = 4, task_dispatcher::priority aPriority = task_dispatcher::PRIORITY_MEDIUM) {
		implementation::parallel_for_greater< F, implementation::for_greater_than_equals_task<F>>(
			aDispatcher,
			aMin,
			aMax,
			aFunction,
			aBlocks,
			aPriority
		);
	}
}

#endif