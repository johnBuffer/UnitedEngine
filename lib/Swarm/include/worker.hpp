#pragma once

#include <inttypes.h>
#include <thread>
#include <mutex>

namespace swrm
{

class Swarm;

using WorkerFunction = std::function<void(uint32_t, uint32_t)>;

class Worker
{
public:
	Worker();
	Worker(Swarm* swarm, uint32_t worker_id);

	void createThread();

	void lockReady();
	void lockDone();
	void unlockReady();
	void unlockDone();

	void setJob(WorkerFunction job);
	void stop();
	void join();

private:
	bool m_running;
	const uint32_t m_id;
	const uint32_t m_worker_count;

	Swarm* m_swarm;
	std::thread m_thread;
	WorkerFunction m_job;

	std::mutex m_ready_mutex;
	std::mutex m_done_mutex;

	void run();
	void waitReady();
	void waitDone();
};

}
