#pragma once

#include <thread>
#include <mutex>
#include <vector>
#include "condition.hpp"
#include <functional>

template<typename T>
using WorkerFunction = std::function<void(std::vector<T>&, uint32_t, uint32_t)>;

template<typename T>
struct Worker;

template<typename T>
class Swarm
{
public:
	Swarm(std::vector<T>& data, uint32_t count)
		: m_count(count)
		, m_ready(false)
		, m_processed(false)
	{
		for (uint32_t i(0); i < count; ++i) {
			m_workers.emplace_back(data, *this, i, m_count);
		}
	}

	void setJob(WorkerFunction<T> core)
	{
		for (Worker<T>& w : m_workers) {
			w.setJob(core);
		}
	}

	void notifyReady()
	{
		std::lock_guard<std::mutex> lk(m_mutex_ready);
		m_ready = true;
		m_processed = false;
		m_done_count = 0U;
		m_waiting_ready.notify_all();
	}

	void notifyWorkerDone()
	{
		std::lock_guard<std::mutex> lk(m_mutex_processed);
		++m_done_count;
		if (m_done_count == m_count) {
			m_processed = true;
			m_ready = false;
			m_waiting_processed.notify_all();
		}
	}

	void waitReady()
	{
		std::unique_lock<std::mutex> lk(m_mutex_ready);
		m_waiting_ready.wait(lk, [this] {return m_ready;});
	}

	void waitProcessed()
	{
		std::unique_lock<std::mutex> lk(m_mutex_processed);
		m_waiting_processed.wait(lk, [this] {return m_processed; });
	}

private:
	bool m_ready, m_processed;
	uint32_t m_done_count;

	const uint32_t m_count;
	std::vector<Worker<T>> m_workers;

	std::condition_variable m_waiting_ready;
	std::condition_variable m_waiting_processed;
	std::mutex m_mutex_processed;
	std::mutex m_mutex_ready;
};


template<typename T>
struct Worker
{
	Worker()
		: swarm(nullptr)
		, data(nullptr)
	{}

	Worker(WorkerFunction<T> core_, std::vector<T>& data_, Swarm<T>& swarm_, uint32_t id_, uint32_t step_)
		: swarm(&swarm_)
		, data(&data_)
		, id(id_)
		, step(step_)
		, core(core_)
	{
		worker_thread = std::thread(&Worker<T>::run, this);
	}

	Worker(std::vector<T>& data_, Swarm<T>& swarm_, uint32_t id_, uint32_t step_)
		: swarm(&swarm_)
		, data(&data_)
		, id(id_)
		, step(step_)
	{
	}

	void setJob(WorkerFunction<T> job)
	{
		core = job;
		worker_thread = std::thread(&Worker<T>::run, this);
	}

	void run()
	{
		while (true) {
			swarm->waitReady();
			
			core(*data, id, step);
			
			swarm->notifyWorkerDone();
			swarm->waitProcessed();
		}
	}

private:
	uint32_t id, step;
	Swarm<T>* swarm;
	std::vector<T>* data;
	std::thread worker_thread;

	WorkerFunction<T> core;
};
