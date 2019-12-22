#pragma once

#include <mutex>


namespace swrm
{

template<typename T>
class SyncAdd
{
public:
	SyncAdd(T& container)
		: m_container(container)
	{}

	void push_back(WorkerPtr worker)
	{
		std::lock_guard<std::mutex> lg(m_mutex);
		m_container.push_back(worker);
	}

private:
	T& m_container;
	std::mutex m_mutex;
};

}
