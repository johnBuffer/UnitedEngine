#pragma once

#include <thread>
#include <mutex>
#include <vector>
#include "condition.hpp"
#include <functional>
#include <atomic>
#include <SFML/System/Clock.hpp>

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
		, m_idle_count(0U)
		, m_done_count(0U)
		, m_waiting_others(0U)
	{
		for (uint32_t i(0); i < count; ++i) {
			m_workers.emplace_back(data, *this, i, m_count);
		}
	}

	~Swarm()
	{
		for (Worker<T>& w : m_workers) {
			w.stop();
		}
		notifyReady();
		for (Worker<T>& w : m_workers) {
			w.join();
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
		{
			while (m_waiting_others) {}
			std::lock_guard<std::mutex> lk(m_mutex_ready);
			m_ready = true;
			m_processed = false;
			m_others = false;
			m_waiting_others = 0U;
			m_done_count = 0U;
		}

		m_waiting_ready.notify_all();
	}

	void notifyWorkerDone()
	{
		m_mutex_ready.lock();
		++m_done_count;
		std::cout << "Done " << m_done_count << " / " << m_count << std::endl;
		if (m_done_count == m_count) {
			m_processed = true;
			m_mutex_ready.unlock();
			m_waiting_ready.notify_one();
		} else {
			m_mutex_ready.unlock();
		}
	}

	void waitReady()
	{
		std::unique_lock<std::mutex> lk(m_mutex_ready);
		m_waiting_ready.wait(lk, [&]() {return m_ready; });
	}

	void waitOthers()
	{
		std::unique_lock<std::mutex> lk(m_mutex_ready);
		m_waiting_ready.wait(lk, [&]() {return m_others; });
		std::cout << "Others OK" << std::endl;
		--m_waiting_others;
	}

	void waitProcessed()
	{
		std::unique_lock<std::mutex> lk(m_mutex_ready);
		m_waiting_ready.wait(lk, [&]() {return m_processed; });

		std::lock_guard<std::mutex> lg(m_mutex_ready);
		m_ready = false;
		m_others = true;
		m_waiting_others = m_count;
		m_waiting_ready.notify_all();
	}

private:
	bool m_ready;
	bool m_processed;
	bool m_others;
	std::atomic<uint32_t> m_idle_count;
	std::atomic<uint32_t> m_done_count;
	std::atomic<uint32_t> m_waiting_others;

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
		running = true;
		core = job;
		worker_thread = std::thread(&Worker<T>::run, this);
	}

	void run()
	{
		while (true) {
			swarm->waitReady();
			if (!running) { break; }
			core(*data, id, step);
			swarm->notifyWorkerDone();
			swarm->waitOthers();
		}
	}

	void stop()
	{
		running = false;
		std::cout << "DONE" << std::endl;
	}

	void join()
	{
		worker_thread.join();
	}

private:
	bool running;
	uint32_t id, step;
	Swarm<T>* swarm;
	std::vector<T>* data;
	std::thread worker_thread;

	WorkerFunction<T> core;
};
