#include "worker.hpp"

#include "swarm.hpp"


namespace swrm
{

Worker::Worker(Swarm* swarm)
	: m_swarm(swarm)
	, m_group(nullptr)
	, m_id(0)
	, m_group_size(0)
	, m_running(true)
	, m_ready_mutex()
	, m_done_mutex()
{
}

void Worker::createThread()
{
	lockReady();
	m_thread = std::thread(&Worker::run, this);
}

void Worker::run()
{
	while (true) {
		waitReady();

		if (!m_running) {
			break;
		}
		
		m_job(m_id, m_group_size);

		waitDone();
	}
}

void Worker::lockReady()
{
	m_ready_mutex.lock();
}

void Worker::unlockReady()
{
	m_ready_mutex.unlock();
}

void Worker::lockDone()
{
	m_done_mutex.lock();
}

void Worker::unlockDone()
{
	m_done_mutex.unlock();
}

void Worker::setJob(uint32_t id, ExecutionGroup* group)
{
	m_id = id;
	m_job = group->m_job;
	m_group_size = group->m_group_size;
	m_group = group;
}

void Worker::stop()
{
	m_running = false;
}

void Worker::join()
{
	m_thread.join();
}

void Worker::waitReady()
{
	m_swarm->notifyWorkerReady(this);
	lockReady();
	unlockReady();
}

void Worker::waitDone()
{
	m_group->notifyWorkerDone();
	lockDone();
	unlockDone();
}

}
