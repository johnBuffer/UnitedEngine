#include <thread>
#include <mutex>
#include <list>
#include <functional>
#include <atomic>
#include <iostream>
#include <chrono>
#include <condition_variable>

#include "synchronizer.hpp"

namespace swrm
{

class ExecutionGroup
{
public:
	ExecutionGroup(WorkerFunction job, uint32_t group_size, std::list<Worker*>& available_workers)
		: m_job(job)
		, m_group_size(group_size)
		, m_done_count(0U)
		, m_condition()
		, m_condition_mutex()
	{
		retrieveWorkers(available_workers);
		start();
	}

	~ExecutionGroup()
	{
	}

	void start()
	{
		m_done_count = 0U;
		Synchronizer::lockAtDone(m_workers);
		Synchronizer::unlockAtReady(m_workers);
	}

	void waitExecutionDone()
	{
		waitWorkersDone();
		Synchronizer::lockAtReady(m_workers);
		Synchronizer::unlockAtDone(m_workers);
		m_workers.clear();
	}

private:
	const uint32_t       m_group_size;
	const WorkerFunction m_job;
	std::list<Worker*>   m_workers;

	std::atomic<uint32_t>   m_done_count;
	std::condition_variable m_condition;
	std::mutex              m_condition_mutex;

	void notifyWorkerDone()
	{
		{
			std::lock_guard<std::mutex> lg(m_condition_mutex);
			++m_done_count;
		}
		m_condition.notify_one();
	}

	void waitWorkersDone()
	{
		std::unique_lock<std::mutex> ul(m_condition_mutex);
		m_condition.wait(ul, [this] { return m_done_count == m_group_size; });
	}

	void retrieveWorkers(std::list<Worker*>& available_workers)
	{
		for (uint32_t i(m_group_size); i--;) {
			Worker* worker = available_workers.front();
			available_workers.pop_front();
			worker->setJob(i, this);
			m_workers.push_back(worker);
		}
	}

	friend Worker;
};


class WorkGroup
{
public:
	WorkGroup()
		: m_group(nullptr)
	{}

	WorkGroup(std::shared_ptr<ExecutionGroup> execution_group)
		: m_group(execution_group)
	{}

	void waitExecutionDone()
	{
		if (m_group) {
			m_group->waitExecutionDone();
		}
	}

private:
	std::shared_ptr<ExecutionGroup> m_group;
};

}