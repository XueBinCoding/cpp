#include "timer.h"

namespace utility 
{
	timerId timerQueue::addTimer(const timerEvent&event, uint32_t ms) 
	{
		std::lock_guard<std::mutex> locker(mutex);
		int64_t timeout = getTimeNow();
		timerId mTimerId = ++lastTimerId;

		auto mTimer = std::make_shared<timer>(event, ms);
		mTimer->SetNextTimeout(timeout);
		timers.emplace(mTimerId, mTimer);
		events.emplace(std::pair<int64_t, timerId>(timeout + ms, mTimerId), std::move(mTimer));
		return mTimerId;
	}
	
	void timerQueue::removeTimer(timerId mTimerId)
	{
		std::lock_guard<std::mutex>locker(mutex);
		auto iter = timers.find(mTimerId);
		if (iter != timers.end()) 
		{
			int64_t timeout = iter->second->getNextTimeout();
			events.erase(std::pair<int64_t, timerId>(timeout, mTimerId));
			timers.erase(mTimerId);
		}
	}
	int64_t timerQueue::getTimeNow() 
	{
		auto timePoint = std::chrono::steady_clock::now();
		return std::chrono::duration_cast<std::chrono::milliseconds>(timePoint.time_since_epoch()).count();
	}
}

