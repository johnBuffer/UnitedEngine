#pragma once

#include <thread>
#include <mutex>
#include <list>
#include <functional>
#include <atomic>
#include <iostream>
#include <chrono>
#include <condition_variable>

#include "worker.hpp"
#include "synchronizer.hpp"

namespace swrm
{

class Swarm
{
public:
	Swarm(uint32_t thread_count)
		: m_thread_count(thread_count)
		, m_done_count(0U)
		, m_ready_count(0U)
		, m_condition()
		, m_condition_mutex()
	{
		for (uint32_t i(thread_count); i--;) {
			m_workers.emplace_back(this, i);
		}

		Synchronizer::lockAtReady(m_workers);
		Synchronizer::createThreads(m_workers);
	}

	~Swarm()
	{
		Synchronizer::stop(m_workers);
		Synchronizer::unlockAtDone(m_workers);
		Synchronizer::unlockAtReady(m_workers);
		Synchronizer::join(m_workers);
	}

	uint32_t getWorkerCount() const {
		return m_thread_count;
	}

	void execute(WorkerFunction function)
	{
		for (Worker& worker : m_workers) {
			worker.setJob(function);
		}

		start();
	}

	void waitExecutionDone()
	{
		m_ready_count = 0U;
		waitWorkersDone();
		Synchronizer::lockAtReady(m_workers);
		Synchronizer::unlockAtDone(m_workers);
	}

private:
	const uint32_t m_thread_count;

	std::atomic<uint32_t> m_done_count;
	std::atomic<uint32_t> m_ready_count;

	std::condition_variable m_condition;
	std::mutex              m_condition_mutex;
	std::list<Worker>       m_workers;

	void start()
	{
		m_done_count = 0U;
		waitWorkersReady();
		Synchronizer::lockAtDone(m_workers);
		Synchronizer::unlockAtReady(m_workers);
	}

	void notifyWorkerDone()
	{
		{
			std::lock_guard<std::mutex> lg(m_condition_mutex);
			++m_done_count;
		}
		m_condition.notify_one();
	}

	void notifyWorkerReady()
	{
		{
			std::lock_guard<std::mutex> lg(m_condition_mutex);
			++m_ready_count;
		}
		m_condition.notify_one();
	}

	void waitWorkersDone()
	{
		std::unique_lock<std::mutex> ul(m_condition_mutex);
		m_condition.wait(ul, [&] { return m_done_count == m_thread_count; });
	}

	void waitWorkersReady()
	{
		std::unique_lock<std::mutex> ul(m_condition_mutex);
		m_condition.wait(ul, [&] { return m_ready_count == m_thread_count; });
	}

	friend Worker;
};

}
