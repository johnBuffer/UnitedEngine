#pragma once

#include <thread>
#include <mutex>
#include <vector>
#include "condition.hpp"

struct Worker;

class Swarm
{
public:
	Swarm(uint32_t count)
		: m_count(count)
	{
		m_workers.resize(count);
	}



private:
	const uint32_t m_count;
	std::vector<Worker> m_workers;

};


struct Worker
{
	Worker()
		: sync(nullptr)
	{}

	Worker(Condition& condition)
		: sync(&condition)
	{
	}

	void job()
	{
		while (true) {
			sync->waitReady();

			sync->notifyProcessed();
		}
	}

	Condition* sync;
};
