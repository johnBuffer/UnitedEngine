#include <iostream>
#include "swarm.hpp"

#include <vector>
#include <ctime>



struct Compute
{
	Compute()
		: swarm(16)
	{
		const uint32_t vec_size(1000000);

		vec1.resize(vec_size);
		vec2.resize(vec_size);

		for (uint32_t i(0); i < vec_size; ++i) {
			vec1[i] = (rand() % 10);
		}

	}

	void job(uint32_t worker_id, uint32_t worker_count)
	{
		const uint32_t data_size = vec1.size();
		const uint32_t step = data_size / worker_count;
		const uint32_t start_index = worker_id * step;
		const uint32_t end_index = (worker_id == worker_count - 1) ? data_size - 1 : start_index + step;

		const uint32_t filter_width(64);

		for (uint32_t i(start_index); i < end_index; ++i) {
			float value = 0.0f;
			for (uint32_t k(0); k < filter_width; ++k) {
				if (i >= filter_width / 2 && i < end_index - filter_width / 2) {
					value += vec1[i + k - filter_width / 2];
				}
			}

			vec2[i] = value / float(filter_width);
		}
	}

	void sum()
	{
		swrm::WorkGroup group1 = swarm.execute([&](uint32_t worker_id, uint32_t worker_count) {job(worker_id, worker_count); }, 16);
		group1.waitExecutionDone();
		swrm::WorkGroup group2 = swarm.execute([&](uint32_t worker_id, uint32_t worker_count) {job(worker_id, worker_count); }, 16);
		group2.waitExecutionDone();
	}

	swrm::Swarm swarm;
	std::vector<float> vec1;
	std::vector<float> vec2;
};

int32_t main()
{
	Compute c;

	std::cout << "START" << std::endl;
	std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
	c.sum();
	std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
	std::cout << "END" << std::endl;


	std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);

	std::cout << "Elapsed time: " << time_span.count() << " s" << std::endl;

	return 0;
}