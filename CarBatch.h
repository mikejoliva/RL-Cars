#ifndef CAR_BATCH_H
#define CAR_BATCH_H

#include <thread>
#include <vector>
#include <functional>

class CarBatch
{
private:

	bool stopThread = false;
	std::vector<std::function<void()>> carfunc;
	size_t size;


public:
	CarBatch(std::vector<std::function<void()>>&);
	CarBatch(const CarBatch&);
	CarBatch() : size(0) { /* Empty */ }

	void stop();
	void start();

	void run();
};

#endif
