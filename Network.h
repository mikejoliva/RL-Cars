#ifndef _NETWORK_H
#define _NETWORK_H

#include <vector>

#include "EDirection.h"
#include "Layer.h"

class Network
{
private:
	std::vector<Layer*> layers;
	unsigned int generation = 0;

	Network() = delete;
public:
	Network(unsigned int, unsigned int);

	~Network();

	EDirection predictMove(std::array<int, 5>&);
	void mutate(std::vector<Layer*>);
	void setupWeights();

	std::vector<Layer*> getLayers();

	void dump();
};

#endif