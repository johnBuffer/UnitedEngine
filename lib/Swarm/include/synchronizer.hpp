#pragma once

#include "worker.hpp"
#include <list>

namespace swrm
{

class Synchronizer
{
public:
	static void lockAtReady(std::list<Worker*>& workers)
	{
		for (Worker* worker : workers) {
			worker->lockReady();
		}
	}

	static void unlockAtReady(std::list<Worker*>& workers)
	{
		for (Worker* worker : workers) {
			worker->unlockReady();
		}
	}

	static void lockAtDone(std::list<Worker*>& workers)
	{
		for (Worker* worker : workers) {
			worker->lockDone();
		}
	}

	static void unlockAtDone(std::list<Worker*>& workers)
	{
		for (Worker* worker : workers) {
			worker->unlockDone();
		}
	}

	static void stop(std::list<Worker*>& workers)
	{
		for (Worker* worker : workers) {
			worker->stop();
		}
	}

	static void join(std::list<Worker*>& workers)
	{
		for (Worker* worker : workers) {
			worker->join();
		}
	}
};

}
