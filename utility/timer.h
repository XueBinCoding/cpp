#ifndef TIMER_H
#define TIMER_H
#include <functional>
#include <thread>
#include <mutex>
#include <map>
#include <unordered_map>

namespace utility
{
	typedef std::function<bool(void)> timerEvent;
	typedef uint32_t timerId;
	class timer
	{
	public:
		timer(const timerEvent& event, uint32_t msec) 
		:event_callback(event),interval(msec)
		{
			if (interval == 0) { interval = 1; }
		}

		timer() {}

		static void Sleep(uint32_t mesc) 
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(mesc));
		}

		void SetEventCallback(const timerEvent& event) 
		{
			event_callback = event;
		}

		void start(int microseconds,bool repeat=false) 
		{
			isRepeat = repeat;
			auto timeBegin = std::chrono::high_resolution_clock::now();
			int64_t elapsed = 0;
			do 
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(microseconds - elapsed));
				timeBegin = std::chrono::high_resolution_clock::now();
				event_callback();
				elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - timeBegin).count();
				if (elapsed < 0) {elapsed = 0;}
			} while (isRepeat);
		}

		void stop() { isRepeat = false; }
	private:

		friend class timerQueue;
		void SetNextTimeout(int64_t timePoint) 
		{
			nextTimeout = timePoint + interval;
		}

		int64_t getNextTimeout() const
		{
			return nextTimeout;
		}
		bool isRepeat = false;
		timerEvent event_callback = [] {return false; };
		timerId interval =0;
		int64_t nextTimeout = 0;
	};

	class timerQueue 
	{
	public:
		timerId addTimer(const timerEvent& event,uint32_t msec);
		void removeTimer(timerId timerId);
		int64_t getTimerRemaining();
		void HandleTimerEvent();
	private:
		int64_t getTimeNow();
		std::mutex mutex;
		std::unordered_map<timerId, std::shared_ptr<timer>> timers;
		std::map<std::pair<int64_t, timerId>, std::shared_ptr<timer>> events;
		uint32_t lastTimerId = 0;
		uint32_t timeRemain = 0;
	};
}
#endif // !TIMER_H