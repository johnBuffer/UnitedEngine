#pragma once

#include <condition_variable>
#include <mutex>
#include <iostream>

class Condition
{
public:
	Condition()
		: m_ready(false)
		, m_processed(false)
		, m_mutex()
	{}

	void notifyReady()
	{
		std::lock_guard<std::mutex> lk(m_mutex);
		m_processed = false;
		m_ready = true;
		m_cv.notify_one();
	}

	void waitReady()
	{
		std::unique_lock<std::mutex> lk(m_mutex);
		m_cv.wait(lk, [this] {return m_ready; });
	}

	void notifyProcessed()
	{
		std::lock_guard<std::mutex> lk(m_mutex);
		m_ready = false;
		m_processed = true;
		m_cv.notify_one();
	}

	void waitProcessed()
	{
		std::unique_lock<std::mutex> lk(m_mutex);
		m_cv.wait(lk, [this] {return m_processed; });
	}

private:
	bool m_ready, m_processed;

	std::mutex m_mutex;
	std::condition_variable m_cv;

};
