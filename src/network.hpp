#ifndef _NETWORK_H
#define _NETWORK_H

#include <vector>

#include "direction.hpp"
#include "layer.hpp"

class Network
{
private:
	std::vector<Layer*> layers;
	unsigned int generation = 0;

	Network() = delete;
public:
	Network(unsigned int, unsigned int, unsigned int, unsigned int);

	~Network();

	Direction PredictMove(std::array<int, 5>&);
	void Mutate(std::vector<Layer*>&, float range);
	void SetupWeights();

	std::vector<Layer*> GetLayers();

	void Dump();
};

#endif