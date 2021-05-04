#pragma once

#include <inttypes.h>
#include <thread>
#include <mutex>
#include <memory>
#include <functional>


namespace swrm
{

class Swarm;
class ExecutionGroup;

using WorkerFunction = std::function<void(uint32_t, uint32_t)>;

class Worker
{
public:
	Worker(Swarm* swarm);

	void createThread();

	void lockReady();
	void lockDone();
	void unlockReady();
	void unlockDone();

	void setJob(uint32_t id, ExecutionGroup* group);
	void stop();
	void join();

private:
	bool m_running;
	uint32_t m_id;
	uint32_t m_group_size;

	Swarm* m_swarm;
	ExecutionGroup* m_group;
	std::thread     m_thread;
	WorkerFunction  m_job;

	std::mutex m_ready_mutex;
	std::mutex m_done_mutex;

	void run();
	void waitReady();
	void waitDone();
};

using WorkerPtr = std::shared_ptr<Worker>;

}
