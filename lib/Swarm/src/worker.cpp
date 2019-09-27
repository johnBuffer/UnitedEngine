#include "worker.hpp"

#include "swarm.hpp"

namespace swrm
{

Worker::Worker()
	: m_swarm(nullptr)
	, m_id(0U)
	, m_worker_count(0U)
	, m_job(nullptr)
{}

Worker::Worker(Swarm* swarm, uint32_t worker_id)
	: m_swarm(swarm)
	, m_id(worker_id)
	, m_worker_count(swarm->getWorkerCount())
	, m_running(true)
	, m_ready_mutex()
	, m_done_mutex()
{
}

void Worker::createThread()
{
	m_thread = std::thread(&Worker::run, this);
}

void Worker::run()
{
	while (true) {
		waitReady();

		if (!m_running) {
			break;
		}
		
		m_job(m_id, m_worker_count);

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

void Worker::setJob(WorkerFunction job)
{
	m_job = job;
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
	m_swarm->notifyWorkerReady();
	lockReady();
	unlockReady();
}

void Worker::waitDone()
{
	m_swarm->notifyWorkerDone();
	lockDone();
	unlockDone();
}

}
