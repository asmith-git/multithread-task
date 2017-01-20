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
		template<class I, class F>
		class for_less_than_task : public task<void> {
		private:
			const I mMin;
			const I mMax;
			const F mFunction;
		protected:
			// Inherited from task
			void execute_for_return(task_controller&) override {
				for (I i = mMin; i < mMax; ++i) mFunction(i);
			}
		public:
			for_less_than_task(I aMin, I aMax, F aFunction) :
				mMin(aMin),
				mMax(aMax),
				mFunction(aFunction)
			{}
		};

		template<class I, class F>
		class for_less_than_equals_task : public task<void> {
		private:
			const I mMin;
			const I mMax;
			const F mFunction;
		protected:
			// Inherited from task
			void execute_for_return(task_controller&) override {
				for (I i = mMin; i <= mMax; ++i) mFunction(i);
			}
		public:
			for_less_than_equals_task(I aMin, I aMax, F aFunction) :
				mMin(aMin),
				mMax(aMax),
				mFunction(aFunction)
			{}
		};

		template<class I, class F>
		class for_greater_than_task : public task<void> {
		private:
			const I mMin;
			const I mMax;
			const F mFunction;
		protected:
			// Inherited from task
			void execute_for_return(task_controller&) override {
				for (I i = mMin; i > mMax; --i) mFunction(i);
			}
		public:
			for_greater_than_task(I aMin, I aMax, F aFunction) :
				mMin(aMin),
				mMax(aMax),
				mFunction(aFunction)
			{}
		};

		template<class I, class F>
		class for_greater_than_equals_task : public task<void> {
		private:
			const I mMin;
			const I mMax;
			const F mFunction;
		protected:
			// Inherited from task
			void execute_for_return(task_controller&) override {
				for (I i = mMin; i >= mMax; --i) mFunction(i);
			}
		public:
			for_greater_than_equals_task(I aMin, I aMax, F aFunction) :
				mMin(aMin),
				mMax(aMax),
				mFunction(aFunction)
			{}
		};

		template<class V, class F, class T, class I, class I2>
		void parallel_for(task_dispatcher& aDispatcher, V aMin, V aMax, F aFunction, size_t aBlocks, task_dispatcher::priority aPriority, I aMinFn, I2 aMaxFn) {
			std::future<void>* const futures = new std::future<void>[aBlocks];
			try{
				for(size_t i = 0; i < aBlocks; ++i) {
					task_dispatcher::task_ptr task(new T(aMinFn(i), aMaxFn(i), aFunction));
					futures[i] = aDispatcher.schedule<void>(task, aPriority);
				}
				for(size_t i = 0; i < aBlocks; ++i) futures[i].get();
			}catch (std::exception& e) {
				delete[] futures;
				throw e;
			}

			delete[] futures;
		}
	}

	template<class I, class F>
	void parallel_for_less_than(task_dispatcher& aDispatcher, I aMin, I aMax, F aFunction, uint8_t aBlocks = 4, task_dispatcher::priority aPriority = task_dispatcher::PRIORITY_MEDIUM) {
		implementation::parallel_for<I, F, implementation::for_less_than_task<I,F>>(
			aDispatcher,
			aMin,
			aMax,
			aFunction,
			aBlocks,
			aPriority,
			[=](I i)->I {
				const I range = aMax - aMin;
				const I sub_range = range / aBlocks;
				return aMin + (sub_range * i);
			},
			[=](I i)->I {
				const I range = aMax - aMin;
				const I sub_range = range / aBlocks;
				return i + 1 == aBlocks ? aMax : sub_range * (i + 1);
			}
		);
	}

	template<class I, class F>
	void parallel_for_less_than_equals(task_dispatcher& aDispatcher, I aMin, I aMax, F aFunction, uint8_t aBlocks = 4, task_dispatcher::priority aPriority = task_dispatcher::PRIORITY_MEDIUM) {
		implementation::parallel_for<I, F, implementation::for_less_than_equals_task<I, F>>(
			aDispatcher,
			aMin,
			aMax,
			aFunction,
			aBlocks,
			aPriority,
			[=](I i)->I {
				const I range = aMax - aMin;
				const I sub_range = range / aBlocks;
				return i == 0 ? aMin : aMin + (sub_range * i) + 1;
			},
			[=](int i)->I {
				const I range = aMax - aMin;
				const I sub_range = range / aBlocks;
				return i + 1 == aBlocks ? aMax : sub_range * (i + 1);
			}
		);
	}

	template<class I, class F>
	void parallel_for_greater_than(task_dispatcher& aDispatcher, I aMin, I aMax, F aFunction, uint8_t aBlocks = 4, task_dispatcher::priority aPriority = task_dispatcher::PRIORITY_MEDIUM) {
		implementation::parallel_for<I, F, implementation::for_greater_than_task<I, F>>(
			aDispatcher,
			aMin,
			aMax,
			aFunction,
			aBlocks,
			aPriority,
			[=](I i)->I {
				const I range = aMin - aMax;
				const I sub_range = range / aBlocks;
				return aMin -(sub_range * i);
			},
			[=](I i)->I {
				const I range = aMin - aMax;
				const I sub_range = range / aBlocks;
				return i + 1 == aBlocks ? aMax : aMin - (sub_range * (i + 1));
			}
		);
	}

	template<class I, class F>
	void parallel_for_greater_than_equals(task_dispatcher& aDispatcher, I aMin, I aMax, F aFunction, uint8_t aBlocks = 4, task_dispatcher::priority aPriority = task_dispatcher::PRIORITY_MEDIUM) {
		implementation::parallel_for<I, F, implementation::for_greater_than_equals_task<I, F>>(
			aDispatcher,
			aMin,
			aMax,
			aFunction,
			aBlocks,
			aPriority,
			[=](I i)->I {
				const I range = aMin - aMax;
				const I sub_range = range / aBlocks;
				return i == 0 ? aMin : aMin - (sub_range * i) - 1;
			},
			[=](I i)->I {
				const I range = aMin - aMax;
				const I sub_range = range / aBlocks;
				return i + 1 == aBlocks ? aMax : aMin - (sub_range * (i + 1));
			}
		);
	}
}

#endif