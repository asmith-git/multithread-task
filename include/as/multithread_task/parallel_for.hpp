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
	template<class T, class F, class L1, class L2>
	class parallel_for_task : public task<void> {
	private:
		const L1 mCondition;
		const L2 mIncrement;
		const F mFunction;
		const T mBegin;
		const T mEnd;
		T mIndex;
	public:
		parallel_for_task(const T aBegin, const T aEnd, const F aFunction, const L1 aCondition, const L2 aIncrement) :
			mCondition(aCondition),
			mIncrement(aIncrement),
			mFunction(aFunction),
			mBegin(aBegin),
			mEnd(aEnd),
			mIndex(aBegin)
		{}

		void on_execute(as::task_controller& aController) override {
			mIndex = mBegin;
			on_resume(aController, 0);
		}

		void on_resume(as::task_controller& aController, uint8_t aLocation) override {
			while(mCondition(mIndex, mEnd)) {
#ifndef ASMITH_DISABLE_PARALLEL_FOR_PAUSE
				if(is_pause_requested()) pause(aController, aLocation);
#endif
				mFunction(mIndex);
				mIncrement(mIndex);
			}
			set_return();
		}
	};

	namespace implementation {

		template<class V, class F, class I, class I2, class L1, class L2>
		void parallel_for(task_dispatcher& aDispatcher, V aMin, V aMax, F aFunction, size_t aBlocks, task_dispatcher::priority aPriority, I aMinFn, I2 aMaxFn, L1 aCondition, L2 aIncrement) {
			std::future<void>* const futures = new std::future<void>[aBlocks];
			try{
				for(size_t i = 0; i < aBlocks; ++i) {
					task_dispatcher::task_ptr task(new parallel_for_task<V,F,L1,L2>(aMinFn(i), aMaxFn(i), aFunction, aCondition, aIncrement));
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
	void parallel_for_less_than(task_dispatcher& aDispatcher, I aMin, I aMax, F aFunction, uint8_t aBlocks = 4, task_dispatcher::priority aPriority = task_dispatcher::priority::PRIORITY_MEDIUM) {
		implementation::parallel_for<I, F>(
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
			},
			[](const I a, const I b)->bool{ return a < b; },
			[](I& i)->void { ++i; }
		);
	}

	template<class I, class F>
	void parallel_for_less_than_equals(task_dispatcher& aDispatcher, I aMin, I aMax, F aFunction, uint8_t aBlocks = 4, task_dispatcher::priority aPriority = task_dispatcher::priority::PRIORITY_MEDIUM) {
		implementation::parallel_for<I, F>(
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
			},
			[](const I a, const I b)->bool{ return a <= b; },
			[](I& i)->void { ++i; }
		);
	}

	template<class I, class F>
	void parallel_for_greater_than(task_dispatcher& aDispatcher, I aMin, I aMax, F aFunction, uint8_t aBlocks = 4, task_dispatcher::priority aPriority = task_dispatcher::priority::PRIORITY_MEDIUM) {
		implementation::parallel_for<I, F>(
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
			},
			[](const I a, const I b)->bool{ return a > b; },
			[](I& i)->void { --i; }
		);
	}

	template<class I, class F>
	void parallel_for_greater_than_equals(task_dispatcher& aDispatcher, I aMin, I aMax, F aFunction, uint8_t aBlocks = 4, task_dispatcher::priority aPriority = task_dispatcher::priority::PRIORITY_MEDIUM) {
		implementation::parallel_for<I, F>(
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
			},
			[](const I a, const I b)->bool{ return a >= b; },
			[](I& i)->void { --i; }
		);
	}
}

#endif