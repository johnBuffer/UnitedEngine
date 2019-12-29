#pragma once

#include <thread>
#include <mutex>
#include <list>
#include <functional>
#include <atomic>
#include <iostream>
#include <chrono>
#include <condition_variable>
#include <memory>

#include "worker.hpp"
#include "synchronizer.hpp"
#include "execution_group.hpp"
#include "write_synchronizer.hpp"

namespace swrm
{

class Swarm
{
public:
	Swarm(uint32_t thread_count)
		: m_thread_count(thread_count)
		, m_ready_count(0U)
	{
		for (uint32_t i(thread_count); i--;) {
			createWorker();
		}

		while (m_ready_count < m_thread_count) {}
	}

	~Swarm()
	{
		Synchronizer::stop(m_workers);
		Synchronizer::unlockAtReady(m_workers);
		Synchronizer::join(m_workers);
		deleteWorkers();
	}

	WorkGroup execute(WorkerFunction job, uint32_t group_size = 0)
	{
		if (!group_size) {
			group_size = m_thread_count;
		}

		if (group_size > m_available_workers.size()) {
			return WorkGroup();
		}

		return WorkGroup(std::make_unique<ExecutionGroup>(job, group_size, m_available_workers));
	}


private:
	const uint32_t m_thread_count;

	std::atomic<uint32_t> m_ready_count;
	std::list<Worker*>  m_workers;
	std::list<Worker*>  m_available_workers;
	std::mutex m_mutex;

	void createWorker()
	{
		Worker* new_worker = new Worker(this);
		new_worker->createThread();
		m_workers.push_back(new_worker);
	}

	void deleteWorkers()
	{
		for (Worker* worker : m_workers) {
			delete worker;
		}
	}

	void notifyWorkerReady(Worker* worker)
	{
		std::lock_guard<std::mutex> lg(m_mutex);
		++m_ready_count;
		m_available_workers.push_back(worker);
	}

	friend Worker;
};

}
