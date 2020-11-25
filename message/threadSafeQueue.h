#ifndef THREAD_SAFE_QUEUE_H
#define THREAD_SAFE_QUEUE_H

#include <mutex>
#include <queue>
#include <condition_variable>

namespace utility 
{
	template<typename T> 
	class threadsafeQueue 
	{
	public:
		threadsafeQueue() {}
		threadsafeQueue(threadsafeQueue const& other) 
		{
			std::lock_guard<std::mutex> lock(mutex);
			dataQueue = other.dataQueue;
		}

		~threadsafeQueue() {}

		void push(T value) 
		{
			std::lock_guard<std::mutex> lock(mutex);
			dataQueue.push();
			dataCondition.notify_one();
		}

		bool waitAndPop(T& value) 
		{
			std::unique_lock<std::mutex> lock(mutex);
			dataCondition.wait(lock);
			if (dataQueue.empty()) { return false; }
			value = dataQueue.front();
			dataQueue.pop();
			return true;
		}

		std::shared_ptr<T> waitAndPop() 
		{
			std::unique_lock<std::mutex> lock(mutex);
			dataCondition.wait(lock);
			if (dataQueue.empty()) { return nullptr; }
			std::shared_ptr<T> res(std::make_shared<T>(dataQueue.front()));
			dataQueue.pop();
			return res;
		}

		bool tryPop(T& value) 
		{
			std::lock_guard<std::mutex> lock(mutex);
			if (dataQueue.empty()) { return false; }
			value = dataQueue.front();
			dataQueue.pop();
			return true;
		}
		
		std::shared_ptr<T> tryPop() 
		{
			std::lock_guard<std::mutex> lock(mutex);
			if (dataQueue.empty()) { return std::shared_ptr<T>(); }
			std::shared_ptr<T> res(std::make_shared<T>(dataQueue.front()));
			dataQueue.pop();
			return res;
		}

		size_t size() const 
		{
			std::lock_guard<std::mutex> lock(mutex);
			dataQueue.size();
		}

		bool empty() const 
		{
			std::lock_guard<std::mutex> lock(mutex);
			return dataQueue.empty();
		}
		void clear() 
		{
			std::lock_guard<std::mutex> lock(mutex);
			std::queue<T> empty;
			dataQueue.swap(empty);
		}

		void wake()
		{
			dataCondition.notify_one();
		}

	private:
		mutable std::mutex mutex;
		std::queue<T> dataQueue;
		std::condition_variable dataCondition;

	};
}
#endif // !THREAD_SAFE_QUEUE_H

