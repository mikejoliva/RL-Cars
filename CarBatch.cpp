#include "CarBatch.h"

void CarBatch::run()
{
	while (!stopThread)
		for (size_t idx = 0; idx < size; ++idx)
			std::invoke(carfunc[idx]);
}

CarBatch::CarBatch(std::vector<std::function<void()>>& f) :
	carfunc(f)
{
	size = carfunc.size();
}

CarBatch::CarBatch(const CarBatch& b) :
	carfunc(b.carfunc),
	size(b.size)
{
}

void CarBatch::stop()
{
	stopThread = true;
}

void CarBatch::start()
{
	stopThread = false;
	run();
}
